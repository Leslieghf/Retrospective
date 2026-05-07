# Retrospective

Internal workspace for Source SDK 2013 + Rust runtime bridge work.

## Docs Entry

Start at [docs/README.md](/home/leslieghf/Documents/GitHub/Retrospective/docs/README.md).

## Repository Map

- `rust/retrospective_core/`: Rust gameplay/runtime core (`cdylib`).
- `native/`: C++ bridge/loader code owned by this repo.
- `scripts/`: build/launch scripts for harness + SDK flow.
- `docs/`: current direction, architecture, contracts, workflows, and phase notes.
- `external/source-sdk-2013/`: local SDK checkout (ignored by this repo).

## Quick Commands

```bash
# list task runner commands
cargo xtask help

# standalone C++20 sandbox (no Source SDK / no Steam)
cargo xtask sandbox-run

# phase 2 flow (SDK integration)
cargo xtask sdk-sync-patches
cargo xtask sdk-build --mode release
cargo xtask sdk-install-core --target x86_64-unknown-linux-gnu
cargo xtask sdk-launch
```

Legacy shell scripts remain available under `scripts/`.
