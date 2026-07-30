# gitpatch — maintain the NTVDM patch sets through git

The `.patch` files in `patches/` and `experimental/` are line-numbered
**normal (ed-style) diffs**. That format is deliberate: it embeds almost none
of the original (non-shippable) Microsoft source, keeping the patches small and
licence-clean. The price is that every patch encodes **absolute line numbers**
computed against the state produced by *all earlier patches*. Edit one early
patch and every later patch that touches the same file silently rots — which is
exactly the maintenance pain this tool removes.

**Idea:** make git the source of truth. Replay the existing patches into a
linear git history (one commit per patch), edit code there, and **regenerate**
the `.patch` files deterministically. Line numbers stop being something you
maintain — they become a generated artifact of *pristine source + ordered
commits*.

```
   pristine NT source  ──bootstrap──►  git history (1 commit / patch)
                                              │  you edit / rebase / add commits
                                              ▼
                          generate  ──►  patches/*.patch  (regenerated)
                                              │
                                         verify  ──►  apply == git HEAD ?
```

Nothing about the distribution model changes: the git working repo is private
(it contains original source); only the regenerated `.patch` files ship.

---

## Requirements

* **git**
* **GNU patch, diff, sed, awk, coreutils** — a normal POSIX environment.
  * On **Windows**, install *Git for Windows* and run everything from its
    **Git Bash** shell. It bundles all of the above (patch 2.7, diff, sed, awk).
    Works on Windows 7 and up. No separate install needed.
  * On **Linux**, it just works.

The bundled `util/patch.exe` / `util/sed.exe` are still used by the normal
`patch.cmd` build flow; gitpatch uses the Git Bash tools instead, so the two
don't interfere.

---

## One-time bootstrap

You need a **source tree** (`BASEPATH` — e.g. `…\nt\private` or
`…\minnt\base`). Two ways to provide the base state:

### Recommended: from your already-patched tree (`--mode patched`)

You always have a working, fully-patched tree after a build. gitpatch runs the
existing patches **backwards** (`patch -R`, reverse order) to recover the exact
pre-patch base — so you don't have to reproduce any of the setup steps (the
DPMI swap, the cvidc `sed`, the header copies). This is the most robust mode.

```bash
cd ntvdmpatch
bash src/gitpatch/gitpatch.sh bootstrap --src "/m/bld/wk/minnt/base" --mode patched
```

It reverses the **whole** patch stack to find the base, then by default replays
**every** patch — `patches/common`, `patches/<repo>`, and `experimental/*` — as
managed commits. This is the right default: experimental patches edit some of the
same files as the core patches, so they must be regenerated together or editing a
core patch would silently break a later experimental one (see *Scope* for why).
Files that can't be inverted from a built tree are auto-frozen and logged. Use
`--scope core` if you ever want only the `patches/` subset.

### Alternative: from a clean tree (`--mode clean`)

If you have a tree that has had `prepare.cmd`/setup run but **not** the patches,
use `--mode clean` (no reverse step).

Either way you end up with a git repo at `ntvdmpatch/.patchsrc`:

```bash
git -C .patchsrc log --oneline      # base + one commit per patch
```

> Add `.patchsrc/` to your `.gitignore` — it's a private working repo, not part
> of the project repo.

Immediately confirm the round-trip is faithful:

```bash
bash src/gitpatch/gitpatch.sh generate --check
```

`--check` regenerates every `.patch` and then applies the regenerated set to the
base, asserting the result equals git HEAD **exactly**. Use `git status` /
`git diff` in `ntvdmpatch` to see whether any regenerated `.patch` changed
versus what you had (for LF patches it should be byte-identical; see *Line
endings* below).

---

## Daily workflow

1. **Edit the code in the git repo**, grouped however the change belongs:

   ```bash
   cd .patchsrc
   # fix a bug that belongs to an existing patchset (commit subjects are
   # "<repo>/<name>", e.g. minnt/ega_panning):
   $EDITOR mvdm/softpc.new/host/src/nt_ega.c
   # find the target commit by its patchset, make a fixup against it, fold in:
   sha=$(git log --format=%H --grep='^Patchset: ega_panning$' -1)
   git commit -a --fixup="$sha"
   git rebase -i --autosquash --root            # --root = replay from the base commit
   ```

   `--root` rebases the whole patch history (simplest, always works). To replay
   only from the target instead, use `"$sha^"` (the commit before it) in place of
   `--root`. Autosquash matches by the `fixup!` subject git wrote, so use
   `--fixup=<sha>` rather than hand-typing `fixup! <name>` — the subjects are now
   `<repo>/<name>`, not the bare patchset name.

   To add a **new** patchset, make a normal commit and give it the trailers the
   generator reads (`Target` is authoritative — it's where the `.patch` is
   written):

   ```
   Add VESA linear framebuffer support

   Target: experimental/vesa/vesa.patch
   Repo: vesa
   Patchset: vesa
   ```

   `bootstrap` also records `Style:` (`minus` for `--- \private\` headers,
   `index` for `Index: nt/private/…` SVN-style) and `Eol:` (`lf`/`crlf`) on each
   commit, so the regenerated `.patch` reproduces the original's header style and
   line endings exactly — even when regenerated from scratch with the old files
   deleted. For a brand-new patchset add `Style:`/`Eol:` trailers if you want a
   non-default convention (defaults: `minus`; EOL is `GP_EOL` if set, else
   auto-sniffed from the file's blob in git — a Windows/CRLF checkout produces
   CRLF, a Unix/LF checkout produces LF — falling back to `crlf` when the sniff
   is inconclusive).

   If the new patch belongs to an *earlier* tier than what currently sits at
   HEAD (e.g. you commit a `Repo: minnt` patch after the experimental commits),
   run `reflow` before `generate` — see the reflow section below.

2. **Regenerate and verify:**

   ```bash
   cd ..
   bash src/gitpatch/gitpatch.sh generate --check
   ```

3. Commit the regenerated `.patch` files in the project repo as usual.

Because line numbers are recomputed from real tree states, editing an early
patch and having ten later patches shift is now a non-event: just regenerate.

## Patching a file no existing patch touches yet

The repo only contains files referenced by existing patches, so a not-yet-patched
original file isn't in `.patchsrc`. Its pristine content belongs in the **base**
commit (the base *is* "the original source"); your edit on top is then the patch.
`add-source` does the base part for you:

```bash
# 1. bring the original file in and fold it into the base commit
bash src/gitpatch/gitpatch.sh add-source --src "/m/bld/wk/minnt/base" \
     mvdm/softpc.new/host/src/newfile.c

# 2. edit it and commit as a (new or existing) patchset
cd .patchsrc
$EDITOR mvdm/softpc.new/host/src/newfile.c
git add mvdm/softpc.new/host/src/newfile.c
git commit -F - <<'EOF'
common/newfix

Target: patches/common/newfix.patch
Repo: common
Patchset: newfix
EOF
cd ..

# 3. generate
bash src/gitpatch/gitpatch.sh generate --check
```

`generate` then diffs `base → your commit` and emits a normal modification patch.
Notes:

* `add-source` copies the file from `--src` (use the same source tree you
  bootstrapped from; for an untouched file it's already pristine there), folds it
  into base via an autosquash rebase, and needs a **clean** working tree in
  `.patchsrc`. It skips files already in the repo.
* Do **not** make a separate "add the original file" commit yourself — every
  non-base commit must carry a `Target:` trailer or `generate` errors; only the
  base commit is skipped. That's exactly why `add-source` folds into base.
* **Creating a brand-new file** that doesn't exist in the NT source at all needs
  no `add-source`: just add the file in your patchset commit (step 2 only) and
  `generate` emits a `0a1,N` creation patch (like `ntstubs.c` / `j_c_lang.c`).
* This is a one-time step per file: once the `.patch` exists and is part of the
  build, a later `bootstrap` picks the file up automatically (it's now referenced
  by a patch). Generate before re-bootstrapping, or the manual commit is lost.

---

## Adding a patch in an *earlier* tier: `reflow`

The order of commits in the git history IS the apply order — `generate` diffs
each commit against its parent. A commit tagged `Repo: minnt` sitting *after*
`Repo: haxm/vesa/adlib` commits therefore records a diff computed against the
experimental state, not the minnt state, and the emitted `.patch` misapplies at
autobuild time.

That's exactly the situation when you commit a **new** patches/`<repo>` patch on
top of HEAD after bootstrap: the tail of history is already experimental. The
`reflow` subcommand fixes it:

```bash
cd .patchsrc
# ...you just committed a Repo: minnt patch on top of HEAD...
cd ..
bash src/gitpatch/gitpatch.sh reflow          # moves it between the last
                                              # minnt commit and the first
                                              # experimental commit
bash src/gitpatch/gitpatch.sh generate --check
```

What reflow does:

1. Reads every commit after base, extracts its `Repo:` trailer.
2. Uses `order()` (the canonical tier list for the current `REPO`/`SCOPE`) as
   the tier priority. Untagged commits sort to the end.
3. Stable-sorts: primary key = tier priority, secondary = original position
   (order within a tier is preserved).
4. If the sequence differs from the current one, runs `git rebase -i base` with
   a canned todo — same mechanism `--fixup` / `--autosquash` uses.
5. On conflict (e.g. a later experimental patch touches the same lines as your
   new one), it exits with instructions; resolve as with any git rebase and run
   `git rebase --continue`.
6. Saves `refs/gitpatch/reflow-pre` before touching history, so you can always
   `git reset --hard refs/gitpatch/reflow-pre` to abandon.

Reflow is a no-op when history is already in canonical order — safe to run
routinely after any hand-authored commit. It complements `--fixup` /
`--autosquash`: use those to fold a change into an *existing* commit, and
`reflow` when a **new** commit needs to slot into an earlier tier.

---

## New / "shipped-as-is" files

Some modules ship **whole new files** (e.g. the `cvidcnew` sources, VESA and
AdLib code). These are *not* — and cannot be — normal-diff patches; they're
plain files that the module's `patch.cmd` `xcopy`s into the tree. How they're
tracked:

* **The authoritative copies already live in the project repo** under
  `experimental/<module>/…` and ship from there. Nothing to change.
* In the gitpatch working repo, a new file appears **only if a patch later
  modifies it**. In that case its commit base is the as-shipped file (recovered
  by reverse-apply), and `generate` correctly emits just the *delta* — exactly
  like today's `cvidc-new.patch`. The shipped file itself is never turned into a
  patch.
* New files that are never patched simply aren't in the working repo — they're
  irrelevant to patch generation and ship verbatim from `experimental/<module>/`.

To **audit** the set so nothing is forgotten at release time:

```bash
# Lists files committed in HEAD that don't exist in a pristine tree —
# i.e. files that must be released as-is rather than as a patch.
bash src/gitpatch/gitpatch.sh newfiles --src "/c/.../pristine/nt/private"
```

This catches the dangerous case (a file you *think* is patched but is actually
new, which would otherwise generate a bogus full-file patch against an empty
base). The complete as-is payload for a release is the contents of the
`experimental/<module>/` staging trees (their non-`.patch`, non-`patch.cmd`
files); `newfiles` is the cross-check, not the inventory.

---

## Scope, and how the full stack is reversed

A fully-built tree is `base + common + <repo> + all experimental (+ post-patch
file surgery)`. To recover the base, bootstrap **always reverses the whole stack
top-to-bottom in patched mode**, regardless of `--scope`. This matters even if
you only want the core patches: an experimental change left sitting on top of,
say, `monitor.patch`'s files would make `monitor.patch`'s own reverse mismatch.
So the reverse peels *everything*; `--scope` then controls only which patches are
**replayed as managed commits**:

* `--scope all` (default) — replay `patches/common`, `patches/<repo>` **and**
  `experimental/*`. This keeps the whole set consistent: a core patch and an
  experimental patch often edit the same file (the experimental one applies on
  top), so they share line-number state. If only the core patch were managed,
  regenerating it would shift lines and silently break the hand-maintained
  experimental patch. Managing both means one `generate` re-flows them together.
* `--scope core` — replay only `patches/common` + `patches/<repo>`; experimental
  patches are peeled off to reach the base but not committed. Use this only if
  you deliberately want to manage the core subset alone and keep experimental
  patches by hand.

During the reverse, any file segment that **won't invert** — because it was
deleted (`ccpu386\evidgen.h`) or overwritten by a later module overlay (haxm's
`minnt\v86` clobbering `mvdm\v86\dirs`) — is automatically **frozen** (see
below). bootstrap logs each one, so a built tree bootstraps without hand-holding;
you just get a short list of the files that can't be round-tripped through git.

## Frozen files (patched then deleted or clobbered)

Some files cannot be recovered from a fully-built tree: they were patched and
then **deleted** (`ccpu386\evidgen.h`, removed at the end of the main
`patch.cmd`) or **overwritten** by a later module overlay (haxm's `minnt\v86`
clobbering `mvdm\v86\dirs`). bootstrap detects these — absent files up front,
clobbered files when their reverse won't apply — marks them **frozen**, and:

* reverse/forward-applies only the *other* files in each affected patch, and
* at generate time lifts the frozen file's hunk **verbatim** from the original
  patch, so the regenerated `.patch` is unchanged.

The trade-off: a frozen file's hunk is not managed through git (its line numbers
can't be recomputed — the post-build content is gone). That's fine for generated/
deleted headers and module-overlaid build files. If you ever need to change such
a hunk, edit the saved original under `.patchsrc/.git/gitpatch/orig/<patch>` (or
the generated `.patch`) directly. `verify` covers every non-frozen file; frozen
hunks are correct by construction (copied verbatim).

## Apply order

`generate` and `verify` use the canonical order (mirrors `patch.cmd`):

1. `patches/common/*.patch`
2. `patches/<repo>/*.patch`   (`repo` = `minnt`, override with `GP_REPO=old-src`)
3. `experimental/cvidc-new/` → `cvidc-new.patch`, then `cvidc-minnt.patch` (minnt)
4. `experimental/haxm/`      → `minnt/*.patch`, then `haxm-dpmi.patch` (minnt)
5. `experimental/adlib/*.patch` (minnt)
6. `experimental/vesa/*.patch`

Inspect it any time:

```bash
bash src/gitpatch/gitpatch.sh order      # TARGET <tab> TAG <tab> NAME <tab> ABS
bash src/gitpatch/gitpatch.sh files      # every file referenced by the patches
```

---

## Command reference

| Command | What it does |
|---|---|
| `order` | Print the canonical apply order. |
| `files` | Print every file referenced by the in-scope patches. |
| `bootstrap --src DIR [--mode patched\|clean] [--scope all\|core] [--eol crlf\|lf] [--force]` | Build `.patchsrc` from a source tree (default scope `all` = whole patch set). `--eol` forces a uniform EOL in every committed blob (else sniffed; falls back to `crlf`). |
| `generate [--dest DIR] [--check]` | Regenerate all `.patch` files from git; `--check` runs `verify`. |
| `verify [--dest DIR]` | Apply the (regenerated) patches to base, assert result == git HEAD. |
| `newfiles --src DIR` | List committed files absent from a pristine tree (ship-as-is candidates). |
| `add-source --src DIR <relpath>…` | Bring not-yet-patched original file(s) into the repo and fold them into the base commit (clean tree required). |
| `reflow` | Re-order commits to match the canonical tier order (common → repo → experimental). Use after committing a new patch at HEAD whose `Repo:` trailer belongs to an earlier tier. |

Environment overrides: `GP_REPO` (minnt/old-src), `GP_GITREPO` (working repo
path), `GP_PATCHROOT` (the `ntvdmpatch` dir), `GP_PATCH`/`GP_DIFF` (tool paths;
on git-bash / msys / cygwin, `GP_PATCH` defaults to the bundled
`util/patch.exe` — the same binary the autobuild uses — falling back to
`patch` on PATH; a Windows GNU patch from PATH may silently strip CRs on
reverse-apply and corrupt the base tree),
`GP_EOL` (`native`/`lf`/`crlf`, default auto-sniffed from the file's git blob,
else `crlf`), `GP_STYLE` (`minus`/`index`,
default `minus`) — `GP_EOL`/`GP_STYLE` apply only to commits without a recorded
`Eol:`/`Style:` trailer — and `GP_SCOPE` (`core`/`all`) for the `order`/`files`
introspection commands.

---

## Caveats & notes

* **Correctness vs. byte-identity.** Correctness is guaranteed by `verify`
  (apply → reproduces HEAD). *Byte-identity* with the previous hand-made files
  is best-effort: it holds for content and hunks; two cosmetic things can
  differ — see below — and neither affects whether a patch applies.

* **Two header styles.** Most patches use `--- NT4\private\…` / `+++ nt\private\…`
  (backslashes); `cvidcheap`, `ntvdmctl` and `sasgfx` use the SVN `Index: …` +
  `===` style. Both are parsed on input and faithfully reproduced on output
  (recorded per-commit as `Style:`). Override new patches with `GP_STYLE`.

* **Line endings.** The repo's patches are mixed LF/CRLF (the NT source is CRLF).
  `bootstrap` picks one EOL for the whole `.patchsrc` and normalises every
  file to it *at check-in* — both the base commit and every forward-applied
  patch commit — so `git show` / `git log -p` / `git diff` between any two
  commits read as targeted hunks rather than whole-file rewrites. The EOL is
  picked in this order:

  1. `--eol crlf|lf` on the `bootstrap` command line (or `GP_EOL=…`).
  2. Sniffed from a sample of the copied pristine files (majority wins;
     binary files are skipped).
  3. Default `crlf` (the NT source convention).

  `add-source` uses the same tree EOL for the file it imports; `generate`
  still reproduces each patch's recorded `Eol:` trailer (or, for a brand-new
  patch, sniffs the file's blob), so the emitted `.patch` matches the target
  convention regardless of what the git blob uses internally. When applying
  patches, structural header lines are always CR-stripped (GNU patch needs
  that) and content lines are normalised to the tree's EOL so they byte-match
  the target file. Keep `core.autocrlf=false` in `.patchsrc` (bootstrap sets
  it) so nothing sneaks around this.

* **Multi-file patch ordering.** Within a patch, files are emitted in the order
  recorded by the `Files:` trailer (captured from the original at bootstrap).
  New multi-file patchsets you author later are emitted in git's name order; if
  you need a specific order, set the `Files:` trailer on the commit.

* **Multi-segment files consolidated.** A couple of patches list the *same* file
  in two separate `--- ` segments (e.g. `wow32.patch` for `wudlg.h`,
  `minnt/cvidc.patch` for `cvidc\sources`). git captures all of a file's changes
  in one diff, so regeneration emits a single consolidated segment. The result is
  equivalent and applies to the same end state (verify confirms it), but those
  one or two `.patch` files will differ from the originals after the first
  `generate` — a one-time, intentional cleanup.

* **Experimental module pre/post steps.** The module `patch.cmd`s also `xcopy`
  new files and (haxm) rename `i386`→`x86` *after* patching. Those affect the
  *build*, not patch generation, and `--mode patched` already starts from the
  post-everything tree, so generation is unaffected. If you ever change a module
  to patch a file under a renamed path, just re-run `generate --check`; verify
  will tell you immediately if something doesn't round-trip.

* **Scope.** gitpatch manages the NTVDM source patch sets above. The build-
  environment patches (`minnt/sdk.patch`, `tools.patch`, `be.patch`,
  `wow16-xp.patch`) are applied with `-p0` to non-NTVDM trees and are out of
  scope; keep maintaining those by hand.

---

## Self-test

`selftest.sh` builds a synthetic source tree + two chained patches (the second
depends on the first and also creates a new file) and exercises the whole cycle
— clean bootstrap, reverse-apply bootstrap, generate, verify, CRLF preservation,
and newfiles detection. Run it anywhere to sanity-check the engine:

```bash
bash src/gitpatch/selftest.sh
```
