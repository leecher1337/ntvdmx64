#!/usr/bin/env bash
#
# selftest.sh - end-to-end proof of the gitpatch engine on a synthetic source
# tree (so it runs anywhere, without the real non-shippable NT source).
#
# It builds a pristine tree + two CHAINED normal-diff patches (the 2nd's line
# numbers depend on the 1st, plus it creates a brand-new file), then exercises:
#   * clean-mode bootstrap   (base = pristine)
#   * patched-mode bootstrap (base recovered by reverse-applying patches)
#   * generate  (regenerate .patch files from git)
#   * verify    (regenerated patches reproduce HEAD)
#   * byte-identity of regenerated patches vs the originals
#
# set -eu (no pipefail): `git … | grep -q X` is intentional here, and pipefail
# would misreport such pipes as failed when grep -q exits early and SIGPIPEs the
# upstream command (a timing-dependent false negative).
set -eu
export LC_ALL=C
HERE="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
GP="$HERE/gitpatch.sh"

T=$(mktemp -d)
trap 'rm -rf "$T"' EXIT
PR="$T/patchroot"                 # fake PATCHROOT (ntvdmpatch)
PRISTINE="$T/pristine/private"    # fake clean source tree (= BASEPATH)
PATCHED="$T/patched/private"      # fake fully-patched source tree
mkdir -p "$PR/patches/common" "$PR/patches/minnt" "$PRISTINE/mvdm/test"

ok()  { echo "  PASS: $*"; }
bad() { echo "  FAIL: $*"; exit 1; }

# --- 1. pristine source -----------------------------------------------------
cat > "$PRISTINE/mvdm/test/foo.c" <<'EOF'
#include <stdio.h>

int main(void)
{
    int a = 1;
    int b = 2;
    return a + b;
}
EOF

# --- 2. craft two chained patches in the real NT format ---------------------
mkhdr() { # $1 relpath -> two header lines with NT4/nt backslash prefixes
  local bs; bs=$(printf '%s' "$1" | tr '/' '\\')
  printf -- '--- NT4\\private\\%s\n+++ nt\\private\\%s\n' "$bs" "$bs"
}

# Patch 1: edit foo.c (change b, add a line).
work=$(mktemp -d)
cp "$PRISTINE/mvdm/test/foo.c" "$work/foo.c"
cat > "$work/foo.c" <<'EOF'
#include <stdio.h>

int main(void)
{
    int a = 1;
    int b = 20;
    int c = 3;
    return a + b + c;
}
EOF
{ mkhdr mvdm/test/foo.c; diff --normal "$PRISTINE/mvdm/test/foo.c" "$work/foo.c" || true; } \
  > "$PR/patches/common/a_edit.patch"

# Patch 2 (chained): further edit the ALREADY-patched foo.c + create new bar.c.
cp "$work/foo.c" "$work/foo2.c"
cat > "$work/foo2.c" <<'EOF'
#include <stdio.h>

int main(void)
{
    int a = 10;
    int b = 20;
    int c = 3;
    return a + b + c;
}
EOF
cat > "$work/bar.c" <<'EOF'
/* brand new file added by patch */
void bar(void) {}
EOF
{
  mkhdr mvdm/test/foo.c; diff --normal "$work/foo.c" "$work/foo2.c" || true
  mkhdr mvdm/test/bar.c; diff --normal /dev/null "$work/bar.c" || true
} > "$PR/patches/common/b_more.patch"

echo "== synthetic patches =="
echo "--- a_edit.patch ---"; cat "$PR/patches/common/a_edit.patch"
echo "--- b_more.patch ---"; cat "$PR/patches/common/b_more.patch"

# --- 3. build the fully-patched tree (for patched-mode test) ----------------
cp -r "$T/pristine" "$T/patched"
apply_to() { # apply $2 into dir $1, pre-creating any missing target file
  local dir="$1" pf="$2" rel
  grep '^--- ' "$pf" | sed -e 's/^--- //' -e 's/[[:space:]]*$//' | tr '\\' '/' \
    | sed 's#^NT4/private/##' | while IFS= read -r rel; do
      [ -e "$dir/$rel" ] || { mkdir -p "$dir/$(dirname "$rel")"; : > "$dir/$rel"; }
    done
  ( cd "$dir" && patch -s -N -p2 -i <(awk '/^--- |^\+\+\+ /{gsub(/\\/,"/")}1' "$pf") )
}
apply_to "$PATCHED" "$PR/patches/common/a_edit.patch"
apply_to "$PATCHED" "$PR/patches/common/b_more.patch"

run_gp() { GP_PATCHROOT="$PR" GP_GITREPO="$T/gitrepo" bash "$GP" "$@"; }

# Keep golden copies of the originals for byte comparison.
golden=$(mktemp -d); cp -r "$PR/patches" "$golden/"

assert_identical() {
  for f in a_edit.patch b_more.patch; do
    if cmp -s "$golden/patches/common/$f" "$PR/patches/common/$f"; then
      ok "byte-identical: $f"
    else
      echo "    diff for $f:"; diff "$golden/patches/common/$f" "$PR/patches/common/$f" | sed 's/^/      /'
      bad "regenerated $f differs from original"
    fi
  done
}

# ===========================================================================
echo; echo "########## TEST A: clean-mode bootstrap ##########"
rm -rf "$T/gitrepo"
run_gp bootstrap --src "$PRISTINE" --mode clean --force
echo "git history:"; git -C "$T/gitrepo" log --oneline | sed 's/^/  /'
# wipe the .patch files, then regenerate them from git only
rm -f "$PR"/patches/common/*.patch
run_gp generate --check
assert_identical
cp -r "$golden/patches" "$PR/"   # restore originals for next test

echo; echo "########## TEST B: patched-mode bootstrap (reverse-apply) ##########"
rm -rf "$T/gitrepo"
run_gp bootstrap --src "$PATCHED" --mode patched --force
echo "git history:"; git -C "$T/gitrepo" log --oneline | sed 's/^/  /'
# confirm base really lacks the patch-created file
if git -C "$T/gitrepo" cat-file -e "$(git -C "$T/gitrepo" rev-list --max-parents=0 HEAD):mvdm/test/bar.c" 2>/dev/null; then
  bad "base wrongly contains patch-created bar.c"
else
  ok "base correctly lacks patch-created bar.c"
fi
rm -f "$PR"/patches/common/*.patch
run_gp generate --check
assert_identical

echo; echo "########## TEST C: newfiles detection ##########"
nf=$(run_gp newfiles --src "$PRISTINE")
if [ "$nf" = "mvdm/test/bar.c" ]; then ok "newfiles reports: $nf"; else bad "newfiles wrong: '$nf'"; fi

echo; echo "########## TEST D: Index/SVN-style header round-trips ##########"
# Fresh isolated setup with an Index: style patch (CRLF, like cvidcheap.patch).
PRI2="$T/pri2/private"; mkdir -p "$PRI2/mvdm/x"
printf 'one\r\ntwo\r\nthree\r\n' > "$PRI2/mvdm/x/qux.c"   # CRLF source, like real NT source
SEP='==================================================================='
{                                                          # CRLF Index-style patch with a 'c' (change) hunk
  printf 'Index: nt/private/mvdm/x/qux.c\r\n'
  printf '%s\r\n' "$SEP"
  printf '2c2\r\n'
  printf '< two\r\n'
  printf '%s\r\n' '---'
  printf '> TWO\r\n'
} > "$PR/patches/common/c_index.patch"
rm -f "$PR"/patches/common/a_edit.patch "$PR"/patches/common/b_more.patch
golden2=$(cat "$PR/patches/common/c_index.patch")
rm -rf "$T/gitrepo"
run_gp bootstrap --src "$PRI2" --mode clean --force >/dev/null
rm -f "$PR"/patches/common/c_index.patch
run_gp generate --check >/dev/null
if grep -q '^Index: ' "$PR/patches/common/c_index.patch"; then ok "Index-style header preserved"; else bad "lost Index-style header"; fi
if [ "$(cat "$PR/patches/common/c_index.patch")" = "$golden2" ]; then ok "Index-style byte-identical (incl. CRLF)"; else
  diff <(printf '%s' "$golden2") "$PR/patches/common/c_index.patch" | cat -A | sed 's/^/    /'; bad "Index-style differs"
fi

echo; echo "########## TEST E: scope (core vs all) ##########"
# Restore exactly the two original common patches (drop TEST D leftovers) and
# add an experimental vesa patch + target.
rm -f "$PR"/patches/common/*.patch
cp "$golden"/patches/common/*.patch "$PR/patches/common/"
mkdir -p "$PR/experimental/vesa" "$PRISTINE/mvdm/v"
printf 'p\nq\nr\n' > "$PRISTINE/mvdm/v/vid.c"
printf -- '--- NT4\\private\\mvdm\\v\\vid.c\n+++ nt\\private\\mvdm\\v\\vid.c\n2c2\n< q\n---\n> Q\n' > "$PR/experimental/vesa/vesa.patch"

# Default (core) must EXCLUDE experimental:
ord_core=$(GP_PATCHROOT="$PR" GP_SCOPE=core bash "$GP" order | grep -c vesa || true)
[ "$ord_core" = 0 ] && ok "order --scope core excludes experimental" || bad "core scope leaked experimental"
ord_all=$(GP_PATCHROOT="$PR" GP_SCOPE=all bash "$GP" order | grep -c vesa || true)
[ "$ord_all" = 1 ] && ok "order --scope all includes experimental" || bad "all scope missing experimental"

# bootstrap --scope core: vesa.patch must NOT be committed.
rm -rf "$T/gitrepo"
run_gp bootstrap --src "$PRISTINE" --mode clean --scope core --force >/dev/null
if git -C "$T/gitrepo" log --format=%B | grep -q 'Target: experimental/vesa'; then bad "core scope committed experimental"; else ok "--scope core omits experimental"; fi

# bootstrap default (scope=all): vesa.patch is committed and round-trips.
rm -rf "$T/gitrepo"
run_gp bootstrap --src "$PRISTINE" --mode clean --force >/dev/null
if git -C "$T/gitrepo" log --format=%B | grep -q 'Target: experimental/vesa'; then ok "default scope=all includes experimental"; else bad "default did not include experimental"; fi
rm -f "$PR"/patches/common/*.patch "$PR"/experimental/vesa/*.patch
run_gp generate --check >/dev/null
[ -f "$PR/experimental/vesa/vesa.patch" ] && ok "default generates experimental patch" || bad "default did not generate vesa.patch"

echo; echo "########## TEST F: frozen file (patched then deleted) ##########"
# A patch modifies foo.c AND qux.c; the build then DELETES qux.c. qux.c must be
# carried verbatim (not reverse-applied, not regenerated from git).
PRf="$T/prf"; mkdir -p "$PRf/patches/common" "$PRf/patches/minnt"
PRI3="$T/pri3/private"; mkdir -p "$PRI3/mvdm/f"
printf 'A\nB\nC\n' > "$PRI3/mvdm/f/foo.c"
printf 'X\nY\nZ\n' > "$PRI3/mvdm/f/qux.c"
w=$(mktemp -d)
printf 'A\nBB\nC\n' > "$w/foo2"; printf 'X\nYY\nZ\n' > "$w/qux2"
{
  printf -- '--- NT4\\private\\mvdm\\f\\foo.c\n+++ nt\\private\\mvdm\\f\\foo.c\n'; diff --normal "$PRI3/mvdm/f/foo.c" "$w/foo2" || true
  printf -- '--- NT4\\private\\mvdm\\f\\qux.c\n+++ nt\\private\\mvdm\\f\\qux.c\n'; diff --normal "$PRI3/mvdm/f/qux.c" "$w/qux2" || true
} > "$PRf/patches/common/p1.patch"
golden_p1=$(cat "$PRf/patches/common/p1.patch")
# Build the patched tree, then delete qux.c (simulating patch.cmd's cleanup).
cp -r "$T/pri3" "$T/pat3"
apply_to "$T/pat3/private" "$PRf/patches/common/p1.patch"
rm "$T/pat3/private/mvdm/f/qux.c"
run_gpf() { GP_PATCHROOT="$PRf" GP_GITREPO="$T/repo3" bash "$GP" "$@"; }
boot_out=$(run_gpf bootstrap --src "$T/pat3/private" --mode patched --force 2>&1)
echo "$boot_out" | grep -q 'frozen.*qux.c' && ok "qux.c detected as frozen" || { echo "$boot_out"|sed 's/^/    /'; bad "qux.c not flagged frozen"; }
# base must contain foo.c (reverted) but not qux.c
b3=$(git -C "$T/repo3" rev-list --max-parents=0 HEAD)
git -C "$T/repo3" cat-file -e "$b3:mvdm/f/qux.c" 2>/dev/null && bad "frozen qux.c leaked into git" || ok "frozen qux.c kept out of git"
rm -f "$PRf"/patches/common/p1.patch
run_gpf generate --check >/dev/null
if [ "$(cat "$PRf/patches/common/p1.patch")" = "$golden_p1" ]; then ok "frozen patch regenerated byte-identical (foo.c from git + qux.c verbatim)"; else
  diff <(printf '%s' "$golden_p1") "$PRf/patches/common/p1.patch" | sed 's/^/    /'; bad "frozen patch differs"
fi

echo; echo "########## TEST G: clobbered file auto-frozen ##########"
# A patch modifies foo.c and gizmo.c; the build then OVERWRITES gizmo.c with
# unrelated content (like haxm's minnt\v86 overlay). gizmo.c is present but won't
# reverse -> must be auto-frozen and carried verbatim.
PRg="$T/prg"; mkdir -p "$PRg/patches/common" "$PRg/patches/minnt"
PRI4="$T/pri4/private"; mkdir -p "$PRI4/mvdm/g"
printf 'A\nB\nC\n' > "$PRI4/mvdm/g/foo.c"
printf 'X\nY\nZ\n' > "$PRI4/mvdm/g/gizmo.c"
w=$(mktemp -d); printf 'A\nBB\nC\n' > "$w/f2"; printf 'X\nYY\nZ\n' > "$w/g2"
{
  printf -- '--- NT4\\private\\mvdm\\g\\foo.c\n+++ nt\\private\\mvdm\\g\\foo.c\n';   diff --normal "$PRI4/mvdm/g/foo.c" "$w/f2" || true
  printf -- '--- NT4\\private\\mvdm\\g\\gizmo.c\n+++ nt\\private\\mvdm\\g\\gizmo.c\n'; diff --normal "$PRI4/mvdm/g/gizmo.c" "$w/g2" || true
} > "$PRg/patches/common/q1.patch"
golden_q1=$(cat "$PRg/patches/common/q1.patch")
cp -r "$T/pri4" "$T/pat4"
apply_to "$T/pat4/private" "$PRg/patches/common/q1.patch"
printf 'OVERLAY\nCONTENT\n' > "$T/pat4/private/mvdm/g/gizmo.c"     # clobber, post-patch
run_gpg() { GP_PATCHROOT="$PRg" GP_GITREPO="$T/repo4" bash "$GP" "$@"; }
g_out=$(run_gpg bootstrap --src "$T/pat4/private" --mode patched --force 2>&1)
echo "$g_out" | grep -q 'froze.*gizmo.c' && ok "clobbered gizmo.c auto-frozen" || { echo "$g_out"|sed 's/^/    /'; bad "gizmo.c not auto-frozen"; }
rm -f "$PRg"/patches/common/q1.patch
run_gpg generate --check >/dev/null
[ "$(cat "$PRg/patches/common/q1.patch")" = "$golden_q1" ] && ok "clobber patch byte-identical (foo from git + gizmo verbatim)" || bad "clobber patch differs"

echo; echo "########## TEST H: full-stack reverse lets --scope core work under an experimental patch ##########"
# A core patch and an experimental patch both touch shared.c (experimental on
# top). --scope core must still reverse the WHOLE stack to reach base, then
# replay only the core patch.
PRh="$T/prh"; mkdir -p "$PRh/patches/common" "$PRh/patches/minnt" "$PRh/experimental/vesa"
PRI5="$T/pri5/private"; mkdir -p "$PRI5/mvdm/s"
printf 'L1\nL2\nL3\nL4\nL5\n' > "$PRI5/mvdm/s/shared.c"
w=$(mktemp -d)
printf 'L1\nL2X\nL3\nL4\nL5\n' > "$w/afterpc"
printf 'L1\nL2X\nL3\nL4Y\nL5\n' > "$w/afterpe"
{ printf -- '--- NT4\\private\\mvdm\\s\\shared.c\n+++ nt\\private\\mvdm\\s\\shared.c\n'; diff --normal "$PRI5/mvdm/s/shared.c" "$w/afterpc" || true; } > "$PRh/patches/common/pc.patch"
{ printf -- '--- NT4\\private\\mvdm\\s\\shared.c\n+++ nt\\private\\mvdm\\s\\shared.c\n'; diff --normal "$w/afterpc" "$w/afterpe" || true; } > "$PRh/experimental/vesa/pe.patch"
golden_pc=$(cat "$PRh/patches/common/pc.patch")
cp -r "$T/pri5" "$T/pat5"
apply_to "$T/pat5/private" "$PRh/patches/common/pc.patch"
apply_to "$T/pat5/private" "$PRh/experimental/vesa/pe.patch"
run_gph() { GP_PATCHROOT="$PRh" GP_GITREPO="$T/repo5" bash "$GP" "$@"; }
if run_gph bootstrap --src "$T/pat5/private" --mode patched --scope core --force >/dev/null 2>&1; then
  ok "core bootstrap succeeds despite experimental patch on top"
else bad "core bootstrap failed under experimental patch"; fi
# Only the core patch is committed; experimental pe is peeled off, not replayed.
git -C "$T/repo5" log --format=%B | grep -q 'Target: experimental' && bad "core scope committed experimental" || ok "experimental peeled but not committed"
rm -f "$PRh"/patches/common/pc.patch
run_gph generate --check >/dev/null
[ "$(cat "$PRh/patches/common/pc.patch")" = "$golden_pc" ] && ok "core patch byte-identical after full-stack reverse" || bad "core patch differs"

echo; echo "########## TEST I: same file in two segments of one patch ##########"
# wow32.patch lists wudlg.h as two separate --- segments. The regenerated patch
# must consolidate them into one (git captures all of a file's changes at once)
# and still round-trip.
PRi="$T/pri_i"; mkdir -p "$PRi/patches/common" "$PRi/patches/minnt"
PRI6="$T/pri6/private"; mkdir -p "$PRI6/mvdm/m"
printf 'L1\nL2\nL3\nL4\nL5\nL6\nL7\nL8\nL9\nL10\n' > "$PRI6/mvdm/m/two.c"
{
  printf -- '--- NT4\\private\\mvdm\\m\\two.c\n+++ nt\\private\\mvdm\\m\\two.c\n2c2\n< L2\n---\n> L2X\n'
  printf -- '--- NT4\\private\\mvdm\\m\\two.c\n+++ nt\\private\\mvdm\\m\\two.c\n8c8\n< L8\n---\n> L8X\n'
} > "$PRi/patches/common/two.patch"
run_gpi() { GP_PATCHROOT="$PRi" GP_GITREPO="$T/repo6" bash "$GP" "$@"; }
run_gpi bootstrap --src "$PRI6" --mode clean --force >/dev/null
rm -f "$PRi"/patches/common/two.patch
run_gpi generate --check >/dev/null && ok "two-segment patch round-trips (verify OK)" || bad "two-segment patch failed verify"
hdrs=$(grep -c '^--- ' "$PRi/patches/common/two.patch")
[ "$hdrs" = 1 ] && ok "two segments consolidated into one ($hdrs header)" || bad "expected 1 header, got $hdrs"
# and the consolidated patch carries both changes
grep -q 'L2X' "$PRi/patches/common/two.patch" && grep -q 'L8X' "$PRi/patches/common/two.patch" && ok "both edits present" || bad "lost an edit"

echo; echo "########## TEST J: add-source folds a not-yet-patched file into base ##########"
PRj="$T/prj"; mkdir -p "$PRj/patches/common" "$PRj/patches/minnt"
SRCj="$T/srcj/private"; mkdir -p "$SRCj/mvdm/n"
printf 'p\nq\nr\n' > "$SRCj/mvdm/n/old.c"                 # already-patched file
printf 'K1\nK2\nK3\n' > "$SRCj/mvdm/n/fresh.c"            # NOT yet patched
printf -- '--- NT4\\private\\mvdm\\n\\old.c\n+++ nt\\private\\mvdm\\n\\old.c\n1c1\n< p\n---\n> P\n' > "$PRj/patches/common/old.patch"
run_gpj() { GP_PATCHROOT="$PRj" GP_GITREPO="$T/repoj" bash "$GP" "$@"; }
run_gpj bootstrap --src "$SRCj" --mode clean --force >/dev/null
git -C "$T/repoj" cat-file -e "HEAD:mvdm/n/fresh.c" 2>/dev/null && bad "fresh.c unexpectedly present" || ok "fresh.c absent before add-source"
run_gpj add-source --src "$SRCj" "mvdm/n/fresh.c" >/dev/null 2>&1
basej=$(git -C "$T/repoj" rev-list --max-parents=0 HEAD)
if git -C "$T/repoj" cat-file -e "$basej:mvdm/n/fresh.c" 2>/dev/null; then ok "fresh.c folded into base commit"; else bad "fresh.c not in base"; fi
git -C "$T/repoj" log --format=%B | grep -q '^fixup!' && bad "leftover fixup! commit" || ok "no leftover fixup commit"
( cd "$T/repoj"
  printf 'K1\nK2X\nK3\n' > mvdm/n/fresh.c
  git add mvdm/n/fresh.c
  git commit -q -F - <<EOF
common/fresh

Target: patches/common/fresh.patch
Repo: common
Patchset: fresh
EOF
)
run_gpj generate --check >/dev/null && ok "new patch on added file round-trips" || bad "generate/verify failed"
grep -q 'K2X' "$PRj/patches/common/fresh.patch" 2>/dev/null && ok "fresh.patch contains the edit" || bad "fresh.patch missing edit"
( cd "$T/repoj"; printf 'dirty\n' >> mvdm/n/fresh.c )
run_gpj add-source --src "$SRCj" "mvdm/n/old.c" >/dev/null 2>&1 && bad "add-source ran on dirty tree" || ok "add-source refuses dirty tree"
( cd "$T/repoj"; git checkout -q -- mvdm/n/fresh.c )

echo; echo "########## TEST K: scope=all keeps core + experimental consistent on a shared file ##########"
# A core patch (pc) and an experimental patch (pe) edit the SAME file, pe after
# pc. With --scope all both are managed, so re-flowing pc (inserting a line)
# automatically keeps pe's regenerated patch correct.
PRk="$T/prk"; mkdir -p "$PRk/patches/common" "$PRk/patches/minnt" "$PRk/experimental/vesa"
PRI7="$T/pri7/private"; mkdir -p "$PRI7/mvdm/s"
printf 'L1\nL2\nL3\nL4\nL5\nL6\n' > "$PRI7/mvdm/s/shared.c"
w=$(mktemp -d)
printf 'L1\nL2X\nL3\nL4\nL5\nL6\n'  > "$w/apc"          # after pc (L2 -> L2X)
printf 'L1\nL2X\nL3\nL4\nL5Y\nL6\n' > "$w/ape"          # after pe (L5 -> L5Y)
{ printf -- '--- NT4\\private\\mvdm\\s\\shared.c\n+++ nt\\private\\mvdm\\s\\shared.c\n'; diff --normal "$PRI7/mvdm/s/shared.c" "$w/apc" || true; } > "$PRk/patches/common/pc.patch"
{ printf -- '--- NT4\\private\\mvdm\\s\\shared.c\n+++ nt\\private\\mvdm\\s\\shared.c\n'; diff --normal "$w/apc" "$w/ape" || true; } > "$PRk/experimental/vesa/pe.patch"
cp -r "$T/pri7" "$T/pat7"
apply_to "$T/pat7/private" "$PRk/patches/common/pc.patch"
apply_to "$T/pat7/private" "$PRk/experimental/vesa/pe.patch"
run_gpk() { GP_PATCHROOT="$PRk" GP_GITREPO="$T/repo7" bash "$GP" "$@"; }
# default scope is now 'all' -> both managed
run_gpk bootstrap --src "$T/pat7/private" --mode patched --force >/dev/null
git -C "$T/repo7" log --format=%B | grep -q 'Target: experimental/vesa/pe.patch' && ok "experimental patch managed by default (scope=all)" || bad "experimental not managed by default"
run_gpk generate --check >/dev/null && ok "full set (core+experimental) round-trips" || bad "initial generate failed"
# Now re-flow pc: insert a line near the top, folded into the pc commit.
pcsha=$(git -C "$T/repo7" log --format=%H --grep='^Patchset: pc$' -1)
( cd "$T/repo7"
  sed -i '1a INSERTED' mvdm/s/shared.c           # shifts pe's target line down
  git commit -aq --fixup="$pcsha"
  GIT_SEQUENCE_EDITOR=true GIT_EDITOR=true git rebase -i --autosquash --root >/dev/null 2>&1
)
# pe must still round-trip (its regenerated line numbers re-flowed automatically)
if run_gpk generate --check >/dev/null 2>&1; then ok "after editing pc, pe re-flowed and whole set still round-trips"; else bad "editing core broke experimental (inconsistent set)"; fi
grep -q 'INSERTED' "$PRk/patches/common/pc.patch" && ok "pc.patch carries the new line" || bad "pc.patch missing new line"

echo; echo "########## TEST L: outside-tree file (path with '../') is frozen and round-trips ##########"
# Some patches (e.g. monitor.patch's NT4\private\..\public\sdk\inc\nt_vdd.h)
# reference a file OUTSIDE the -p2 root via '../'. cp+git can't track it, so
# gitpatch should auto-freeze the path and lift its hunk verbatim.
PRl="$T/prl"; mkdir -p "$PRl/patches/minnt"
SRCl="$T/srcl/private"; mkdir -p "$SRCl/mvdm/n" "$SRCl/../public/inc"
printf 'inside\ncontent\n' > "$SRCl/mvdm/n/local.c"
printf 'shared\nheader\n'  > "$SRCl/../public/inc/nt_vdd.h"
{
  printf -- '--- NT4\\private\\mvdm\\n\\local.c\n+++ nt\\private\\mvdm\\n\\local.c\n'
  printf '2c2\n< content\n---\n> CONTENT\n'
  printf -- '--- NT4\\private\\..\\public\\inc\\nt_vdd.h\n+++ nt\\private\\..\\public\\inc\\nt_vdd.h\n'
  printf '2c2\n< header\n---\n> HEADER\n'
} > "$PRl/patches/minnt/outside.patch"
run_gpl() { GP_PATCHROOT="$PRl" GP_GITREPO="$T/repol" bash "$GP" "$@"; }
_out_l=$(run_gpl bootstrap --src "$SRCl" --mode clean --force 2>&1)
printf '%s\n' "$_out_l" | grep -q 'outside-tree file' && ok "outside-tree file detected and frozen" || bad "outside-tree not reported"
git -C "$T/repol" ls-files | grep -q 'nt_vdd.h' && bad "outside-tree file leaked into git" || ok "outside-tree file kept out of git"
run_gpl generate --check >/dev/null && ok "outside.patch round-trips (verify OK)" || bad "outside.patch failed to round-trip"
grep -q 'nt_vdd.h' "$PRl/patches/minnt/outside.patch" && ok "regenerated patch retains nt_vdd.h hunk" || bad "regenerated patch dropped nt_vdd.h hunk"
grep -q 'HEADER' "$PRl/patches/minnt/outside.patch" && ok "verbatim hunk content preserved" || bad "verbatim hunk content lost"

echo; echo "ALL TESTS PASSED"
