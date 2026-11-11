#!/bin/sh
# Build with: sh BuildMyGame.sh
set -eu

project_dir=$(CDPATH= cd -- "$(dirname -- "$0")" && pwd)

cmake -S "$project_dir" -B "$project_dir/build" \
    -DCMAKE_PREFIX_PATH=/opt/homebrew \
    -DCMAKE_BUILD_TYPE=Debug

cmake --build "$project_dir/build" --target ReDox --parallel 4

printf '\nBuilt: %s/build/ReDox.exe\n' "$project_dir"

./build/ReDox.exe