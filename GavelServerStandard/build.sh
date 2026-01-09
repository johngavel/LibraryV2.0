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
REGISTER_FILE="$CURRENT_DIR"/src/register.h
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

addFiles() {
  local type="$1"
  local file="$2"
  {
    echo "void registerStatic${type^^}Files(ArrayDirectory* dir) {"
    "$CURRENT_DIR"/gen_staticfiles.sh -m "$type" "$CURRENT_DIR"/src/webpage
    echo "}"
    echo ""
  } >> "$file"
}

createAddFiles() {
  local TMP_FILE=$(mktemp)

  # Remove old temp files on exit
  trap 'rm -f -- '"$TMP_FILE" RETURN
  {
    echo "#ifndef __GAVEL_SERVER_REGISTER_FILES"
    echo "#define __GAVEL_SERVER_REGISTER_FILES"
    echo ""
    echo '#include "GavelServerStandard.h"'
    echo '#include "webpage/webpage_all.h"'
    echo ""
  } > "$TMP_FILE"
  addFiles "html" "$TMP_FILE"
  addFiles "js" "$TMP_FILE"
  addFiles "css" "$TMP_FILE"
  {
    echo "#endif // __GAVEL_SERVER_REGISTER_FILES"
  } >> "$TMP_FILE"

  # Only replace if different
  if [ ! -f "$REGISTER_FILE" ] || ! cmp -s "$TMP_FILE" "$REGISTER_FILE"; then
    mv "$TMP_FILE" "$REGISTER_FILE"
    log_passed "Header file updated: $REGISTER_FILE"
  else
    log_info "No changes detected. Existing header kept: $REGISTER_FILE"
  fi
}

# ------------------------------------------------------------------------------
# Dispatch
# ------------------------------------------------------------------------------

case "$BUILD" in
  --clean)
    generate_from_assets.sh -c -n SERVER -i "$CURRENT_DIR"/assets -o "$CURRENT_DIR"/src/webpage
    clean_tests $CURRENT_DIR
    DELETE "$REGISTER_FILE"
    ;;

  --pre)
    [[ ! -d "$CURRENT_DIR"/src/webpage ]] && mkdir "$CURRENT_DIR"/src/webpage
    generate_from_assets.sh -b -n SERVER -i "$CURRENT_DIR"/assets -o "$CURRENT_DIR"/src/webpage
    createAddFiles
    ;;

  --post) ;;

  --build)
    [[ ! -d "$CURRENT_DIR"/src/webpage ]] && mkdir "$CURRENT_DIR"/src/webpage
    generate_from_assets.sh -b -n SERVER -i "$CURRENT_DIR"/assets -o "$CURRENT_DIR"/src/webpage
    createAddFiles
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
