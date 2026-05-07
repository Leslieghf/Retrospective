#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
PLAYGROUND_DIR="$ROOT_DIR/rust/sandbox/cpp_playground"
BUILD_DIR="$ROOT_DIR/build/rust_cpp_playground"

cmake -S "$PLAYGROUND_DIR" -B "$BUILD_DIR"
cmake --build "$BUILD_DIR" -j

"$BUILD_DIR/cpp_playground" "$@"
