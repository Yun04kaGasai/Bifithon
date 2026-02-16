#!/usr/bin/env bash
set -euo pipefail

REPO_PATH="${1:-$(pwd)}"

if [[ ! -f "$REPO_PATH/tools/bifc.cpp" ]]; then
  echo "Run this script inside the Bifithon repo root (tools/bifc.cpp not found)." >&2
  exit 1
fi

if command -v apt-get >/dev/null 2>&1; then
  sudo apt-get update
  sudo apt-get install -y g++ cmake libwxgtk3.2-dev
elif command -v pacman >/dev/null 2>&1; then
  sudo pacman -S --needed gcc cmake wxwidgets-gtk3
else
  if ! command -v g++ >/dev/null 2>&1; then
    echo "g++ not found. Install a C++ compiler manually." >&2
    exit 1
  fi
  if ! command -v cmake >/dev/null 2>&1; then
    echo "cmake not found. Install CMake manually." >&2
    exit 1
  fi
  if ! command -v wx-config >/dev/null 2>&1; then
    echo "wxWidgets not found. Install wxWidgets dev package manually." >&2
    exit 1
  fi
fi

cd "$REPO_PATH"

echo "Building C++ compiler..."
g++ -std=c++17 -O2 tools/bifc.cpp -o tools/bifc
chmod +x tools/bifc

echo "Building C++ IDE..."
cmake -S ide -B ide/build
cmake --build ide/build

echo "Done. You can run: ide/build/bifide"
