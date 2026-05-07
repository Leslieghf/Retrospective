#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SDK_DIR="${1:-$ROOT_DIR/external/source-sdk-2013}"
RUST_TARGET="${2:-x86_64-unknown-linux-gnu}"
MOD_NAME="${3:-mod_hl2mp}"

case "$RUST_TARGET" in
  x86_64-unknown-linux-gnu) PLATFORM_DIR="linux64" ;;
  i686-unknown-linux-gnu) PLATFORM_DIR="linux32" ;;
  *)
    echo "unsupported target: $RUST_TARGET"
    echo "supported: x86_64-unknown-linux-gnu, i686-unknown-linux-gnu"
    exit 1
    ;;
esac

if [[ ! -d "$SDK_DIR/src" ]]; then
  echo "source-sdk-2013 not found at: $SDK_DIR"
  echo "tip: clone it at $ROOT_DIR/external/source-sdk-2013"
  exit 1
fi

cd "$ROOT_DIR"
echo "[phase2] building Rust core for $RUST_TARGET..."
cargo build -p retrospective_core --release --target "$RUST_TARGET"

LIB_PATH="$ROOT_DIR/target/$RUST_TARGET/release/libretrospective_core.so"
if [[ ! -f "$LIB_PATH" ]]; then
  echo "missing built library: $LIB_PATH"
  exit 1
fi

DEST_DIR="$SDK_DIR/game/$MOD_NAME/bin/$PLATFORM_DIR/retrospective"
mkdir -p "$DEST_DIR"
cp "$LIB_PATH" "$DEST_DIR/libretrospective_core.so"

echo "[phase2] installed core to:"
echo "  $DEST_DIR/libretrospective_core.so"

