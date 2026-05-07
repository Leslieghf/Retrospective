#!/usr/bin/env bash
set -euo pipefail

ROOT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
SDK_DIR="$ROOT_DIR/external/source-sdk-2013"
if [[ $# -gt 0 && -d "$1" && -d "$1/src" && -d "$1/game" ]]; then
  SDK_DIR="$1"
  shift
fi

LAUNCHER="$SDK_DIR/game/mod_hl2mp_linux64"
MOD_DIR="$SDK_DIR/game/mod_hl2mp"
CORE_PATH="$SDK_DIR/game/mod_hl2mp/bin/linux64/retrospective/libretrospective_core.so"
APP_ID="243750"

log() {
  echo "[phase2-launch] $*"
}

die() {
  log "error: $*"
  exit 1
}

if [[ ! -x "$LAUNCHER" ]]; then
  die "missing launcher: $LAUNCHER (run ./scripts/phase2_build_source_sdk.sh)"
fi

if [[ ! -f "$CORE_PATH" ]]; then
  die "missing rust core: $CORE_PATH (run ./scripts/phase2_build_rust_core_for_sdk.sh)"
fi

if [[ ! -f "$MOD_DIR/gameinfo.txt" ]]; then
  die "missing mod gameinfo: $MOD_DIR/gameinfo.txt"
fi

STEAM_ROOT=""
for candidate in "$HOME/.local/share/Steam" "$HOME/.steam/steam"; do
  if [[ -d "$candidate/steamapps" ]]; then
    STEAM_ROOT="$candidate"
    break
  fi
done

if [[ -z "$STEAM_ROOT" ]]; then
  die "could not find Steam library root under ~/.local/share/Steam or ~/.steam/steam"
fi

MANIFEST="$STEAM_ROOT/steamapps/appmanifest_${APP_ID}.acf"
if [[ ! -f "$MANIFEST" ]]; then
  die "missing app manifest: $MANIFEST (install Source SDK Base 2013 Multiplayer in Steam first)"
fi

INSTALL_DIR_NAME="$(awk -F'"' '/"installdir"/ {print $4; exit}' "$MANIFEST")"
if [[ -z "$INSTALL_DIR_NAME" ]]; then
  die "failed to parse installdir from $MANIFEST"
fi

STEAM_APP_DIR="$STEAM_ROOT/steamapps/common/$INSTALL_DIR_NAME"
if [[ ! -d "$STEAM_APP_DIR" ]]; then
  die "Steam app directory not found: $STEAM_APP_DIR"
fi

if [[ ! -x "$STEAM_APP_DIR/hl2.sh" && ! -x "$STEAM_APP_DIR/hl2_linux" ]]; then
  PROTON_HINT=""
  if [[ -f "$STEAM_ROOT/steamapps/compatdata/${APP_ID}/config_info" ]]; then
    PROTON_VER="$(head -n 1 "$STEAM_ROOT/steamapps/compatdata/${APP_ID}/config_info" || true)"
    PROTON_HINT="detected Proton compatibility prefix (${PROTON_VER})."
  fi

  LINUX_DEPOT_HINT=""
  if ! grep -q "\"243754\"" "$MANIFEST"; then
    LINUX_DEPOT_HINT="Manifest does not list Linux depot 243754."
  fi

  die "Steam install at '$STEAM_APP_DIR' is missing native Linux launcher files (expected hl2.sh/hl2_linux). ${PROTON_HINT} ${LINUX_DEPOT_HINT} Fix in Steam: Source SDK Base 2013 Multiplayer -> Properties -> Compatibility -> disable forced Steam Play/Proton, then Verify integrity and relaunch this script."
fi

RUNTIME_LIBCURL_TARGET="$STEAM_APP_DIR/bin/linux64/libcurl-gnutls.so.4"
if [[ ! -f "$RUNTIME_LIBCURL_TARGET" ]] && ! ldconfig -p | grep -q 'libcurl-gnutls\.so\.4'; then
  shopt -s nullglob
  runtime_candidates=(
    "$STEAM_ROOT/steamapps/common/SteamLinuxRuntime_sniper/var"/tmp-*/usr/lib/x86_64-linux-gnu/libcurl-gnutls.so.4
    "$STEAM_ROOT/steamapps/common/SteamLinuxRuntime_soldier/var"/tmp-*/usr/lib/x86_64-linux-gnu/libcurl-gnutls.so.4
  )
  shopt -u nullglob

  if [[ ${#runtime_candidates[@]} -eq 0 ]]; then
    die "missing libcurl-gnutls.so.4 for engine runtime and no Steam runtime candidate was found"
  fi

  install -m 0644 "${runtime_candidates[0]}" "$RUNTIME_LIBCURL_TARGET"
  log "installed runtime dependency: $RUNTIME_LIBCURL_TARGET"
fi

log "launching mod_hl2mp with rust core:"
log "  launcher: $LAUNCHER"
log "  core:     $CORE_PATH"
log "  mod:      $MOD_DIR"
log "  steamapp: $STEAM_APP_DIR"

cd "$SDK_DIR/game"
exec ./mod_hl2mp_linux64 -game "$MOD_DIR" -retrocore "$CORE_PATH" "$@"
