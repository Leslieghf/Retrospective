# Xtask

This repo uses the `cargo xtask` pattern as the primary task runner.

Alias definition:

- [`.cargo/config.toml`](/home/leslieghf/Documents/GitHub/Retrospective/.cargo/config.toml)

Implementation:

- [`rust/xtask/src/main.rs`](/home/leslieghf/Documents/GitHub/Retrospective/rust/xtask/src/main.rs)

## Commands

- `cargo xtask help`
- `cargo xtask audit`
- `cargo xtask sandbox-run`
- `cargo xtask sdk-build`
- `cargo xtask sdk-install-core`
- `cargo xtask sdk-launch`
- `cargo xtask sdk-sync-patches`

## Notes

- `sdk-build` has Linux and Windows command paths.
- `sdk-launch` automation is Linux-only right now; Windows launch remains manual via generated Visual Studio startup profile.
- `sdk-sync-patches` applies a tracked patchset to a clean SDK checkout to avoid repeating manual hook edits.
