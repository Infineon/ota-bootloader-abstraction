#!/bin/bash

# Usage: ./convert_header_to_mk.sh input.h output_mk_file.mk output_header_file.h

input_file="$1"
output_mk_file="$2"
output_header_file="$3"

# Check if input and output files are provided
if [[ -z "$input_file" || -z "$output_mk_file" || -z "$output_header_file" ]]; then
    echo "Invalid input. Usage: $0 input.h output_mk_file.mk output_header_file.h"
    exit 1
fi

# Check if output file exists, clear the content for the current build.
if [[ -f "$output_mk_file" ]]; then
    > "$output_mk_file"
else
    # Create the output file if it does not exist.
    touch "$output_mk_file"
fi

# Check if output file exists, clear the content for the current build.
if [[ -f "$output_header_file" ]]; then
    > "$output_header_file"
else
    # Create the output file if it does not exist.
    touch "$output_header_file"
fi

# Cross-platform sed inline option
if sed --version >/dev/null 2>&1; then
    # GNU sed (Linux, Git Bash on Windows)
    SED_INPLACE=(-i)
else
    # BSD sed (macOS)
    SED_INPLACE=(-i '')
fi

# Remove comments and '#define' from each line, write to output
grep '^#define' "$input_file" | \
  sed -E 's/^#define[[:space:]]+([A-Za-z0-9_]+)[[:space:]]+\((.*)\)$/\1 := \2/; s/^#define[[:space:]]+([A-Za-z0-9_]+)[[:space:]]+(.*)$/\1 := \2/' > "$output_mk_file"

# Remove any lines containing '#define' from the output file
sed "${SED_INPLACE[@]}" '/#define/d' "$output_mk_file"

# Replace 'MCUBOOT_APP' with 'FLASH_AREA_IMG' in the output file
sed "${SED_INPLACE[@]}" 's/MCUBOOT_APP\([0-9]*_\)/FLASH_AREA_IMG_\1/g' "$output_mk_file"

# Replace 'MCUBOOT_' with 'CY_IFX_MCUBOOT_' in the output file
sed "${SED_INPLACE[@]}" 's/MCUBOOT_/CY_IFX_MCUBOOT_/g' "$output_mk_file"

# Replace ' := ' with '=' in the output file
sed "${SED_INPLACE[@]}" 's/ := /=/g' "$output_mk_file"

# Generating the cy_flash_map.h file
# Copy memorymap.c to ./../source/COMPONENT_IFX_MCUBOOT/ and rename to cy_flash_map.h
src_dir="$(dirname "$input_file")"
cp "$src_dir/memorymap.c" "$output_header_file"

# Remove specific include lines from cy_flash_map.h
sed "${SED_INPLACE[@]}" '/#include "memorymap.h"/d; /#include "flash_map_backend.h"/d' "$output_header_file"

# Rename COMPONENT_MW_IFX_MCUBOOT_PSE84 to COMPONENT_IFX_MCUBOOT in cy_flash_map.h
sed "${SED_INPLACE[@]}" 's/COMPONENT_MW_IFX_MCUBOOT_PSE84/COMPONENT_IFX_MCUBOOT/g' "$output_header_file"

# Rename BOOT_IMAGE_NUMBER to CY_IFX_MCUBOOT_IMAGE_NUMBER in cy_flash_map.h
sed "${SED_INPLACE[@]}" 's/BOOT_IMAGE_NUMBER/CY_IFX_MCUBOOT_IMAGE_NUMBER/g' "$output_header_file"
