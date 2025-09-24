#!/bin/bash

#***************************************************NOTE***************************************************
# This script assumes some of the predefined path and hex files names based on the default configurations of
# the code example. There are multiple paramters required for signing an image, whi is not easy to type
# while invoking the script. If user wants to pass the arguments, he can still pass, but with a condition that
# all arguments should be passed. You can't pass selective arguments. However, if you choose to NOT to pass any
# argument, script will use default configuration and signs images and creates combined image for programming.
# TODO: Update the script to support signing one image and allow user to invoke the script with limited options.
# TODO: Invoke this script from Application Makefile.
#

#User provided arguments. Expect Full path to these files
FLASHMAP_MAKEFILE=$1
IN_FILE_1=$2
IN_FILE_2=$3
IN_FILE_3=$4
SRAM_LOADING=$5
KEY=$6

#If you are placing EdgeProtect Bootloader on OSPI, pass this arugment while signing. Otherwise leave it empty.
OSPI_OR_QSPI=$7

if [[ ! (-e $FLASHMAP_MAKEFILE) ]];then
    echo "Error: Flashmap Makefile can't be found "
    exit 1
fi

if [[ ! (-e $IN_FILE_1) ]];then
    echo "Error: Input File (1) can't be found "
    exit 1
fi

if [[ ! (-e $IN_FILE_2) ]];then
    echo "Error: Input File (2) can't be found "
    exit 1
fi

if [[ ! (-e $IN_FILE_3) ]];then
    echo "Error: Input File (3) can't be found "
    exit 1
fi

source $FLASHMAP_MAKEFILE

CY_TOOL_edgeprotecttools_EXE_ABS=edgeprotecttools
MTB_TOOLCHAIN_GCC_ARM__OBJCOPY="C:/Users/Manikonda/Infineon/Tools/mtb-gcc-arm-eabi/14.2.1/gcc/bin/arm-none-eabi-objcopy"

#Assumed paths for output files
OUT_BIN_FILE_1=./build/proj_cm33_s.bin
OUT_BIN_FILE_2=./build/proj_cm33_ns.bin
OUT_BIN_FILE_3=./build/proj_cm55.bin

MERGED_FILE_HEX=./app_combined.hex
MERGED_FILE_BIN=./app_combined.bin

APP_1_NAME=proj_cm33_s
APP_2_NAME=proj_cm33_ns
APP_3_NAME=proj_cm55

FLASH_AREA_EXTERNAL_S_START=0x60000000
FLASH_AREA_EXTERNAL_NS_START=0x70000000

# Calculate start addresses using offsets and base addresses
FLASH_AREA_IMG_1_SECONDARY_START=$(printf "0x%X" $((FLASH_AREA_IMG_1_SECONDARY_OFFSET + FLASH_AREA_EXTERNAL_S_START)))
FLASH_AREA_IMG_2_SECONDARY_START=$(printf "0x%X" $((FLASH_AREA_IMG_2_SECONDARY_OFFSET + FLASH_AREA_EXTERNAL_NS_START)))
FLASH_AREA_IMG_3_SECONDARY_START=$(printf "0x%X" $((FLASH_AREA_IMG_3_SECONDARY_OFFSET + FLASH_AREA_EXTERNAL_NS_START)))

#echo "FLASH_AREA_IMG_1_SECONDARY_START = ${FLASH_AREA_IMG_1_SECONDARY_START}"
#echo "FLASH_AREA_IMG_2_SECONDARY_START = ${FLASH_AREA_IMG_2_SECONDARY_START}"
#echo "FLASH_AREA_IMG_3_SECONDARY_START = ${FLASH_AREA_IMG_3_SECONDARY_START}"

# Convert signed hex file to Binary
echo "Signed .hex to .bin"
echo "$MTB_TOOLCHAIN_GCC_ARM__OBJCOPY --change-address=$FLASH_AREA_IMG_1_SECONDARY_START --input-target=ihex --output-target=binary $IN_FILE_1 ./build/${APP_1_NAME}.bin"
$MTB_TOOLCHAIN_GCC_ARM__OBJCOPY --change-address=$FLASH_AREA_IMG_1_SECONDARY_START --input-target=ihex --output-target=binary $IN_FILE_1 ./build/${APP_1_NAME}.bin

echo""
echo "$MTB_TOOLCHAIN_GCC_ARM__OBJCOPY --change-address=${FLASH_AREA_IMG_2_SECONDARY_START} --input-target=ihex --output-target=binary $IN_FILE_2 ./build/${APP_2_NAME}.bin"
$MTB_TOOLCHAIN_GCC_ARM__OBJCOPY --change-address=${FLASH_AREA_IMG_2_SECONDARY_START} --input-target=ihex --output-target=binary $IN_FILE_2 ./build/${APP_2_NAME}.bin

echo""
echo "$MTB_TOOLCHAIN_GCC_ARM__OBJCOPY --change-address=${FLASH_AREA_IMG_3_SECONDARY_START} --input-target=ihex --output-target=binary $IN_FILE_3 ./build/${APP_3_NAME}.bin"
$MTB_TOOLCHAIN_GCC_ARM__OBJCOPY --change-address=${FLASH_AREA_IMG_3_SECONDARY_START} --input-target=ihex --output-target=binary $IN_FILE_3 ./build/${APP_3_NAME}.bin

APP_1_BIN_SIZE=$(ls -g -o ./build/${APP_1_NAME}.bin | awk '{printf $3}')
APP_2_BIN_SIZE=$(ls -g -o ./build/${APP_2_NAME}.bin | awk '{printf $3}')
APP_3_BIN_SIZE=$(ls -g -o ./build/${APP_3_NAME}.bin | awk '{printf $3}')

echo "APP_1_BIN_SIZE = ${APP_1_BIN_SIZE}"
echo "APP_2_BIN_SIZE = ${APP_2_BIN_SIZE}"
echo "APP_3_BIN_SIZE = ${APP_3_BIN_SIZE}"

# get size of binary file for components.json
CY_OUTPUT_FILE_NAME_TAR=ota-update.tar
CY_COMPONENTS_JSON_NAME=components.json

cd ./build/

# create "components.json" file
echo "{\"numberOfComponents\":\"4\",\"version\":\"${APP_BUILD_VERSION}\",\"files\":["                    >  $CY_COMPONENTS_JSON_NAME
echo "{\"fileName\":\"components.json\"},"                             >> $CY_COMPONENTS_JSON_NAME
echo "{\"fileName\":\"${APP_1_NAME}.bin\",\"fileID\":\"1\",\"fileType\": \"NSPE\",\"fileStartAdd\": \"$FLASH_AREA_IMG_1_SECONDARY_START\",\"fileSize\":\"$APP_1_BIN_SIZE\"}," >> $CY_COMPONENTS_JSON_NAME
echo "{\"fileName\":\"${APP_2_NAME}.bin\",\"fileID\":\"2\",\"fileType\": \"NSPE\",\"fileStartAdd\": \"$FLASH_AREA_IMG_2_SECONDARY_START\",\"fileSize\":\"$APP_2_BIN_SIZE\"}," >> $CY_COMPONENTS_JSON_NAME
echo "{\"fileName\":\"${APP_3_NAME}.bin\",\"fileID\":\"3\",\"fileType\": \"NSPE\",\"fileStartAdd\": \"$FLASH_AREA_IMG_3_SECONDARY_START\",\"fileSize\":\"$APP_3_BIN_SIZE\"}]}" >> $CY_COMPONENTS_JSON_NAME

# create tarball for OTA
echo "Create tarball"
echo "tar -cf $CY_OUTPUT_FILE_NAME_TAR components.json proj_cm33_s.bin proj_cm33_ns.bin proj_cm55.bin"
tar -cf $CY_OUTPUT_FILE_NAME_TAR components.json proj_cm33_s.bin proj_cm33_ns.bin proj_cm55.bin
