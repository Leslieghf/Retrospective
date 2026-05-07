#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
cd "$ROOT_DIR"

if ! rustup target list --installed | grep -q '^i686-unknown-linux-gnu$'; then
  echo "missing rust target: i686-unknown-linux-gnu"
  echo "install manually: rustup target add i686-unknown-linux-gnu"
  exit 1
fi

echo "[phase1] building Rust core (i686)..."
cargo build -p retrospective_core --release --target i686-unknown-linux-gnu

echo "[phase1] building C++ harness (i686)..."
mkdir -p build
clang++ -m32 -std=c++20 -O2 \
  -I native/include \
  native/src/retrospective_loader.cpp \
  native/src/phase1_harness_main.cpp \
  -ldl \
  -o build/phase1_harness_32

echo "[phase1] done. run: ./build/phase1_harness_32 target/i686-unknown-linux-gnu/release/libretrospective_core.so"

