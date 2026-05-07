# Workflows

## Local Setup

1. Keep this repo as your working root.
2. Keep Source SDK checkout at `external/source-sdk-2013` (ignored by git here).
3. Ensure Rust + C++ toolchain is installed (see `PHASE1_BOOTSTRAP.md`).

## Daily Loops

Rust core loop:

1. Edit `rust/retrospective_core/src/lib.rs`.
2. Run `cargo test -p retrospective_core`.

C++ learning sandbox loop:

1. Edit `rust/sandbox/cpp_playground/src/main.cpp`.
2. Run `cargo xtask sandbox-run`.
3. Iterate quickly without Steam/SDK/runtime coupling.

SDK integration loop:

1. Start from a clean SDK checkout at `external/source-sdk-2013`.
2. Run `cargo xtask sdk-sync-patches`.
3. Run `cargo xtask sdk-build --mode release`.
4. Run `cargo xtask sdk-install-core --target x86_64-unknown-linux-gnu`.
5. Run `cargo xtask sdk-launch`.
6. In game console, run `retro_status` and `retro_ping`.

## Git Workflow

- Use short-lived branches off `master`.
- Keep PRs scoped to one objective (runtime fix, hook wiring, docs, etc.).
- Prefer `draft` PRs while iterating on hook wiring.
- Link issue(s) in PR sidebar and keep validation commands in PR description.

## Validation Checklist (Before Merge)

- `cargo test -p retrospective_core`
- If SDK hooks changed: phase 2 build + in-game smoke commands
- Update docs if runtime behavior or commands changed

## External Repo Handling

- `external/` is intentionally ignored in this repository.
- SDK changes are real code changes, but they live in the SDK repository history, not this one.

## Platform Support Snapshot

- Cross-platform today:
  - `cargo test -p retrospective_core`
  - `cargo xtask sandbox-run`
- Linux-first today:
  - `cargo xtask sdk-launch` (automated Linux wrapper only)
- Windows SDK path:
  - `cargo xtask sdk-build` includes a Windows command path (`createallprojects.bat` + `msbuild`)
  - launch/debug remains the Visual Studio startup flow from the generated SDK solution
