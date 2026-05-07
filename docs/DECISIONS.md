# Decisions

Format: `Date | Status | Decision | Reason`

- 2026-05-07 | Active | Keep Rust runtime logic in `retrospective_core` and keep SDK hooks thin. | Minimize C++ surface area while integrating with Source.
- 2026-05-07 | Active | Use dynamic loading (`dlopen`/`dlsym`) for the Rust core. | Decouple SDK binary rebuilds from Rust iteration.
- 2026-05-07 | Active | Keep SDK checkout under `external/source-sdk-2013` and out of this repo history. | Separate responsibilities between bridge repo and SDK repo.
- 2026-05-07 | Active | Treat C ABI exports as the primary compatibility contract. | Keep boundary explicit and reviewable.
- 2026-05-07 | Active | Standardize docs around `NOW/ARCHITECTURE/WORKFLOWS/CONTRACTS/DECISIONS/CONTRIBUTING`. | Reduce cognitive load and documentation drift.
