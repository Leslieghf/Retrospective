# Phase 2 SDK Wiring

Phase 2 is now wired into a real `source-sdk-2013` checkout.

## What Was Integrated

SDK clone location (local workspace):
- `external/source-sdk-2013`

Patched SDK files:
- `src/game/server/gameinterface.cpp`
- `src/game/server/server_base.vpc`
- `src/game/server/retrospective/retrospective_sdk_bridge.h`
- `src/game/server/retrospective/retrospective_sdk_bridge.cpp`

Hook points now call the Rust bridge:
- `CServerGameDLL::DLLInit` (startup/load core)
- `CServerGameDLL::LevelInit` (map start callback)
- `CServerGameDLL::GameFrame` (per-frame callback)
- `CServerGameDLL::DLLShutdown` (shutdown/unload)

Runtime flag:
- `-retrocore /absolute/path/to/libretrospective_core.so`

## Build + Run Commands

Build Source SDK (containerized Steam Runtime):

```bash
./scripts/phase2_build_source_sdk.sh
```

Build Rust core and copy into SDK mod bin:

```bash
./scripts/phase2_build_rust_core_for_sdk.sh
```

Launch HL2MP mod with Rust core attached:

```bash
./scripts/phase2_launch_hl2mp_with_core.sh
```

## Smoke Test Commands

After loading into a map, open the developer console and run:

- `retro_status`
  - prints bridge state + Rust frame counter in console.
- `retro_ping`
  - prints bridge state and draws a visible debug marker/text in front of you.

## Notes

- This validates real SDK integration and linkage for the current official repo build matrix (`hl2mp`, `tf`).
- The official `buildallprojects` script currently generates/builds HL2MP + TF projects by default.
- Your Rust bridge is now a live runtime component for SDK game code via `-retrocore`.
- `external/` is ignored by this repo’s `.gitignore`, so SDK edits are local workspace state, not repo-tracked.

## Troubleshooting (Linux)

- If launch prints Steam init lines then exits immediately, your `Source SDK Base 2013 Multiplayer` install is likely using Proton/Windows files.
- The launch script now checks this explicitly and errors when native files are missing (`hl2.sh` / `hl2_linux`).
- Fix:
  - Steam -> Library -> Tools -> `Source SDK Base 2013 Multiplayer` -> Properties -> Compatibility
  - Disable forced Steam Play/Proton for this tool
  - Verify game files, then rerun `./scripts/phase2_launch_hl2mp_with_core.sh`
- If you hit `Unable to load module engine.so` / `VCvarQuery001`, the root cause on Arch is usually missing `libcurl-gnutls.so.4` in the loader path.
- The launch script now auto-installs `libcurl-gnutls.so.4` into `.../Source SDK Base 2013 Multiplayer/bin/linux64/` from local Steam runtimes when needed.
- If you hit `gameinfo.txt doesn't exist in subdirectory mod_hl2mp`, use the updated launch script: it now passes an absolute `-game` path to the built mod directory.
