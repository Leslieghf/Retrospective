#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

echo "[phase1] building Rust core (x86_64)..."
cargo build -p retrospective_core --release

echo "[phase1] building C++ harness..."
mkdir -p build
clang++ -std=c++20 -O2 \
  -I native/include \
  native/src/retrospective_loader.cpp \
  native/src/phase1_harness_main.cpp \
  -ldl \
  -o build/phase1_harness

echo "[phase1] done. run: ./build/phase1_harness"

