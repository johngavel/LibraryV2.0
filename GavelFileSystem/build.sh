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

case "$BUILD" in
  --clean) clean_tests $CURRENT_DIR ;;
  --pre) ;;
  --post) ;;
  --build) ;;
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

exit 0
