#!/usr/bin/env bash
set -euo pipefail

REPO_PATH="${1:-$(pwd)}"

if [[ ! -f "$REPO_PATH/tools/bifc.py" ]]; then
  echo "Run this script inside the Bifithon repo root (tools/bifc.py not found)." >&2
  exit 1
fi

if ! command -v python3 >/dev/null 2>&1; then
  if command -v apt-get >/dev/null 2>&1; then
    sudo apt-get update
    sudo apt-get install -y python3 python3-tk
  elif command -v pacman >/dev/null 2>&1; then
    sudo pacman -S --needed python tk
  else
    echo "Python not found. Install python3 and tkinter manually." >&2
    exit 1
  fi
fi

if ! command -v g++ >/dev/null 2>&1; then
  if command -v apt-get >/dev/null 2>&1; then
    sudo apt-get update
    sudo apt-get install -y g++
  elif command -v pacman >/dev/null 2>&1; then
    sudo pacman -S --needed gcc
  else
    echo "g++ not found. Install a C++ compiler manually." >&2
    exit 1
  fi
fi

cd "$REPO_PATH"

echo "Building C++ compiler..."
g++ -std=c++17 -O2 tools/bifc.cpp -o tools/bifc
chmod +x tools/bifc

echo "Done. You can run: python3 ide/bifide.py"
