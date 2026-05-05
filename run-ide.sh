#!/usr/bin/env bash
# ============================================================================
# run-ide.sh — One-command Tau IDE launcher (Linux / macOS)
#
# Usage:
#   ./run-ide.sh              # build tau + start IDE on port 8080
#   ./run-ide.sh --port 9090  # use a different port
#   ./run-ide.sh --skip-build # skip C++ build, IDE-only (no Run/Eval/REPL)
#   ./run-ide.sh --help
# ============================================================================
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")" && pwd)"
cd "$SCRIPT_DIR"

PORT=8080
SKIP_BUILD=0
_ALL_CORES=$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)
JOBS=$(( (_ALL_CORES + 1) / 2 ))  # default: half the cores

# ---- Parse arguments -------------------------------------------------------
while [[ $# -gt 0 ]]; do
  case "$1" in
    --port)       PORT="$2"; shift 2 ;;
    --jobs)       JOBS="$2"; shift 2 ;;
    --skip-build) SKIP_BUILD=1; shift ;;
    --help|-h)
      echo "Usage: ./run-ide.sh [--port PORT] [--jobs N] [--skip-build] [--help]"
      echo ""
      echo "  --port PORT    Server port (default: 8080)"
      echo "  --jobs N       Parallel build jobs (default: half your cores)"
      echo "  --skip-build   Skip C++ build (IDE works without tau binary"
      echo "                 for editing, help, grammar, visualization, AI)"
      echo "  --help         Show this message"
      exit 0 ;;
    *) echo "Unknown option: $1. Use --help for usage."; exit 1 ;;
  esac
done

# ---- Helpers ---------------------------------------------------------------
info()  { printf '\033[1;34m[IDE]\033[0m %s\n' "$*"; }
ok()    { printf '\033[1;32m[IDE]\033[0m %s\n' "$*"; }
warn()  { printf '\033[1;33m[IDE]\033[0m %s\n' "$*"; }
fail()  { printf '\033[1;31m[IDE]\033[0m %s\n' "$*"; exit 1; }

# ---- Locate Python ---------------------------------------------------------
PYTHON=""
for cmd in python3 python; do
  if command -v "$cmd" &>/dev/null; then
    ver=$("$cmd" -c 'import sys; print(sys.version_info[:2] >= (3,10))' 2>/dev/null || echo "False")
    if [[ "$ver" == "True" ]]; then
      PYTHON="$cmd"
      break
    fi
  fi
done
[[ -n "$PYTHON" ]] || fail "Python 3.10+ not found. Install it and try again."
info "Using Python: $PYTHON ($($PYTHON --version 2>&1))"

# ---- Fix unordered_dense if needed -----------------------------------------
fix_unordered_dense() {
  local parser_dir="$SCRIPT_DIR/external/parser"
  local vendor_dir="$parser_dir/external/ankerl"
  local header="$vendor_dir/unordered_dense.h"
  if [[ -f "$parser_dir/CMakeLists.txt" ]] && grep -q "unordered_dense" "$parser_dir/CMakeLists.txt" 2>/dev/null; then
    if [[ ! -f "$header" ]]; then
      info "Vendoring ankerl/unordered_dense header (avoids network fetch)..."
      mkdir -p "$vendor_dir"
      local url="https://raw.githubusercontent.com/martinus/unordered_dense/v4.4.0/include/ankerl/unordered_dense.h"
      if command -v curl &>/dev/null; then
        curl -sL "$url" -o "$header"
      elif command -v wget &>/dev/null; then
        wget -q "$url" -O "$header"
      else
        warn "Neither curl nor wget found — cannot vendor header."
        warn "Build may fail if network fetch uses wrong URL."
        return
      fi
      if [[ -s "$header" ]]; then ok "Vendored unordered_dense.h"; else warn "Download failed"; fi
    fi
  fi
}

# ---- Fix missing <mutex> include in parser submodule -----------------------
fix_parser_mutex() {
  local tree_tmpl="$SCRIPT_DIR/external/parser/src/utility/tree.tmpl.h"
  if [[ -f "$tree_tmpl" ]] && grep -q "unique_lock" "$tree_tmpl" 2>/dev/null; then
    if ! grep -q '#include <mutex>' "$tree_tmpl" 2>/dev/null; then
      info "Patching tree.tmpl.h: adding missing #include <mutex>..."
      sed -i '/#include <deque>/a #include <mutex>' "$tree_tmpl"
      ok "Patched tree.tmpl.h"
    fi
  fi
}

# ---- Build tau binary ------------------------------------------------------
TAU_BIN=""
find_tau_binary() {
  for d in build-Release build-Debug build-RelWithDebInfo build; do
    local p="$SCRIPT_DIR/$d/tau"
    [[ -x "$p" ]] && { TAU_BIN="$p"; return 0; }
  done
  if command -v tau &>/dev/null; then
    TAU_BIN="$(command -v tau)"
  fi
  return 0
}

if [[ "$SKIP_BUILD" -eq 0 ]]; then
  find_tau_binary
  if [[ -n "$TAU_BIN" ]]; then
    ok "Found tau binary: $TAU_BIN"
  else
    info "No tau binary found — building from source..."

    # Check C++ compiler
    if ! command -v c++ &>/dev/null && ! command -v g++ &>/dev/null; then
      fail "No C++ compiler found. Install GCC 13+ or Clang 16+ and try again."
    fi

    # Check cmake
    if ! command -v cmake &>/dev/null; then
      fail "cmake not found. Install cmake 3.22+ and try again."
    fi

    # Install Ubuntu CI deps if the script exists and we're on a Debian-like system
    if [[ -f scripts/setup-ubuntu-ci-deps.sh ]] && command -v apt-get &>/dev/null; then
      info "Installing build dependencies via setup-ubuntu-ci-deps.sh..."
      bash scripts/setup-ubuntu-ci-deps.sh || warn "Dependency script had issues — continuing anyway"
    fi

    # Initialize submodules
    if [[ -f .gitmodules ]]; then
      info "Initializing git submodules..."
      git submodule update --init --recursive 2>/dev/null || warn "Submodule update had issues"
    fi

    # Fix known build issues in parser submodule
    fix_unordered_dense
    fix_parser_mutex

    # Build
    info "Building tau (release mode, $JOBS parallel jobs)..."
    if [[ -f release.sh ]]; then
      bash release.sh || fail "Build failed. Check compiler and dependencies."
    else
      mkdir -p build-Release
      cd build-Release
      cmake .. -DCMAKE_BUILD_TYPE=Release 2>&1 | tail -5
      cmake --build . -j "$JOBS" 2>&1 | tail -10
      cd "$SCRIPT_DIR"
    fi

    find_tau_binary
    if [[ -n "$TAU_BIN" ]]; then
      ok "Build successful: $TAU_BIN"
    else
      warn "Build completed but tau binary not found."
      warn "The IDE will still work for editing, help, grammar, visualization, and AI."
      warn "Run/Eval/REPL features require the tau binary."
    fi
  fi
else
  info "Skipping C++ build (--skip-build)"
  find_tau_binary
  if [[ -n "$TAU_BIN" ]]; then
    ok "Found existing tau binary: $TAU_BIN"
  else
    warn "No tau binary found. Run/Eval/REPL will be unavailable."
  fi
fi

# ---- Set up Python venv ----------------------------------------------------
VENV_DIR="$SCRIPT_DIR/.venv"
if [[ ! -d "$VENV_DIR" ]]; then
  info "Creating Python virtual environment..."
  "$PYTHON" -m venv "$VENV_DIR" || fail "Failed to create venv. Install python3-venv: sudo apt install python3-venv"
fi

# Activate
# shellcheck disable=SC1091
source "$VENV_DIR/bin/activate"
info "Activated venv: $VENV_DIR"

# ---- Install Python dependencies -------------------------------------------
info "Installing Python dependencies..."
pip install --quiet --upgrade pip 2>/dev/null || true
pip install --quiet -r ide/requirements.txt || fail "pip install failed"
ok "Python dependencies installed"

# ---- Launch IDE server ------------------------------------------------------
info ""
ok "============================================"
ok "  Tau IDE starting on http://localhost:$PORT"
ok "============================================"
info ""
info "Press Ctrl+C to stop the server."
info ""

exec "$PYTHON" -m uvicorn ide.server:app --host 0.0.0.0 --port "$PORT"
