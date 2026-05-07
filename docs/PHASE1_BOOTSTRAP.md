# Phase 1 Bootstrap (Rust + C++ Bridge)

This phase gives you a minimal but real runtime bridge:
- Rust shared library with C ABI exports
- C++ loader that binds symbols with platform dynamic-loader APIs (`dlopen/dlsym` on Unix, `LoadLibrary/GetProcAddress` on Windows)
- Harness executable that simulates Source-side calls (`init`, `map_start`, `frame`, `player_spawn`, `player_death`)

## What Was Added

- Workspace root: `Cargo.toml`
- Rust core crate: `rust/retrospective_core`
- C/C++ ABI header: `native/include/retrospective_ffi.h`
- C++ runtime loader: `native/include/retrospective_loader.hpp`, `native/src/retrospective_loader.cpp`
- Test harness: `native/src/phase1_harness_main.cpp`
- Build scripts:
  - `scripts/build_phase1_harness.sh` (x86_64)
  - `scripts/build_phase1_linux32.sh` (i686, Source-oriented)
  - `scripts/run_phase1_harness.sh`

## Run It (Current Machine)

```bash
./scripts/run_phase1_harness.sh
```

Expected signal:
- library loads
- API version + core id printed
- `init` and `map_start` return `ok`
- `frame_count` increments
- player event counts (`player_spawn_count`, `player_death_count`) update
- invalid event inputs return expected status codes (`invalid_player_slot`, `invalid_victim_slot`)

## Arch Linux: Manual System Dependencies

Install these yourself (I did not install system packages):

```bash
sudo pacman -S --needed base-devel clang lld rustup
```

For 32-bit Source-style build targets:

```bash
sudo pacman -S --needed gcc-multilib lib32-gcc-libs
rustup target add i686-unknown-linux-gnu
```

Optional but recommended for Source SDK Linux/runtime workflows:

```bash
sudo pacman -S --needed podman
```

## RustRover Notes

- Open repo root (`Retrospective`) in RustRover.
- Rust workspace auto-detects from root `Cargo.toml`.
- Use Cargo run config for:
  - `cargo build -p retrospective_core --release`
- Use Shell Script run config for:
  - `./scripts/run_phase1_harness.sh`

## What This Is (and Is Not)

This is a bootstrap harness, not yet injected into Source SDK game loops.
It proves the core boundary and call contract so Phase 2 can wire into actual Source entrypoints.

See also: [SOURCE_MOD_WIRING.md](/home/leslieghf/Documents/GitHub/Retrospective/docs/SOURCE_MOD_WIRING.md)
Implemented wiring: [PHASE2_SDK_WIRING.md](/home/leslieghf/Documents/GitHub/Retrospective/docs/PHASE2_SDK_WIRING.md)
