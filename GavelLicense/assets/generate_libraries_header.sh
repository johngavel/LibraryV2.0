#!/bin/bash
# Creates libraries.h with an array of { name, version, license_name }.

# set -xv
set -euo pipefail
IFS=$'\n\t'
shopt -s nullglob

# ---- Inputs ------------------------------------------------------------------
ACTION="${1:-"--build"}"
OUTPUT_DIR="${2:-.}"
OUTPUT_FILE="$OUTPUT_DIR/libraries.h"

# ---- Helpers & fallbacks -----------------------------------------------------
if ! source common.sh 2> /dev/null; then
  log_info() { printf '[INFO] %s\n' "$*"; }
  log_warn() { printf '[WARN] %s\n' "$*"; }
  log_error() { printf '[ERROR] %s\n' "$*"; }
fi

build_library_database() {
  local TMP_FILE=$1
  local START_DIR="$HOME_DIR/libraries"

  # Scan for library.properties and extract name, version, and license file name
  find "$START_DIR" \
    \( -type d -name "Gavel*" -prune \) -o \
    \( -type f -name "library.properties" -print \) 2> /dev/null |
    while read -r file; do
      dir_path=$(dirname "$file")

      lib_name=$(grep -E '^name=' "$file" | cut -d'=' -f2- | tr -d '\r')
      lib_version=$(grep -E '^version=' "$file" | cut -d'=' -f2- | tr -d '\r')
      lib_link=$(grep '^url=' "$file" | cut -d'=' -f2-)

      [ -z "$lib_name" ] && lib_name="UNKNOWN"
      [ -z "$lib_version" ] && lib_version="0.0.0"

      license_file=$(find "$dir_path" -maxdepth 1 -type f \( \
        -iname "license" -o \
        -iname "license.*" -o \
        -iname "licence" -o \
        -iname "licence.*" -o \
        -iname "copying" -o \
        -iname "copying.*" -o \
        -iname "*.adoc" \
        \) -print -quit 2> /dev/null)

      if [ -n "$license_file" ]; then
        license_name=$(basename "$license_file")
      else
        license_name="N/A"
      fi

      # Escape quotes for safety
      lib_name=${lib_name//\"/\\\"}
      lib_version=${lib_version//\"/\\\"}
      license_name=${license_name//\"/\\\"}
      lib_link=${lib_link//\"/\\\"}

      # Output only name, version, license_name
      echo "$lib_name,$lib_version,$license_name,$lib_link" >> "$TMP_FILE"
    done
  PLATFORM_VERSION=$(arduino-cli board details -b rp2040:rp2040:rpipico | grep "Board version" | grep -oE '[0-9]+\.[0-9]+\.[0-9]+')
  echo "Arduino Pico,$PLATFORM_VERSION,LICENSE,https://github.com/earlephilhower/arduino-pico" >> "$TMP_FILE"
  CLIENT_VERSION=$(arduino-cli version | grep -o 'Version: [^ ]*' | cut -d' ' -f2)
  echo "arduino-cli,$CLIENT_VERSION,LICENSE.txt,https://github.com/arduino/arduino-cli" >> "$TMP_FILE"
  echo "Arduino IDE,2.3.7,LICENSE.txt,https://github.com/arduino/arduino-ide" >> "$TMP_FILE"
  echo "Gavel Libraries,2.0.0,,https://github.com/johngavel/LibraryV2.0" >> "$TMP_FILE"
  sort "$TMP_FILE" -o "$TMP_FILE"
}

# ---- Temp files --------------------------------------------------------------
TMP_FILE="$(mktemp)"
TMP_HEADER="$(mktemp)"
trap 'rm -f -- "$TMP_FILE" "$TMP_HEADER"' EXIT

# ---- Build data source -------------------------------------------------------
if ! build_library_database "$TMP_FILE"; then
  log_error "Failed to build library database into $TMP_FILE"
  exit 1
fi

# ---- Actions -----------------------------------------------------------------

if [[ $ACTION == "--clean" ]]; then
  Delete "$OUTPUT_FILE"
  log_info "Cleaned: $OUTPUT_FILE"
  exit 0
fi

if [[ $ACTION == "--build" ]]; then
  {
    echo "#ifndef __GAVEL_LIBRARIES_H"
    echo "#define __GAVEL_LIBRARIES_H"
    echo "/* Auto-generated: library names, versions, and license names */"
    echo
    echo "typedef struct {"
    echo "  const char* name;"
    echo "  const char* version;"
    echo "  const char* license_name;"
    echo "  const char* link;"
    echo "} LibraryInfo;"
    echo
    echo "static const LibraryInfo libraries[] = {"
    # CSV fields: dir,name,version,license,full_license,header_file
    # We use name, version, and license_name (derived from full_license sans extension)
    while IFS=',' read -r name version license_name lib_link; do
      esc_name="${name//\"/\\\"}"
      esc_version="${version//\"/\\\"}"
      esc_license_name="${license_name//\"/\\\"}"
      esc_lib_link="${lib_link//\"/\\\"}"
      echo "  {\"$esc_name\", \"$esc_version\", \"$esc_license_name\", \"$esc_lib_link\" },"
    done < "$TMP_FILE"
    echo "};"
    echo
    echo "#define LIBRARY_COUNT (sizeof(libraries) / sizeof(libraries[0]))"
    echo

    # --- Emit one macro per library for its index ---
    define_count=0
    while IFS=',' read -r name version license_name; do
      # Uppercase and sanitize name for macro
      macro="${name^^}"
      macro="${macro//[^A-Z0-9]/_}"             # replace non-alnum with _
      [[ $macro =~ ^[0-9] ]] && macro="_$macro" # prefix if starts with digit
      echo "#define ${macro}_INDEX $define_count"
      let define_count+=1
    done < "$TMP_FILE"
    let define_count+=5
    echo
    echo "#define MAX_LIBRARIES $define_count"
    echo
    echo "#endif /* __GAVEL_LIBRARIES_H */"
  } > "$TMP_HEADER"

  # Replace only if content differs
  if [[ ! -f $OUTPUT_FILE ]] || ! cmp -s "$TMP_HEADER" "$OUTPUT_FILE"; then
    mv -f -- "$TMP_HEADER" "$OUTPUT_FILE"
    log_info "Header file updated: $OUTPUT_FILE"

  else
    log_info "No changes detected. Existing header kept: $OUTPUT_FILE"
  fi

  exit 0
fi

log_error "Unknown ACTION: $ACTION"
exit 1
