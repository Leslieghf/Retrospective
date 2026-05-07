# Contracts

Purpose:

- Define the Rust/C ABI that SDK bridge code depends on.
- Define what counts as a breaking change.
- Define required steps when the ABI evolves.

## ABI Surface (Current)

The SDK/loader side expects the following symbols from the Rust `cdylib`:

- `retro_api_version`
- `retro_core_id`
- `retro_init`
- `retro_shutdown`
- `retro_frame`
- `retro_on_map_start`
- `retro_on_player_spawn`
- `retro_on_player_death`
- `retro_frame_count`
- `retro_player_spawn_count`
- `retro_player_death_count`
- `retro_last_spawn_player_slot`
- `retro_last_death_victim_slot`
- `retro_last_death_attacker_slot`
- `retro_status_text`

## Compatibility Rules

Non-breaking changes:

- Internal Rust logic changes behind existing ABI.
- Additional internal C++ helper code without ABI/signature changes.
- New optional behavior that does not remove or alter existing symbols.

Breaking changes:

- Removing or renaming an exported symbol.
- Changing parameter types or return types for exported symbols.
- Changing status code semantics in incompatible ways.

## Versioning Rules

- `retro_api_version()` must bump on breaking ABI changes.
- Bridge code must validate/track expected API version.
- Breaking ABI updates must include:
  - updated bridge bindings
  - updated phase docs
  - migration note in `DECISIONS.md`

## Ownership and Memory Rules

- C++ owns library load/unload lifecycle.
- Rust returns `const char*` pointers to static storage for IDs/status text.
- Callers must not free pointers returned by Rust status/id functions.
