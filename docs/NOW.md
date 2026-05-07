# NOW

Date: 2026-05-07

Current direction:

- Keep Source SDK engine integration thin and keep gameplay/runtime logic in Rust.
- Keep the SDK checkout local under `external/source-sdk-2013` and out of this repo history.
- Use the Rust/C ABI bridge as the stable seam between engine-side hooks and runtime logic.
- Treat this repo as the authoritative workflow/documentation surface for the bridge project.
- Use `cargo xtask` as the default automation surface instead of ad-hoc shell entrypoints.

Current scope:

- Validate live runtime path: SDK hooks -> C++ bridge -> Rust core.
- Keep smoke-test commands stable (`retro_status`, `retro_ping`, `retro_pulse`).
- Keep phase scripts reliable for build/run on Linux.
- Build C++ confidence specifically around this codebase (not generic tutorial churn).

Success criteria for current phase:

- Reproducible local dev loop.
- Clear docs for what to run and where to edit.
- Clear compatibility policy for ABI changes.
