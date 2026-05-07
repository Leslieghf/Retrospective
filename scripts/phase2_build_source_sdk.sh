#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SDK_DIR="${1:-$ROOT_DIR/external/source-sdk-2013}"
BUILD_MODE="${2:-release}"

if [[ ! -d "$SDK_DIR/src" ]]; then
  echo "source-sdk-2013 not found at: $SDK_DIR"
  echo "tip: clone it at $ROOT_DIR/external/source-sdk-2013"
  exit 1
fi

if [[ "$BUILD_MODE" != "release" && "$BUILD_MODE" != "debug" ]]; then
  echo "usage: $0 [sdk_dir] [release|debug]"
  exit 1
fi

cd "$SDK_DIR/src"
echo "[phase2] building source-sdk-2013 ($BUILD_MODE)..."
./buildallprojects "$BUILD_MODE"

