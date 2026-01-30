#!/usr/bin/env bash
# includes_by_main_dir.sh
# Version: 1.3.2
#
# Scan recursively in each immediate ("main") subdirectory under ROOT and report
# C/C++ includes. Output is grouped ONLY by each main subdirectory (not deeper).
#
# Modes:
#   --report per-file   (default): show includes per file
#   --report aggregate  : unique include list per main subdirectory
#
# Filters:
#   --starts-with <prefix> : only include headers whose NAME starts with <prefix>
#   --system-only          : only <angle-bracket> includes; exclude "quoted" includes
#
# Other options:
#   --show-root            : include files directly under ROOT (non-recursive)
#   --show-empty           : per-file: list files with no includes
#   -r, --root <path>      : project root (default: .)
#   -h, --help             : help
#       --version          : version

set -euo pipefail

# ---------- Defaults ----------
ROOT="."
REPORT="per-file"   # per-file | aggregate
SHOW_ROOT=0
SHOW_EMPTY=0
LIST_FILES=0
STARTS_WITH=""
SYSTEM_ONLY=0
VERSION="1.3.2"

# ---------- Help ----------
print_help() {
  cat <<'EOF'
includes_by_main_dir.sh — scan C/C++ includes per main subdirectory

USAGE:
  includes_by_main_dir.sh [options]

REPORT MODES:
  --report per-file           Show includes per file (default)
  --report aggregate          Show a unique combined include list per main subdirectory

FILTERS:
  --starts-with <prefix>      Only include headers whose NAME starts with <prefix>
                              (NAME excludes delimiters; e.g., 'string' matches <string>, <string.h>, "string_ext.h")
  --system-only               Only match <angle-bracket> includes; exclude "quoted" includes

OTHER OPTIONS:
  -r, --root <path>           Project root to scan (default: .)
      --show-root             Also include files directly under ROOT (non-recursive)
      --show-empty            (per-file mode) list files that have no includes
      --list-files            (aggregate mode) list which files use each include
  -h, --help                  Show this help and exit
      --version               Show version and exit

SCANNED FILE EXTENSIONS:
  *.c, *.cpp, *.h

NOTES:
  - Parser is CRLF-safe and strips // comments; it does not fully remove multi-line /* ... */ comments.
  - On systems without 'realpath', the script falls back to prefix stripping.
EOF
}

# ---------- Parse args ----------
if [[ $# -eq 1 && ( "$1" == "--help" || "$1" == "-h" ) ]]; then
  print_help; exit 0
fi
if [[ $# -eq 1 && "$1" == "--version" ]]; then
  echo "$VERSION"; exit 0
fi

while [[ $# -gt 0 ]]; do
  case "$1" in
    -r|--root) ROOT="${2:-}"; shift 2 ;;
    --report) REPORT="${2:-}"; shift 2 ;;
    --show-root) SHOW_ROOT=1; shift ;;
    --show-empty) SHOW_EMPTY=1; shift ;;
    --list-files) LIST_FILES=1; shift ;;
    --starts-with) STARTS_WITH="${2:-}"; shift 2 ;;
    --system-only) SYSTEM_ONLY=1; shift ;;
    -h|--help) print_help; exit 0 ;;
    --version) echo "$VERSION"; exit 0 ;;
    --) shift; break ;;
    -*)
      echo "Unknown option: $1" >&2
      echo "Try: includes_by_main_dir.sh --help" >&2
      exit 2 ;;
    *)
      echo "Unexpected argument: $1" >&2
      echo "Try: includes_by_main_dir.sh --help" >&2
      exit 2 ;;
  esac
done

if [[ "$REPORT" != "per-file" && "$REPORT" != "aggregate" ]]; then
  echo "Error: --report must be 'per-file' or 'aggregate'." >&2
  exit 2
fi

ROOT="${ROOT%/}"

# ---------- Portable parsing pipeline ----------

# parse_includes:
# Normalize CRLF, strip // comments, extract include tokens.
# Output per include: TOKEN<TAB>NAME<TAB>KIND
parse_includes() {
  sed $'s/\r$//' \
  | sed -E 's#//.*$##' \
  | grep -E '^[[:space:]]*#[[:space:]]*include[[:space:]]*[<"]' \
  | sed -E 's/.*#[[:space:]]*include[[:space:]]*([<"][^">]+[">]).*/\1\t\1/' \
  | sed -E 's/^(<([^>]+)>|\"([^\"]+)\")\t.*$/\1\t\2\3/' \
  | awk -F'\t' '{ tok=$1; name=$2; kind = (tok ~ /^</) ? "sys" : "local"; print tok "\t" name "\t" kind; }'
}

# filter_includes: apply --starts-with and --system-only
filter_includes() {
  awk -F'\t' -v pref="$STARTS_WITH" -v sys_only="$SYSTEM_ONLY" '
    BEGIN { has_pref = (length(pref) > 0) }
    {
      tok=$1; name=$2; kind=$3;
      if (sys_only == 1 && kind != "sys") next;
      if (has_pref && index(name, pref) != 1) next;
      print;
    }
  '
}

# print_token_field: TOKEN \t NAME \t KIND -> TOKEN
print_token_field() {
  awk -F'\t' '{print $1}'
}

# ---------- File gathering ----------
# Clean 'find' expressions (no inline comments)
gather_recursive() {
  find "$1" -type f \( -iname '*.c' -o -iname '*.cpp' -o -iname '*.h' \) -print0 | sort -z
}
gather_root_only() {
  find "$ROOT" -maxdepth 1 -type f \( -iname '*.c' -o -iname '*.cpp' -o -iname '*.h' \) -print0 | sort -z
}
# If sort -z is unavailable on your system, replace `| sort -z` with nothing,
# or post-process with `tr -d '\0' | sort` in a different approach.

relpath() {
  if command -v realpath >/dev/null 2>&1; then
    realpath --relative-to="$1" "$2"
  else
    echo "${2#$1/}"
  fi
}

# ---------- Reporting: per-file ----------
report_per_file_dir() {
  local maindir="$1"
  mapfile -d '' -t files < <(gather_recursive "$maindir")
  [ "${#files[@]}" -eq 0 ] && return

  echo "${maindir#$ROOT/}/"
  for f in "${files[@]}"; do
    mapfile -t incs < <(parse_includes < "$f" | filter_includes | print_token_field | sort -u)
    local r; r=$(relpath "$maindir" "$f")
    if [ "${#incs[@]}" -gt 0 ]; then
      echo "  $r:"
      for i in "${incs[@]}"; do echo "    - $i"; done
    elif [ "$SHOW_EMPTY" -eq 1 ]; then
      echo "  $r: (no includes)"
    fi
  done
  echo
}

report_per_file_root() {
  [ "$SHOW_ROOT" -ne 1 ] && return 0
  mapfile -d '' -t files < <(gather_root_only)
  [ "${#files[@]}" -eq 0 ] && return 0

  echo "./ (root)/"
  for f in "${files[@]}"; do
    mapfile -t incs < <(parse_includes < "$f" | filter_includes | print_token_field | sort -u)
    if [ "${#incs[@]}" -gt 0 ]; then
      echo "  $(basename "$f"):"
      for i in "${incs[@]}"; do echo "    - $i"; done
    elif [ "$SHOW_EMPTY" -eq 1 ]; then
      echo "  $(basename "$f"): (no includes)"
    fi
  done
  echo
}

# ---------- Reporting: aggregate ----------
report_aggregate_dir() {
  local maindir="$1"
  mapfile -d '' -t files < <(gather_recursive "$maindir")
  [ "${#files[@]}" -eq 0 ] && return

  echo "${maindir#$ROOT/}/"
  declare -A inc_files
  for f in "${files[@]}"; do
    mapfile -t incs < <(parse_includes < "$f" | filter_includes | print_token_field | sort -u)
    local r; r=$(relpath "$maindir" "$f")
    for i in "${incs[@]}"; do
      if [ "$LIST_FILES" -eq 1 ]; then
        inc_files["$i"]+=$'\n'"$r"
      else
        inc_files["$i"]=1
      fi
    done
  done

  mapfile -t keys < <(printf '%s\n' "${!inc_files[@]}" | sort)
  if [ "${#keys[@]}" -eq 0 ]; then
    echo "  (no includes)"
    echo
    return
  fi
  for i in "${keys[@]}"; do
    echo "  - $i"
    if [ "$LIST_FILES" -eq 1 ]; then
      while IFS= read -r f; do
        [ -n "$f" ] && echo "      • $f"
      done <<< "${inc_files[$i]}"
    fi
  done
  echo
}

report_aggregate_root() {
  [ "$SHOW_ROOT" -ne 1 ] && return 0
  mapfile -d '' -t files < <(gather_root_only)
  [ "${#files[@]}" -eq 0 ] && return 0

  echo "./ (root)/"
  declare -A inc_files
  for f in "${files[@]}"; do
    mapfile -t incs < <(parse_includes < "$f" | filter_includes | print_token_field | sort -u)
    for i in "${incs[@]}"; do
      if [ "$LIST_FILES" -eq 1 ]; then
        inc_files["$i"]+=$'\n'"$(basename "$f")"
      else
        inc_files["$i"]=1
      fi
    done
  done

  mapfile -t keys < <(printf '%s\n' "${!inc_files[@]}" | sort)
  if [ "${#keys[@]}" -eq 0 ]; then
    echo "  (no includes)"
    echo
    return
  fi
  for i in "${keys[@]}"; do
    echo "  - $i"
    if [ "$LIST_FILES" -eq 1 ]; then
      while IFS= read -r f; do
        [ -n "$f" ] && echo "      • $f"
      done <<< "${inc_files[$i]}"
    fi
  done
  echo
}

# ---------- Main ----------
main() {
  if [ "$REPORT" = "per-file" ]; then
    report_per_file_root
  else
    report_aggregate_root
  fi

  shopt -s nullglob
  for d in "$ROOT"/*/; do
    d="${d%/}"
    if [ "$REPORT" = "per-file" ]; then
      report_per_file_dir "$d"
    else
      report_aggregate_dir "$d"
    fi
  done
  shopt -u nullglob
}

main