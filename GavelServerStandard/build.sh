
#!/bin/bash 

# Source helper scripts
if ! source common.sh 2> /dev/null; then
  echo "Error: common.sh not found. Please ensure it's in the same directory." >&2
  exit 1
fi

# Source helper scripts
if ! source testcommon.sh 2> /dev/null; then
  log_error "testcommon.sh not found. Please ensure it's in the same directory." >&2
  exit 1
fi

DO_SHOW=false
BUILD="$1"
CURRENT_DIR="$2"
shift
shift

while [[ $# -gt 0 ]]; do
  case "$1" in
    -v | --verbose) DO_SHOW=true ;;
    --)
      shift
      break
      ;;
    *)
      log_error "Unknown option: $1"
      exit 1
      ;;
  esac
  shift
done

# ------------------------------------------------------------------------------
# Helpers: mapping, auto-generate, auto-clean, orphan cleanup, master header
# ------------------------------------------------------------------------------

# Return extension without leading dot; empty string if no extension
_ext_nodot() {
  local file="$1"
  local ext="${file##*.}"     # part after last dot
  if [[ "$ext" == "$file" ]]; then
    echo ""                   # no dot present
  else
    echo "$ext"
  fi
}

# Map an asset filename to:
#  - header basename (without ".h")
#  - identifier to pass to createfileheader.sh
# Rules:
#   With ext:  header_basename = <base><ext> ; ident = <base><ext>
#   No ext:    header_basename = <base>      ; ident = <base>
_map_asset() {
  local file="$1"             # just the basename (not path)
  local base="${file%.*}"
  local ext="$(_ext_nodot "$file")"

  if [[ -n "$ext" ]]; then
    echo "${base}${ext}|${base}${ext}"
  else
    echo "${base}|${base}"
  fi
}

# Build a master header that includes all generated headers (array of basenames)
# MASTER HEADER NAME: webpage_all.h ; include guard: WEBPAGE_ALL_H
_build_master_header() {
  local out_dir="$1"
  shift
  local headers=( "$@" )  # array of header basenames (without .h), e.g., stylecss, configjson

  local master="$out_dir/webpage_all.h"
  {
    echo "#ifndef __GAVEL_WEBPAGE_ALL_H"
    echo "#define __GAVEL_WEBPAGE_ALL_H"
    echo
    for hb in "${headers[@]}"; do
      echo "#include \"${hb}.h\""
    done
    echo
    echo "#endif // __GAVEL_WEBPAGE_ALL_H "
  } > "$master"
  log_passed "Header file updated: $master"
}

# Create header files in $out_dir from ALL files in $assets_dir
# Header filename = <mapped header_basename>.h ; Ident = <mapped ident>
# Also builds a master header (webpage_all.h) that includes all generated headers.
generate_from_assets() {
  local assets_dir="$1"
  local out_dir="$2"

  [[ ! -d "$out_dir" ]] && mkdir -p "$out_dir"

  shopt -s nullglob

  # Collect generated header basenames to build master header
  local gen_basenames=()

  for src in "$assets_dir"/*; do
    [[ -f "$src" ]] || continue

    local file="$(basename "$src")"
    IFS='|' read -r header_base ident <<< "$(_map_asset "$file")"
    local dst="$out_dir/${header_base}.h"

    createfileheader.sh "$src" "$dst" "$ident"
    gen_basenames+=( "$header_base" )
  done

  shopt -u nullglob

  # Build master header even if empty (produces guard-only file if no assets)
  _build_master_header "$out_dir" "${gen_basenames[@]}"
}

# Delete headers in $out_dir that correspond to current assets in $assets_dir
# (symmetrical to generation) and then remove the directory unconditionally.
clean_from_assets() {
  local assets_dir="$1"
  local out_dir="$2"

  [[ ! -d "$out_dir" ]] && return 0

  shopt -s nullglob

  for src in "$assets_dir"/*; do
    [[ -f "$src" ]] || continue

    local file="$(basename "$src")"
    IFS='|' read -r header_base _ident <<< "$(_map_asset "$file")"
    local dst="$out_dir/${header_base}.h"

    Delete "$dst"
    log_info "Header file deleted: $dst"
  done

  shopt -u nullglob

  local master="$out_dir/webpage_all.h"
  Delete "$master"
  log_info "Header file deleted: $master"

  # Unconditional removal of the output directory (removes master header too)
  rm -r "$out_dir"
}


# ------------------------------------------------------------------------------
# Dispatch
# ------------------------------------------------------------------------------

case "$BUILD" in
  --clean)
    # Symmetric with --build + remove any orphans, then remove directory
    clean_from_assets "$CURRENT_DIR"/assets "$CURRENT_DIR"/src/webpage
    clean_tests $CURRENT_DIR
    ;;

  --pre)
    [[ ! -d "$CURRENT_DIR"/src/webpage ]] && mkdir "$CURRENT_DIR"/src/webpage
    generate_from_assets "$CURRENT_DIR"/assets "$CURRENT_DIR"/src/webpage
    ;;

  --post) ;;

  --build)
    [[ ! -d "$CURRENT_DIR"/src/webpage ]] && mkdir "$CURRENT_DIR"/src/webpage
    generate_from_assets "$CURRENT_DIR"/assets "$CURRENT_DIR"/src/webpage
    ;;

  --test)
    run_tests $CURRENT_DIR $DO_SHOW
    exit $?
    ;;

  --debug)
    #debug_test <current directory> <test name>
    ;;

  *)
    log_failed "Invalid Command Argument: $BUILD"
       exit 1
    ;;
esac

