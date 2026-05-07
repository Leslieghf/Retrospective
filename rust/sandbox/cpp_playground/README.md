# C++ Playground

Standalone C++20 sandbox for learning syntax, semantics, and standard-library usage.

- No Steam runtime
- No Source SDK linkage
- No third-party dependencies

## Run

From repository root:

```bash
./scripts/run_cpp_playground.sh
```

Or manually:

```bash
cmake -S rust/sandbox/cpp_playground -B build/rust_cpp_playground
cmake --build build/rust_cpp_playground -j
./build/rust_cpp_playground/cpp_playground
```

## What To Edit

- Main file: `rust/sandbox/cpp_playground/src/main.cpp`
- Add your own functions, files, and experiments under `rust/sandbox/cpp_playground/src/`

## Cargo vs CMake (quick map)

- `cargo run` ~= configure + build + run (scripted here in `run_cpp_playground.sh`)
- `Cargo.toml` ~= `CMakeLists.txt` for build configuration
- Cargo includes package/dependency flow; CMake is build-system generation only
