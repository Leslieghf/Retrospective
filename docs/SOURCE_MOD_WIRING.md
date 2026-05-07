# Source Mod Wiring Draft

This repo now has a Source-facing bridge class:
- `native/source_mod/retrospective_source_bridge.hpp`
- `native/source_mod/retrospective_source_bridge.cpp`

The class wraps Rust lifecycle calls:
- `startup(path)` -> `retro_init`
- `on_level_init(map)` -> `retro_on_map_start`
- `on_game_frame(dt)` -> `retro_frame`
- `shutdown()` -> `retro_shutdown`

## Integration Pattern (Phase 2)

In your Source SDK game code, add one bridge instance in a central lifecycle owner.
Wire calls at high-level hooks:
- game startup / dll init: call `startup(...)`
- level/map init: call `on_level_init(...)`
- per-frame tick: call `on_game_frame(...)`
- game shutdown / dll unload: call `shutdown()`

Keep `retrospective_loader.*` and `retrospective_source_bridge.*` in a shared module that both client/server code can include as needed.

## Notes

- This file does not assume exact hook symbol names yet.
- During actual SDK integration, we will map these calls to concrete locations in your chosen Source 2013 branch files.

