#!/usr/bin/env bash

set -euo pipefail

usage() {
  cat << 'HELP'
Generate StaticFile registration lines from C headers.

Usage:
  gen_staticfiles.sh [OPTIONS] [DIR]

Arguments:
  DIR                 Directory containing .h headers (default: .)

Options:
  -m MATCH            Only process headers whose filename contains MATCH
                      (case-insensitive substring; checked against basename
                      and full path)
  -h                  Show this help

Behavior:
  For each header <basename>.h, print:
    dir->addFile(new StaticFile(<basename>_string, <basename>, <basename>_len));
HELP
}

# Defaults
MATCH=""

# Parse options
while getopts ":m:h" opt; do
  case "$opt" in
    m) MATCH="$OPTARG" ;;
    h)
      usage
      exit 0
      ;;
    \?)
      echo "Unknown option: -$OPTARG" >&2
      usage
      exit 2
      ;;
    :)
      echo "Missing argument for -$OPTARG" >&2
      usage
      exit 2
      ;;
  esac
done
shift $((OPTIND - 1))

DIR="${1:-.}"

# Validate directory
if [[ ! -d $DIR ]]; then
  echo "Error: '$DIR' is not a directory" >&2
  exit 1
fi

# Collect headers (top-level only)
mapfile -t headers < <(find "$DIR" -maxdepth 1 -type f -name '*.h' | sort)

# Helper: filename substring match (case-insensitive) on BASENAME ONLY
filename_matches() {
  local file="$1" pat="$2"
  [[ -z $pat ]] && return 0
  local base
  base="$(basename "$file" .h)"
  shopt -s nocasematch
  [[ $base == *"$pat"* ]]
  local rc=$?
  shopt -u nocasematch
  return $rc
}

# Emit lines
for f in "${headers[@]}"; do
  # Apply filename filter
  if ! filename_matches "$f" "$MATCH"; then
    continue
  fi

  base="$(basename "$f" .h)"
  echo "  dir->addFile(new StaticFile(${base}_string, ${base}, ${base}_len));"
done
