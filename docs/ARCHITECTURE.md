# Architecture

## Runtime Layers

1. Source SDK server hook layer (in local SDK checkout)
   - `external/source-sdk-2013/src/game/server/gameinterface.cpp`
2. SDK bridge layer (C++ `dlopen`/`dlsym` wrapper)
   - `native/source_mod/sdk2013/`
3. Shared loader/bridge helpers (repo-owned C++)
   - `native/include/`, `native/src/`, `native/source_mod/`
4. Rust runtime core (`cdylib`)
   - `rust/retrospective_core/src/lib.rs`
5. Script orchestration layer
   - `scripts/`

## Event Flow

1. Launch game with `-retrocore /abs/path/libretrospective_core.so`.
2. SDK `DLLInit` calls bridge `Startup` and `retro_init`.
3. SDK `LevelInit` calls `retro_on_map_start`.
4. SDK `GameFrame` calls `retro_frame`.
5. `player_spawn` / `player_death` events call `retro_on_player_spawn` / `retro_on_player_death`.
6. SDK `DLLShutdown` calls `retro_shutdown` and unloads the module.

## Boundary Rules

- Rust exports only C ABI symbols (`extern "C"` + unmangled names).
- C++ owns dynamic loading and engine-facing pointer/object concerns.
- Rust does not include Source SDK headers and is engine-agnostic by design.
- ABI changes are contract changes and must be documented in `CONTRACTS.md`.

## Ownership

- `rust/retrospective_core/`: runtime/gameplay state evolution.
- `native/`: ABI bridge and loader behavior.
- `scripts/`: developer ergonomics and reproducibility.
- `docs/`: workflow and policy source of truth.
