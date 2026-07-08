#!/usr/bin/env bash
#
# gitpatch.sh - maintain the NTVDM patch sets through a git working repository.
#
# The line-numbered (normal/ed-style) .patch files are fragile to maintain by
# hand because every patch encodes absolute line numbers computed against the
# state produced by all earlier patches. This tool makes git the source of
# truth: it replays the existing patches into a linear git history (one commit
# per patch), and regenerates the .patch files deterministically from that
# history. You edit code in git; line numbers become a generated artifact.
#
# Runs on Linux and on Windows under Git-for-Windows bash ("Git Bash"), which
# bundles GNU patch, diff, sed and awk. No extra install required there.
#
# Subcommands:
#   order                 Print the canonical patch apply order.
#   files                 Print every file referenced by the in-scope patches.
#   bootstrap   --src DIR Build the git working repo from a source tree.
#   generate              Regenerate all .patch files from the git working repo.
#   verify                Apply the regenerated patches to base and diff vs HEAD.
#   newfiles    --src DIR List committed files absent from a pristine tree
#                         (i.e. files that must ship as-is, not as a patch).
#
# See README.md for the full workflow.

set -eu
export LC_ALL=C

# --- locate roots -----------------------------------------------------------
SELF_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
# PATCHROOT = the ntvdmpatch directory (override with GP_PATCHROOT, used by tests)
PATCHROOT="${GP_PATCHROOT:-$(cd "$SELF_DIR/../.." && pwd)}"
# Working git repo that holds the reconstructed source history.
GITREPO="${GP_GITREPO:-$PATCHROOT/.patchsrc}"
REPO="${GP_REPO:-minnt}"          # which build repo: minnt or old-src
# PATCHBIN resolution: on git-bash / msys / cygwin, prefer the bundled
# util/patch.exe over whatever `patch` is on PATH. That is the exact binary
# the autobuild scripts use, so its CRLF handling matches end-to-end -- vs.
# a `patch` from PATH (Git-for-Windows bundles a GNU patch whose text-mode
# heuristic silently strips CRs on reverse-apply, converting a CRLF source
# tree to LF and shifting line numbers downstream). GP_PATCH overrides this.
if [ -n "${GP_PATCH:-}" ]; then
  PATCHBIN="$GP_PATCH"
else
  PATCHBIN=patch
  case "${OSTYPE:-$(uname -s 2>/dev/null)}" in
    msys*|MINGW*|MSYS*|cygwin*|CYGWIN*)
      [ -x "$PATCHROOT/util/patch.exe" ] && PATCHBIN="$PATCHROOT/util/patch.exe"
      ;;
  esac
fi
DIFFBIN="${GP_DIFF:-diff}"        # GNU diff
# Which patch sets to manage:
#   core = patches/common + patches/<repo>  (pure -p2 applications; reverse
#          cleanly; this is where the line-number churn lives)
#   all  = core + experimental/*  (the experimental module patch.cmds do
#          post-patch file surgery -- rename/overlay -- that cannot be inverted
#          from a fully-built tree, so reverse-apply may fail there)
SCOPE="${GP_SCOPE:-all}"          # default all for introspection; bootstrap picks core

die() { echo "gitpatch: $*" >&2; exit 1; }
log() { echo "[gitpatch] $*" >&2; }

# --- helpers ----------------------------------------------------------------

# The patch files use two header conventions (no file mixes them):
#   1. "--- NT4\private\<path>" / "+++ nt\private\<path>"  (backslashes; majority)
#   2. "Index: nt/private/<path>" + a 67-char "===" line   (forward slashes)
# Both strip to the same -p2 relative path ("mvdm/...").

# Detect a patch's header style: prints "index" or "minus".
patch_style() {
  if grep -q '^Index: ' "$1" 2>/dev/null && ! grep -q '^--- ' "$1" 2>/dev/null
  then echo index; else echo minus; fi
}

# Emit header line(s) for a -p2 relative path in the requested style.
emit_header() { # $1 = path (forward slashes)  $2 = style
  if [ "${2:-minus}" = index ]; then
    printf 'Index: nt/private/%s\n' "$1"
    printf '%s\n' '==================================================================='
  else
    local bs; bs=$(printf '%s' "$1" | tr '/' '\\')
    printf -- '--- NT4\\private\\%s\n' "$bs"
    printf -- '+++ nt\\private\\%s\n'  "$bs"
  fi
}

# Prepare a patch for GNU patch:
#  * "--- "/"+++ " headers: backslashes -> slashes (so -p2 splits on every OS).
#  * STRUCTURAL lines (headers, Index:, "===" rule, the bare "---" change
#    separator, and "NaM" hunk commands): strip any trailing CR, otherwise the
#    CR corrupts filename detection / hunk parsing on GNU patch.
#  * CONTENT lines ("<", ">", context) are left byte-for-byte, so their CRLF (or
#    LF) still matches the worktree file and the result keeps a uniform EOL.
normalize_patch() {
  awk '
    /^--- / || /^\+\+\+ / { gsub(/\\/, "/"); sub(/\r$/, ""); print; next }
    /^Index: / || /^==/ || /^---\r?$/ || /^[0-9]/ { sub(/\r$/, ""); print; next }
    { print }
  ' "$1"
}

# Files referenced (in original order) by a single patch file, as -p2 paths.
# Handles both header styles; never fails (no grep), so it is set -e safe.
files_in_patch() {
  awk '
    /^--- /    { s=$0; sub(/^--- /,"",s);    gsub(/\\/,"/",s); sub(/[ \t\r]+$/,"",s);
                 sub(/^NT4\/private\//,"",s); print s; next }
    /^Index: / { s=$0; sub(/^Index: /,"",s); gsub(/\\/,"/",s); sub(/[ \t\r]+$/,"",s);
                 sub(/^nt\/private\//,"",s); print s; next }
  ' "$1"
}

# Emit a patch keeping or dropping the segments for a given set of files.
#   mode=drop : emit every file EXCEPT those listed (used when applying, so a
#               "frozen" file -- referenced but absent from the tree, e.g. a
#               file patched then deleted by patch.cmd -- is skipped).
#   mode=keep : emit ONLY the listed files' segments (used to lift a frozen
#               file's hunk verbatim into the regenerated patch).
# A segment = a "--- "/"Index: " header and everything up to the next header.
patch_filter() {  # $1=patchfile  $2=keep|drop  $3=file-with-one-path-per-line
  awk -v mode="$2" -v flist="$3" '
    BEGIN { while ((getline l < flist) > 0) if (l!="") sel[l]=1 }
    function pathof(line,  s) {
      s=line; sub(/^--- /,"",s); sub(/^Index: /,"",s);
      gsub(/\\/,"/",s); sub(/[ \t\r]+$/,"",s);
      sub(/^NT4\/private\//,"",s); sub(/^nt\/private\//,"",s); return s }
    /^--- / || /^Index: / { cur = (pathof($0) in sel) }
    { if ((mode=="keep") ? cur : !cur) print }
  ' "$1"
}

# Apply a patch FORWARD into a working dir. GNU patch (>=2.7) will not create a
# new file from this project's prepended-header normal-diff convention, so we
# pre-create any missing target file as empty; a "0a1,N" hunk then fills it.
# This keeps behaviour identical across GNU patch and the bundled util/patch.exe.
# apply_forward <patch> <dir> [frozenlist]
# With a frozen list, segments for frozen files are dropped before applying (and
# not pre-created), so a referenced-but-deleted file does not derail the patch.
apply_forward() {
  local pf="$1" dir="$2" frozen="${3:-}" rel
  local src="$pf"
  if [ -n "$frozen" ]; then
    src=$(mktemp); patch_filter "$pf" drop "$frozen" > "$src"
  fi
  while IFS= read -r rel; do
    [ -n "$rel" ] || continue
    if [ ! -e "$dir/$rel" ]; then
      mkdir -p "$dir/$(dirname "$rel")"
      : > "$dir/$rel"
    fi
  done < <(files_in_patch "$src")
  local rc=0
  normalize_patch "$src" | "$PATCHBIN" -s -N -p2 -d "$dir" --no-backup-if-mismatch >/dev/null 2>&1 || rc=$?
  [ -n "$frozen" ] && rm -f "$src"
  return $rc
}

# Try to REVERSE a patch fragment in $GITREPO. Dry-runs first so a fragment that
# won't fully invert leaves the tree untouched (no partial/rejected application).
# Returns 0 if reversed, 1 if it cannot be reversed.
try_reverse() {  # $1 = fragment file
  normalize_patch "$1" | "$PATCHBIN" -s -R -p2 -d "$GITREPO" --dry-run >/dev/null 2>&1 || return 1
  normalize_patch "$1" | "$PATCHBIN" -s -R -p2 -d "$GITREPO" --no-backup-if-mismatch >/dev/null 2>&1
}

# --- canonical apply order --------------------------------------------------
# Emits TAB-separated: TARGET_REL <tab> TAG <tab> NAME <tab> ABS_PATCHFILE
# TARGET_REL is the patch's path under PATCHROOT (where generate writes it back).
# TAG is the grouping label (-> commit "Repo:" trailer).
order() {
  local f base
  _emit() { printf '%s\t%s\t%s\t%s\n' "$1" "$2" "$3" "$4"; }

  for f in "$PATCHROOT"/patches/common/*.patch; do
    [ -e "$f" ] || continue; base=$(basename "$f" .patch)
    _emit "patches/common/$(basename "$f")" common "$base" "$f"
  done

  for f in "$PATCHROOT"/patches/"$REPO"/*.patch; do
    [ -e "$f" ] || continue; base=$(basename "$f" .patch)
    _emit "patches/$REPO/$(basename "$f")" "$REPO" "$base" "$f"
  done

  # Experimental modules are only in scope for SCOPE=all.
  [ "$SCOPE" = all ] || { return 0; }

  # experimental/cvidc-new
  f="$PATCHROOT/experimental/cvidc-new/cvidc-new.patch"
  [ -e "$f" ] && _emit "experimental/cvidc-new/cvidc-new.patch" cvidc-new cvidc-new "$f"
  if [ "$REPO" = minnt ]; then
    f="$PATCHROOT/experimental/cvidc-new/cvidc-minnt.patch"
    [ -e "$f" ] && _emit "experimental/cvidc-new/cvidc-minnt.patch" cvidc-new cvidc-minnt "$f"
  fi

  # experimental/haxm
  if [ "$REPO" = minnt ]; then
    for f in "$PATCHROOT"/experimental/haxm/minnt/*.patch; do
      [ -e "$f" ] || continue; base=$(basename "$f" .patch)
      _emit "experimental/haxm/minnt/$(basename "$f")" haxm "$base" "$f"
    done
    f="$PATCHROOT/experimental/haxm/haxm-dpmi.patch"
    [ -e "$f" ] && _emit "experimental/haxm/haxm-dpmi.patch" haxm haxm-dpmi "$f"
  else
    for f in "$PATCHROOT"/experimental/haxm/*.patch; do
      [ -e "$f" ] || continue; base=$(basename "$f" .patch)
      _emit "experimental/haxm/$(basename "$f")" haxm "$base" "$f"
    done
  fi

  # experimental/adlib (minnt only per patch.cmd)
  if [ "$REPO" = minnt ]; then
    for f in "$PATCHROOT"/experimental/adlib/*.patch; do
      [ -e "$f" ] || continue; base=$(basename "$f" .patch)
      _emit "experimental/adlib/$(basename "$f")" adlib "$base" "$f"
    done
  fi

  # experimental/vesa
  for f in "$PATCHROOT"/experimental/vesa/*.patch; do
    [ -e "$f" ] || continue; base=$(basename "$f" .patch)
    _emit "experimental/vesa/$(basename "$f")" vesa "$base" "$f"
  done
  return 0
}

# All referenced files across the whole apply order, de-duplicated.
all_files() {
  order | cut -f4 | while IFS= read -r p; do files_in_patch "$p"; done | sort -u
  return 0
}

git_q() { git -C "$GITREPO" "$@"; }

# Portable trailer reader (git 2.20+ lacks %(trailers:key=...) pretty format).
# trailer <commit> <key>  ->  first matching trailer value
trailer() {
  git_q show -s --format=%B "$1" | sed -n "s/^$2: //p" | sed -n '1p'
}

# --- subcommand: bootstrap --------------------------------------------------
cmd_bootstrap() {
  local SRC="" MODE=patched FORCE=0
  SCOPE=all                             # manage the COMPLETE patch set by default
  while [ $# -gt 0 ]; do
    case "$1" in
      --src) SRC="$2"; shift 2;;
      --mode) MODE="$2"; shift 2;;        # patched (default) | clean
      --scope) SCOPE="$2"; shift 2;;      # all (default) | core
      --force) FORCE=1; shift;;
      *) die "bootstrap: unknown arg $1";;
    esac
  done
  [ -n "$SRC" ] || die "bootstrap: --src DIR required (the source tree = BASEPATH, e.g. ...\\nt\\private or ...\\minnt\\base)"
  [ -d "$SRC" ] || die "bootstrap: source dir not found: $SRC"
  case "$MODE" in patched|clean) ;; *) die "bootstrap: --mode must be patched or clean";; esac
  case "$SCOPE" in core|all) ;; *) die "bootstrap: --scope must be core or all";; esac

  if [ -e "$GITREPO" ]; then
    [ "$FORCE" = 1 ] || die "bootstrap: $GITREPO exists (use --force to recreate)"
    rm -rf "$GITREPO"
  fi
  mkdir -p "$GITREPO"

  log "MODE=$MODE  SCOPE=$SCOPE  REPO=$REPO  SRC=$SRC  GITREPO=$GITREPO"
  log "PATCHBIN=$PATCHBIN  DIFFBIN=$DIFFBIN"

  # Metadata (frozen list, saved originals) lives under .git so git never tracks
  # it but generate/verify can still read it. The frozen list starts empty.
  local FROZEN; FROZEN=$(mktemp)

  # The built tree = base + ALL patches (+ post-patch file surgery). To recover
  # the base we must peel back the WHOLE stack, even when only managing a subset
  # later -- otherwise an experimental change left on top of, say, monitor.patch's
  # files makes its reverse mismatch. So copy/reverse use the full order; only the
  # forward replay (step 4) is limited to --scope.
  local copy_scope="$SCOPE"
  [ "$MODE" = patched ] && copy_scope=all

  # 1. Copy every referenced file (that exists) from the source tree. In patched
  #    mode a referenced file that is ABSENT was patched and then deleted by
  #    patch.cmd (e.g. ccpu386\evidgen.h) -- seed it as "frozen".
  log "copying referenced files from source tree..."
  local n=0 missing=0 __ctrlz=0 __outside=0
  while IFS= read -r rel; do
    [ -n "$rel" ] || continue
    # Paths that escape the .patchsrc root via '../' (e.g. monitor.patch's
    # NT4\private\..\public\sdk\inc\nt_vdd.h) can't be tracked by git -- the
    # cp destination would land outside the worktree, so the change vanishes.
    # Freeze them: their hunks are lifted verbatim from the original .patch
    # at generate time, and dropped from apply so they don't derail patch.
    if [ "${rel#../}" != "$rel" ]; then
      echo "$rel" >> "$FROZEN"
      __outside=$((__outside+1))
      continue
    fi
    if [ -f "$SRC/$rel" ]; then
      mkdir -p "$GITREPO/$(dirname "$rel")"
      cp -p "$SRC/$rel" "$GITREPO/$rel"
      # Strip DOS EOF marker (0x1A / Ctrl-Z) at ingest. util/patch.exe silently
      # removes 0x1A on forward-apply, so a base with 0x1A would diff against
      # every subsequent commit -- injecting spurious trailing "delete Ctrl-Z"
      # hunks into the regenerated .patch that then fail to apply on verify.
      # tr -d is a no-op on files without 0x1A, so this is safe globally.
      if LC_ALL=C grep -q $'\032' "$GITREPO/$rel" 2>/dev/null; then
        tr -d '\032' < "$GITREPO/$rel" > "$GITREPO/$rel.__no1a" \
          && mv "$GITREPO/$rel.__no1a" "$GITREPO/$rel"
        __ctrlz=$((__ctrlz+1))
      fi
      n=$((n+1))
    else
      missing=$((missing+1))
      [ "$MODE" = patched ] && echo "$rel" >> "$FROZEN"
    fi
  done < <(SCOPE="$copy_scope" all_files)
  [ "$__ctrlz" != 0 ]   && log "stripped Ctrl-Z (0x1A) EOF marker from $__ctrlz file(s)"
  [ "$__outside" != 0 ] && log "froze $__outside outside-tree file(s) (path starts with '../'); hunks lifted verbatim"
  sort -u "$FROZEN" -o "$FROZEN"
  log "copied $n files ($missing referenced files absent in tree)"
  [ -s "$FROZEN" ] && log "frozen (absent/deleted/outside-tree, carried verbatim): $(paste -sd' ' "$FROZEN")"

  # 2. In patched mode, run the WHOLE stack BACKWARD to recover the base. Each
  #    patch is reversed as a unit when it can be; any file segment that won't
  #    invert (deleted, or clobbered by a later module overlay like haxm's
  #    minnt\v86) is "frozen": left as-is and later carried verbatim. Freezing
  #    is global, so earlier patches skip an already-frozen file too.
  if [ "$MODE" = patched ]; then
    log "reverse-applying the full patch stack to recover base..."
    local rev abs file frag fseg
    while IFS= read -r rev; do
      abs=$(printf '%s' "$rev" | cut -f4)
      frag=$(mktemp); patch_filter "$abs" drop "$FROZEN" > "$frag"
      if [ ! -s "$frag" ]; then rm -f "$frag"; continue; fi
      if try_reverse "$frag"; then rm -f "$frag"; continue; fi
      # Whole-patch reverse failed -> isolate the offending file(s).
      while IFS= read -r file; do
        [ -n "$file" ] || continue
        grep -qxF "$file" "$FROZEN" && continue
        fseg=$(mktemp); patch_filter "$abs" keep <(printf '%s\n' "$file") > "$fseg"
        if ! try_reverse "$fseg"; then
          echo "$file" >> "$FROZEN"
          log "froze (cannot invert from built tree): $file  [${abs#$PATCHROOT/}]"
        fi
        rm -f "$fseg"
      done < <(files_in_patch "$frag")
      rm -f "$frag"
    done < <(SCOPE=all order | tac)
    sort -u "$FROZEN" -o "$FROZEN"
    # Files created purely by a patch are now empty; drop them so base lacks them.
    find "$GITREPO" -type f ! -path '*/.git/*' -empty -print -delete | while IFS= read -r e; do
      log "removed empty (patch-created) file: ${e#$GITREPO/}"
    done
  fi

  local nfrozen; nfrozen=$(wc -l < "$FROZEN" | tr -d ' ')
  log "base recovered; $nfrozen frozen file(s) will be carried verbatim"

  # 3. Initialise git and commit the base.
  git_q init -q
  git_q config user.name  "gitpatch" >/dev/null
  git_q config user.email "gitpatch@localhost" >/dev/null
  git_q config core.autocrlf false >/dev/null
  git_q config core.safecrlf false >/dev/null
  # Belt-and-braces: mark ALL files as -text so Git-for-Windows can't apply
  # CRLF<->LF translation even if the user's global core.autocrlf overrides
  # the local one. Without this, a CRLF source file may enter git as an LF
  # blob; regenerated patches then encode line counts that match .patchsrc
  # but not the pristine tree the autobuild patches, giving stack-wide fuzz.
  printf '%s\n' '* -text' > "$GITREPO/.gitattributes"
  git_q add -A
  git_q commit -q -m "base: pristine (post-setup) source" --allow-empty

  # Sanity check: any file where the working-tree bytes differ from the
  # committed blob means something normalised on commit (typically CRLF<->LF).
  # Sample tracked files and NAME the offenders so the user can act.
  local __cksum_fail=0 __rel __wt __blob
  while IFS= read -r __rel; do
    __wt=$(git_q hash-object -- "$__rel" 2>/dev/null)   || continue
    __blob=$(git_q rev-parse "HEAD:$__rel" 2>/dev/null) || continue
    if [ "$__wt" != "$__blob" ]; then
      __cksum_fail=$((__cksum_fail+1))
      [ "$__cksum_fail" -le 3 ] && log "  DIFFERS on commit: $__rel  (wt=$__wt blob=$__blob)"
    fi
  done < <(git_q ls-files | head -50)
  if [ "$__cksum_fail" != 0 ]; then
    log "WARNING: $__cksum_fail of the first 50 tracked file(s) had bytes normalised on commit."
    log "         Regenerated patches may drift vs. your autobuild's source tree."
    log "         Check: git config --show-origin --get-all core.autocrlf core.eol"
  fi

  # Persist metadata under .git (untracked). Save the original of any patch that
  # has a frozen file, so generate can lift that file's hunk verbatim.
  local GPMETA="$GITREPO/.git/gitpatch"
  mkdir -p "$GPMETA/orig"
  cp "$FROZEN" "$GPMETA/frozen"
  if [ "$nfrozen" != 0 ]; then
    while IFS=$'\t' read -r target tag name abs; do
      if [ -n "$(patch_filter "$abs" keep "$FROZEN")" ]; then
        mkdir -p "$GPMETA/orig/$(dirname "$target")"
        cp "$abs" "$GPMETA/orig/$target"
      fi
    done < <(order)
  fi

  # 4. Replay every patch FORWARD, one commit each, carrying metadata trailers.
  log "replaying patches forward (one commit each)..."
  local count=0
  while IFS=$'\t' read -r target tag name abs; do
    if ! apply_forward "$abs" "$GITREPO" "$FROZEN"; then
      die "forward-apply FAILED for $abs (target $target)"
    fi
    git_q add -A
    # If forward-apply produced no diff, the tree is stuck in the built state:
    # nearly always because the reverse-apply of THIS patch silently no-op'd
    # (e.g. patch tool returned 0 without touching the file). Fail with a
    # message that says which patch and where to look, not just "nothing to
    # commit, working tree clean".
    if git_q diff --cached --quiet; then
      die "forward-apply of $target produced NO diff.
  This usually means the earlier reverse-apply of this patch silently no-op'd:
  the base tree still has this patch's changes on top, so forward-apply says
  'already applied' and stages nothing.
  Check the previous state:   git -C $GITREPO log --oneline
  Inspect one target file:    git -C $GITREPO show HEAD:$(files_in_patch \"$abs\" | head -1)
  Compare with the built src: diff -u $SRC/$(files_in_patch \"$abs\" | head -1) $GITREPO/$(files_in_patch \"$abs\" | head -1) | head -30"
    fi
    local filelist; filelist=$(files_in_patch "$abs" | paste -sd';' -)
    local pstyle; pstyle=$(patch_style "$abs")
    local peol=lf; if grep -q $'\r' "$abs"; then peol=crlf; fi
    # Subject = "<tag>/<name>" so it is unique and self-describing in git log
    # (a bare basename like "ccpu" exists in both common and minnt). Target: is
    # still the authoritative key generate uses.
    git_q commit -q -F - <<EOF
$tag/$name

Target: $target
Repo: $tag
Patchset: $name
Style: $pstyle
Eol: $peol
Files: $filelist
EOF
    count=$((count+1))
    # Progress every 10 commits so a stall is visible.
    [ $((count % 10)) = 0 ] && log "  forward-apply: $count patches committed (last: $tag/$name)"
  done < <(order)
  log "done: base + $count patch commits in $GITREPO"
  log "next: review history with 'git -C $GITREPO log --oneline', then '$0 generate' to round-trip."
}

# --- subcommand: generate ---------------------------------------------------
cmd_generate() {
  local DEST="$PATCHROOT" CHECK=0
  while [ $# -gt 0 ]; do
    case "$1" in
      --dest) DEST="$2"; shift 2;;
      --check) CHECK=1; shift;;
      *) die "generate: unknown arg $1";;
    esac
  done
  [ -d "$GITREPO/.git" ] || die "generate: no git repo at $GITREPO (run bootstrap first)"
  local GPMETA="$GITREPO/.git/gitpatch"

  # Walk commits oldest->newest (skip the base commit = root).
  local commits; commits=$(git_q rev-list --reverse HEAD)
  local base; base=$(git_q rev-list --max-parents=0 HEAD)

  # Group consecutive commits sharing the same Target into one output patch.
  local prev_target="" group_first="" group_last="" written=0
  flush_group() {
    [ -n "$prev_target" ] || return 0
    local target="$prev_target"
    local parent; parent=$(git_q rev-parse "$group_first^")
    local out="$DEST/$target"
    mkdir -p "$(dirname "$out")"
    # Pick the line-ending convention: match the existing patch if present,
    # else the GP_EOL default (lf). GNU diff emits LF on its structural lines;
    # we normalise the whole file to one convention so CRLF originals stay
    # byte-identical.
    # Style/EOL come from the commit trailers (recorded at bootstrap), so
    # regeneration is fully reproducible from git even with the old .patch files
    # deleted. Fall back to the existing file, then to env defaults.
    local eol style
    eol=$(trailer "$group_first" Eol)
    style=$(trailer "$group_first" Style)
    if [ -z "$eol$style" ] && [ -f "$out" ]; then
      if head -c 1000000 "$out" | grep -q $'\r'; then eol=crlf; else eol=lf; fi
      style=$(patch_style "$out")
    fi
    local body; body=$(mktemp)
    # File order: use the Files: trailer of the first commit if present, else
    # fall back to git's name order across the whole group.
    local files
    files=$(trailer "$group_first" Files | tr ';' '\n' | sed '/^$/d')
    if [ -z "$files" ]; then
      files=$(git_q diff --name-only "$parent" "$group_last")
    fi
    # A file may appear as several segments in the original patch (e.g.
    # wow32.patch lists wudlg.h twice). git captures all of a file's changes in
    # one diff, so emit each file once -- keep first-seen order, drop repeats.
    files=$(printf '%s\n' "$files" | awk 'NF && !seen[$0]++')
    # Brand-new patch (no trailer, no existing file):
    #   1. Explicit GP_EOL wins (user override).
    #   2. Else sniff the first tracked file's blob in git — a Windows/CRLF
    #      checkout produces a CRLF patch, a Unix/LF checkout produces LF.
    #   3. Else fall back to crlf (matches the NT source convention).
    [ -z "$eol" ] && [ -n "${GP_EOL:-}" ] && eol="$GP_EOL"
    if [ -z "$eol" ]; then
      local __sniff
      __sniff=$(printf '%s\n' "$files" | awk 'NF{print;exit}')
      if [ -n "$__sniff" ] && git_q cat-file -e "$group_last:$__sniff" 2>/dev/null; then
        if git_q show "$group_last:$__sniff" | head -c 100000 | grep -q $'\r'; then
          eol=crlf
        else
          eol=lf
        fi
      fi
    fi
    [ -n "$eol" ]   || eol="crlf"
    [ -n "$style" ] || style="${GP_STYLE:-minus}"
    local rel
    while IFS= read -r rel; do
      [ -n "$rel" ] || continue
      if grep -qxF "$rel" "$GPMETA/frozen" 2>/dev/null; then
        # Frozen file: not in git (patched then deleted). Lift its hunk verbatim
        # from the saved original patch so the regenerated patch is unchanged.
        local orig="$GPMETA/orig/$target"
        [ -f "$orig" ] && patch_filter "$orig" keep <(printf '%s\n' "$rel") >> "$body"
        continue
      fi
      local old new
      old=$(mktemp); new=$(mktemp)
      git_q cat-file -e "$parent:$rel" 2>/dev/null     && git_q show "$parent:$rel"     > "$old" || : > "$old"
      git_q cat-file -e "$group_last:$rel" 2>/dev/null && git_q show "$group_last:$rel" > "$new" || : > "$new"
      # Force both sides to the target patch's EOL before diffing. Without this,
      # committing an LF edit against a CRLF base (typical when a file is copied
      # into the repo from a Windows source but edited on Unix) makes diff see
      # every line as changed and blows up the patch into a whole-file rewrite.
      # native = user asked to keep bytes as-is; skip.
      case "$eol" in
        crlf) sed -i -e 's/\r$//' -e 's/$/\r/' "$old" "$new" ;;
        lf)   sed -i -e 's/\r$//'              "$old" "$new" ;;
      esac
      if ! cmp -s "$old" "$new"; then
        emit_header "$rel" "$style" >> "$body"
        "$DIFFBIN" --normal "$old" "$new" >> "$body" || true
      fi
      rm -f "$old" "$new"
    done <<< "$files"
    case "$eol" in
      crlf) sed -e 's/\r$//' -e 's/$/\r/' "$body" > "$out" ;;
      lf)   sed -e 's/\r$//' "$body" > "$out" ;;
      *)    cp "$body" "$out" ;;   # native: no EOL normalisation
    esac
    rm -f "$body"
    written=$((written+1))
  }

  local c target
  for c in $commits; do
    [ "$c" = "$base" ] && continue
    target=$(trailer "$c" Target)
    [ -n "$target" ] || die "commit $c has no Target: trailer (not produced by bootstrap?)"
    if [ "$target" = "$prev_target" ]; then
      group_last="$c"
    else
      flush_group
      prev_target="$target"; group_first="$c"; group_last="$c"
    fi
  done
  flush_group
  log "generated $written patch file(s) under $DEST"

  [ "$CHECK" = 1 ] && cmd_verify --dest "$DEST"
  return 0
}

# --- subcommand: verify -----------------------------------------------------
# Apply the (regenerated) patches in order onto the base tree and confirm the
# result matches git HEAD exactly. This is the correctness guarantee.
cmd_verify() {
  local DEST_DIR="$PATCHROOT"
  while [ $# -gt 0 ]; do
    case "$1" in --dest) DEST_DIR="$2"; shift 2;; *) die "verify: unknown arg $1";; esac
  done
  [ -d "$GITREPO/.git" ] || die "verify: no git repo at $GITREPO"
  local GPMETA="$GITREPO/.git/gitpatch"
  local frozen=""; [ -f "$GPMETA/frozen" ] && frozen="$GPMETA/frozen"
  local tmp; tmp=$(mktemp -d)
  local base; base=$(git_q rev-list --max-parents=0 HEAD)
  git_q archive "$base" | tar -x -C "$tmp"

  # Apply the regenerated patches in the order the commits sit in git history,
  # each Target once. Driving this from the commits (not from order()) keeps
  # verify consistent with whatever scope was bootstrapped.
  local fail=0 c target
  local targets
  targets=$(for c in $(git_q rev-list --reverse HEAD); do
              [ "$c" = "$base" ] && continue
              trailer "$c" Target
            done | awk '!seen[$0]++')
  while IFS= read -r target; do
    [ -n "$target" ] || continue
    local pf="$DEST_DIR/$target"
    [ -f "$pf" ] || { log "verify: missing generated patch $target"; fail=1; continue; }
    if ! apply_forward "$pf" "$tmp" "$frozen"; then
      log "verify: FAILED to apply $target"; fail=1
    fi
  done <<< "$targets"

  find "$tmp" -type f -empty -delete
  local head; head=$(mktemp -d)
  git_q archive HEAD | tar -x -C "$head"

  if diff -r -q "$head" "$tmp" >/dev/null 2>&1; then
    log "verify: OK - regenerated patches reproduce git HEAD exactly"
  else
    log "verify: MISMATCH between regenerated result and git HEAD:"
    diff -r "$head" "$tmp" | sed 's/^/  /' >&2 || true
    fail=1
  fi
  rm -rf "$tmp" "$head"
  [ "$fail" = 0 ] || die "verify: FAILED"
}

# --- subcommand: newfiles ---------------------------------------------------
# List files committed in HEAD that do NOT exist in a pristine source tree.
# These are the "shipped as-is" new files that must be released as plain files
# (they are not, and cannot be, represented as normal-diff patches).
cmd_newfiles() {
  local SRC=""
  while [ $# -gt 0 ]; do
    case "$1" in --src) SRC="$2"; shift 2;; *) die "newfiles: unknown arg $1";; esac
  done
  [ -n "$SRC" ] || die "newfiles: --src DIR (pristine, UNpatched source tree) required"
  [ -d "$GITREPO/.git" ] || die "newfiles: no git repo at $GITREPO"
  git_q ls-tree -r --name-only HEAD | while IFS= read -r rel; do
    # Skip gitpatch-internal files that live in the repo but aren't source.
    [ "$rel" = ".gitattributes" ] && continue
    [ -f "$SRC/$rel" ] || echo "$rel"
  done
}

# --- subcommand: add-source -------------------------------------------------
# Bring one or more not-yet-patched original files into the repo and fold them
# into the BASE commit, so a following commit that edits them generates as a
# normal modification patch. Use this before writing a patch for a file no
# existing patch touches (so it isn't yet in .patchsrc).
#   gitpatch.sh add-source --src DIR mvdm/.../foo.c [mvdm/.../bar.c ...]
cmd_add_source() {
  local SRC="" ; local files=""
  while [ $# -gt 0 ]; do
    case "$1" in
      --src) SRC="$2"; shift 2;;
      --*) die "add-source: unknown option $1";;
      *) files="$files$1"$'\n'; shift;;
    esac
  done
  [ -d "$GITREPO/.git" ] || die "add-source: no git repo at $GITREPO (run bootstrap first)"
  [ -n "$SRC" ] || die "add-source: --src DIR required (the source tree, same as bootstrap)"
  [ -d "$SRC" ] || die "add-source: source dir not found: $SRC"
  [ -n "$files" ] || die "add-source: at least one file path required (e.g. mvdm/softpc.new/host/src/foo.c)"
  # We rebase onto the base commit, which needs a clean working tree.
  if [ -n "$(git_q status --porcelain)" ]; then
    die "add-source: working tree at $GITREPO is not clean.
  Commit or stash your changes first (add-source folds into the base commit,
  which requires a clean tree)."
  fi
  local base; base=$(git_q rev-list --max-parents=0 HEAD)
  local f rel staged=0
  while IFS= read -r f; do
    [ -n "$f" ] || continue
    # forgive Windows-style / prefixed paths -> -p2 relative path
    rel=$(printf '%s\n' "$f" | tr '\\' '/' | sed -e 's#^\./##' -e 's#^NT4/private/##' -e 's#^nt/private/##')
    if git_q cat-file -e "HEAD:$rel" 2>/dev/null; then
      log "add-source: already in repo, skipping: $rel"; continue
    fi
    [ -f "$SRC/$rel" ] || die "add-source: not found in source tree: $SRC/$rel"
    mkdir -p "$GITREPO/$(dirname "$rel")"
    cp -p "$SRC/$rel" "$GITREPO/$rel"
    git_q add -- "$rel"
    staged=1
    log "add-source: staged pristine $rel"
  done <<< "$files"
  [ "$staged" = 1 ] || { log "add-source: nothing new to add"; return 0; }
  git_q commit -q --fixup="$base"
  if ! GIT_SEQUENCE_EDITOR=true GIT_EDITOR=true \
       git -C "$GITREPO" rebase -i --autosquash --root >/dev/null 2>&1; then
    git -C "$GITREPO" rebase --abort >/dev/null 2>&1 || true
    die "add-source: failed to fold into the base commit (rebase aborted; tree unchanged)."
  fi
  log "add-source: folded into base. Now edit the file(s) and commit your change"
  log "  with Target:/Repo:/Patchset: trailers, then run 'generate'."
}

# --- dispatch ---------------------------------------------------------------
[ $# -ge 1 ] || die "usage: $0 {order|files|bootstrap|generate|verify|newfiles|add-source} [opts]"
cmd="$1"; shift
case "$cmd" in
  order)      order ;;
  files)      all_files ;;
  bootstrap)  cmd_bootstrap "$@" ;;
  generate)   cmd_generate "$@" ;;
  verify)     cmd_verify "$@" ;;
  newfiles)   cmd_newfiles "$@" ;;
  add-source) cmd_add_source "$@" ;;
  *) die "unknown subcommand: $cmd" ;;
esac
