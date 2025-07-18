#!/bin/bash
#
# This file is used by make to create the build commands to sign an OTA image
#
# Modify at your peril !
#
# break immediately on errors
set -e

#
# Arguments
# We have a lot
#
GCC_ARM_TOOLCHAIN_PATH=$1
shift
CY_PYTHON_PATH=$1
shift
CY_OUTPUT_PATH=$1
shift
CY_OUTPUT_NAME=$1
shift
CY_ELF_TO_HEX=$1
shift
CY_ELF_TO_HEX_OPTIONS=$1
shift
CY_ELF_TO_HEX_FILE_ORDER=$1
shift
CY_HEX_TO_BIN=$1
shift
FLASH_ERASE_VALUE=$1
shift
MCUBOOT_HEADER_SIZE=$1
shift
APP_BUILD_VERSION=$1
shift
APP_PLATFORM=$1
shift
FLASH_BASE_ADDRESS=$1
shift
FLASH_BASE_CBUS_ADDRESS=$1
shift
FLASH_AREA_IMG_1_PRIMARY_START=$1
shift
CY_SLOT_SIZE=$1
shift
FLASH_AREA_IMG_1_SECONDARY_START=$1
shift
MCUBOOT_KEY_DIR=$1
shift
MCUBOOT_KEY_FILE=$1
shift
CY_TOC2_GENERATOR=$1
shift
CY_LCS=$1
shift
CY_SIGN_TYPE=$1
shift
APPNAME=$1
shift
APPTYPE=$1
shift
CURRENT_DIRECTORY=$1
shift
SMIF_CRYPTO_CONFIG=$1
shift
OTA_APP_POLICY_PATH=$1
shift
CY_SERVICE_APP_DESCR_ADDR=$1
shift
BOOTSTRAP_SIZE=$1
shift
DEVICE_SRAM_SIZE_KB=$1
shift
CY_SIGN_IMG_ID=$1
shift
OTA_BUILD_POST_VERBOSE=$1
shift
CY_ENC_IMG=$1
shift
CY_ENC_KEY=$1
shift
CY_OEM_PRIVATE_KEY=$1
shift
CY_OTA_APP_ADDRESS=$1
shift
CY_OTA_APP_SECONDARY_ADDRESS=$1
shift
CY_SMIF_ENC=$1
shift
CY_NONCE=$1
shift
CY_OTA_DIRECT_XIP=$1
shift
CY_COMPANY_ID=$1
shift
CY_TLV_INDEX_COMPANY_ID=$1
shift
CY_PRODUCT_ID=$1
shift
CY_TLV_INDEX_PRODUCT_ID=$1
shift
CY_ADD_ARGS=$1

# Export these values for python3 click module
OS_NAME=$(uname -s)
if [ "$OS_NAME" == "Darwin" ]
then
    export LC_ALL=en_US.UTF-8
    export LANG=en_US.UTF-8
else
    export LC_ALL=C.UTF-8
    export LANG=C.UTF-8
fi

CY_OUTPUT_ELF=$CY_OUTPUT_PATH/$CY_OUTPUT_NAME.elf
CY_OUTPUT_BIN=$CY_OUTPUT_PATH/$CY_OUTPUT_NAME.bin
CY_OUTPUT_FINAL_BIN=$CY_OUTPUT_PATH/$CY_OUTPUT_NAME.final.bin
CY_OUTPUT_HEX=$CY_OUTPUT_PATH/$CY_OUTPUT_NAME.unsigned.hex
CY_OUTPUT_FINAL_HEX=$CY_OUTPUT_PATH/$CY_OUTPUT_NAME.hex
CY_OUTPUT_UPGRADE_BIN=$CY_OUTPUT_PATH/$CY_OUTPUT_NAME.upgrade_signed.bin

# Copy final to .final.hex for MTB
CY_OUTPUT_FINAL_FINAL_HEX=$CY_OUTPUT_PATH/$CY_OUTPUT_NAME.final.hex
CY_OUTPUT_FILE_WILD=$CY_OUTPUT_PATH/$CY_OUTPUT_NAME.*
CY_OUTPUT_FILE_NAME_BIN=$CY_OUTPUT_NAME.bin
CY_OUTPUT_FILE_NAME_TAR=$CY_OUTPUT_NAME.tar
CY_COMPONENTS_JSON_NAME=components.json

#
# For elf -> hex conversion
#
if [ "$CY_ELF_TO_HEX_FILE_ORDER" == "elf_first" ]
then
    CY_ELF_TO_HEX_FILE_1=$CY_OUTPUT_ELF
    CY_ELF_TO_HEX_FILE_2=$CY_OUTPUT_HEX
else
    CY_ELF_TO_HEX_FILE_1=$CY_OUTPUT_HEX
    CY_ELF_TO_HEX_FILE_2=$CY_OUTPUT_ELF
fi

# Add base address to offset for PRIMARY slot
FLASH_AREA_IMG_1_PRIMARY_START_ABS=`printf "0x%x\n" $(($FLASH_BASE_ADDRESS + $FLASH_AREA_IMG_1_PRIMARY_START))`
FLASH_AREA_IMG_1_PRIMARY_START_CBUS=`printf "0x%x\n" $(($FLASH_BASE_CBUS_ADDRESS + $FLASH_AREA_IMG_1_PRIMARY_START))`
FLASH_AREA_IMG_1_SECONDARY_START_CBUS=`printf "0x%x\n" $(($FLASH_BASE_CBUS_ADDRESS + $FLASH_AREA_IMG_1_SECONDARY_START))`

#
# Leave here for debugging
echo ""
echo "------------------------------------------ MCUBOOT SIGN Vars ------------------------------------------"
echo "CURRENT_DIRECTORY                       = $CURRENT_DIRECTORY"
echo "GCC_ARM_TOOLCHAIN_PATH                  = $GCC_ARM_TOOLCHAIN_PATH"
echo "CY_PYTHON_PATH                          = $CY_PYTHON_PATH"
echo "CY_OUTPUT_PATH                          = $CY_OUTPUT_PATH"
echo "CY_OUTPUT_NAME                          = $CY_OUTPUT_NAME"
echo "CY_ELF_TO_HEX                           = $CY_ELF_TO_HEX"
echo "CY_ELF_TO_HEX_OPTIONS                   = $CY_ELF_TO_HEX_OPTIONS"
echo "CY_ELF_TO_HEX_FILE_ORDER                = $CY_ELF_TO_HEX_FILE_ORDER"
echo "CY_HEX_TO_BIN                           = $CY_HEX_TO_BIN"
echo "MCUBOOT_HEADER_SIZE                     = $MCUBOOT_HEADER_SIZE"
echo "APP_BUILD_VERSION                       = $APP_BUILD_VERSION"
echo "FLASH_BASE_ADDRESS                      = $FLASH_BASE_ADDRESS"
echo "FLASH_BASE_CBUS_ADDRESS                 = $FLASH_BASE_CBUS_ADDRESS"
echo "FLASH_AREA_IMG_1_PRIMARY_START          = $FLASH_AREA_IMG_1_PRIMARY_START"
echo "FLASH_AREA_IMG_1_PRIMARY_START_ABS      = $FLASH_AREA_IMG_1_PRIMARY_START_ABS"
echo "FLASH_AREA_IMG_1_PRIMARY_START_CBUS     = $FLASH_AREA_IMG_1_PRIMARY_START_CBUS"
echo "FLASH_AREA_IMG_1_SECONDARY_START        = $FLASH_AREA_IMG_1_SECONDARY_START"
echo "FLASH_AREA_IMG_1_SECONDARY_START_CBUS   = $FLASH_AREA_IMG_1_SECONDARY_START_CBUS"
echo "CY_SLOT_SIZE                            = $CY_SLOT_SIZE"
echo "FLASH_ERASE_VALUE                       = $FLASH_ERASE_VALUE"
echo "MCUBOOT_KEY_DIR                         = $MCUBOOT_KEY_DIR"
echo "MCUBOOT_KEY_FILE                        = $MCUBOOT_KEY_FILE"
echo "CY_TOC2_GENERATOR                       = $CY_TOC2_GENERATOR"
echo "CY_SIGN_TYPE                            = $CY_SIGN_TYPE"
echo "APPNAME                                 = $APPNAME"
echo "APPTYPE                                 = $APPTYPE"
echo "SMIF_CRYPTO_CONFIG                      = $SMIF_CRYPTO_CONFIG"
echo "OTA_APP_POLICY_PATH                     = $OTA_APP_POLICY_PATH"
echo "CY_SLOT_SIZE                            = $CY_SLOT_SIZE"
echo "CY_SERVICE_APP_DESCR_ADDR               = $CY_SERVICE_APP_DESCR_ADDR"
echo "BOOTSTRAP_SIZE                          = $BOOTSTRAP_SIZE"
echo "DEVICE_SRAM_SIZE_KB                     = $DEVICE_SRAM_SIZE_KB"
echo "CY_SIGN_IMG_ID                          = $CY_SIGN_IMG_ID"
echo "CY_ENC_IMG                              = $CY_ENC_IMG"
echo "CY_ENC_KEY                              = $CY_ENC_KEY"
echo "CY_OEM_PRIVATE_KEY                      = $CY_OEM_PRIVATE_KEY"
echo "CY_SMIF_ENC                             = $CY_SMIF_ENC"
echo "CY_NONCE                                = $CY_NONCE"
echo "CY_OTA_DIRECT_XIP                       = $CY_OTA_DIRECT_XIP"
echo "CY_COMPANY_ID                           = $CY_COMPANY_ID"
echo "CY_TLV_INDEX_COMPANY_ID                 = $CY_TLV_INDEX_COMPANY_ID"
echo "CY_PRODUCT_ID                           = $CY_PRODUCT_ID"
echo "CY_TLV_INDEX_PRODUCT_ID                 = $CY_TLV_INDEX_PRODUCT_ID"
echo "CY_ADD_ARGS                             = $CY_ADD_ARGS"

echo "Working directory"
pwd
echo ""

if [ "$CY_ENC_IMG" == "1" ]
then
    if [ "$CY_OTA_APP_ADDRESS" == "0" ]
    then
        echo "Note : CY_OTA_APP_ADDRESS is 0 so setting it according to App address in provided flashmap"
        echo ""
        CY_OTA_APP_ADDR=`printf "0x%x\n" $(($FLASH_AREA_IMG_1_PRIMARY_START_CBUS))`
    else
        CY_OTA_APP_ADDR=`printf "0x%x\n" $(($CY_OTA_APP_ADDRESS))`
    fi

    if [ "$CY_SMIF_ENC" == "1" ]
    then
        if [ "$CY_OTA_APP_SECONDARY_ADDRESS" == "0" ]
        then
            echo "Note : CY_OTA_APP_SECONDARY_ADDRESS is 0 so setting it according to App address in provided flashmap"
            echo ""
            CY_OTA_APP_SECONDARY_ADDRESS=`printf "0x%x\n" $(($FLASH_AREA_IMG_1_SECONDARY_START_CBUS))`
        else
            CY_OTA_APP_SECONDARY_ADDRESS=`printf "0x%x\n" $(($CY_OTA_APP_SECONDARY_ADDRESS))`
        fi
    fi
fi

echo "------------------------------- Signing and Encrypting OTA Images -------------------------------"
echo -e " LCS: SECURE \n ENCYPTION : $CY_ENC_IMG \n SMIF_ENCYPTION : $CY_SMIF_ENC"
echo "-------------------------------------------------------------------------------------------------"

echo "[Bin to Hex for debug (unsigned)]"
echo ""
if [ "$OTA_BUILD_POST_VERBOSE" == "1" ]
then
   echo "$CY_HEX_TO_BIN -I binary -O ihex $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.bin $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.unsigned.hex"
fi
$CY_HEX_TO_BIN -I binary -O ihex $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.bin $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.unsigned.hex

echo ""
echo "[TOC2_Generate - Executing toc2 generator script for $CY_OUTPUT_NAME]"
if [ "$OTA_BUILD_POST_VERBOSE" == "1" ]
then
   echo "source $CY_TOC2_GENERATOR NORMAL_NO_SECURE $CY_OUTPUT_PATH $APPNAME $APPTYPE $CURRENT_DIRECTORY $SMIF_CRYPTO_CONFIG $GCC_ARM_TOOLCHAIN_PATH $OTA_APP_POLICY_PATH $CY_SLOT_SIZE 0 $CY_SERVICE_APP_DESCR_ADDR $BOOTSTRAP_SIZE $DEVICE_SRAM_SIZE_KB"
fi
source $CY_TOC2_GENERATOR NORMAL_NO_SECURE $CY_OUTPUT_PATH $APPNAME $APPTYPE $CURRENT_DIRECTORY $SMIF_CRYPTO_CONFIG $GCC_ARM_TOOLCHAIN_PATH $OTA_APP_POLICY_PATH $CY_SLOT_SIZE 0 $CY_SERVICE_APP_DESCR_ADDR $BOOTSTRAP_SIZE $DEVICE_SRAM_SIZE_KB;

if [ "$OTA_BUILD_POST_VERBOSE" == "1" ]
then
    echo "Checking for TOC2 output file"
    ls -l $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.final.bin
    echo ""
fi
echo ""

if [ "$CY_ENC_IMG" == "0" ]
then
    echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
    echo "Default post build script for LCS=SECURE and CY_ENC_IMG=0 combination NOT supported"
    echo "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX"
    exit 0
fi

echo "------------------------------- Signing and Encrypting BOOT Image -------------------------------"
if [ "$CY_OTA_DIRECT_XIP" == "0" ]
then
    echo "[SIGNING BOOT using cysecuretools]"
else
    echo "[SIGNING BOOT using edgeprotecttools]"
fi

if [ "$CY_SMIF_ENC" == "1" ]
then
    if [ "$OTA_BUILD_POST_VERBOSE" == "1" ]
    then
       echo ""
       if [ "$CY_OTA_DIRECT_XIP" == "0" ]
       then
           echo -e "$CY_PYTHON_PATH -m cysecuretools -q -t $APP_PLATFORM -p $OTA_APP_POLICY_PATH sign-image \n --image-format $CY_SIGN_TYPE \n --align 1 \n --version $APP_BUILD_VERSION \n --slot-size $CY_SLOT_SIZE \n --image-id $CY_SIGN_IMG_ID \n --image $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.final.bin \n --output $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.signed.bin \n --key-path $MCUBOOT_KEY_DIR/$MCUBOOT_KEY_FILE $CY_ADD_ARGS \n"
       else
           echo -e "edgeprotecttools sign-image \n --align 1 \n --image-version $APP_BUILD_VERSION \n --slot-size $CY_SLOT_SIZE \n --image $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.final.bin \n --output $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.signed.bin \n --key-path $MCUBOOT_KEY_DIR/$MCUBOOT_KEY_FILE \n --protected-tlv $CY_TLV_INDEX_COMPANY_ID $CY_COMPANY_ID \n --protected-tlv $CY_TLV_INDEX_PRODUCT_ID $CY_PRODUCT_ID \n $CY_ADD_ARGS\n"
       fi
    fi
    if [ "$CY_OTA_DIRECT_XIP" == "0" ]
    then
        $CY_PYTHON_PATH -m cysecuretools -q -t $APP_PLATFORM -p $OTA_APP_POLICY_PATH sign-image \
                             --image-format $CY_SIGN_TYPE \
                             --align 1 \
                             --version $APP_BUILD_VERSION \
                             --slot-size $CY_SLOT_SIZE \
                             --image-id $CY_SIGN_IMG_ID \
                             --image $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.final.bin \
                             --output $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.signed.bin \
                             --key-path $MCUBOOT_KEY_DIR/$MCUBOOT_KEY_FILE \
                             --update-key-path $CY_OEM_PRIVATE_KEY \
                              $CY_ADD_ARGS
    else
        edgeprotecttools sign-image --align 1 \
	                     --image-version $APP_BUILD_VERSION \
	                     --slot-size $CY_SLOT_SIZE \
	                     --image $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.final.bin \
	                     --output $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.signed.bin \
	                     --key-path $MCUBOOT_KEY_DIR/$MCUBOOT_KEY_FILE \
	                     --protected-tlv $CY_TLV_INDEX_COMPANY_ID $CY_COMPANY_ID \
	                     --protected-tlv $CY_TLV_INDEX_PRODUCT_ID $CY_PRODUCT_ID \
	                     $CY_ADD_ARGS
    fi
    echo "[Encrypting BOOT using cysecuretools]"

    if [ "$OTA_BUILD_POST_VERBOSE" == "1" ]
    then
       echo ""
       echo -e "$CY_PYTHON_PATH -m cysecuretools -q -t $APP_PLATFORM encrypt \n --input $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.signed.bin \n --output $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.encrypted.bin \n --iv $CY_OTA_APP_ADDR \n --enckey $CY_ENC_KEY \n --nonce $CY_NONCE \n"
    fi

    $CY_PYTHON_PATH -m cysecuretools -q -t $APP_PLATFORM encrypt --input $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.signed.bin \
                             --output $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.encrypted.bin \
                             --iv $CY_OTA_APP_ADDR \
                             --enckey $CY_ENC_KEY \
                             --nonce $CY_NONCE
else
    if [ "$OTA_BUILD_POST_VERBOSE" == "1" ]
    then
       echo ""
       echo -e "$CY_PYTHON_PATH -m cysecuretools -q -t $APP_PLATFORM -p $OTA_APP_POLICY_PATH sign-image \n --image-format $CY_SIGN_TYPE \n --align 1 \n --version $APP_BUILD_VERSION \n --slot-size $CY_SLOT_SIZE \n --image-id $CY_SIGN_IMG_ID \n --image $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.final.bin \n --output $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.signed.bin \n --key-path $MCUBOOT_KEY_DIR/$MCUBOOT_KEY_FILE \n --update-key-path $CY_OEM_PRIVATE_KEY \n --encrypt --enckey $CY_ENC_KEY \n --app-addr=$CY_OTA_APP_ADDR \n $CY_ADD_ARGS \n"
    fi
    $CY_PYTHON_PATH -m cysecuretools -q -t $APP_PLATFORM -p $OTA_APP_POLICY_PATH sign-image \
                             --image-format $CY_SIGN_TYPE \
                             --align 1 \
                             --version $APP_BUILD_VERSION \
                             --slot-size $CY_SLOT_SIZE \
                             --image-id $CY_SIGN_IMG_ID \
                             --image $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.final.bin \
                             --output $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.signed.bin \
                             --key-path $MCUBOOT_KEY_DIR/$MCUBOOT_KEY_FILE \
                             --update-key-path $CY_OEM_PRIVATE_KEY \
                             --encrypt --enckey $CY_ENC_KEY \
                             --app-addr=$CY_OTA_APP_ADDR \
                             $CY_ADD_ARGS
fi

echo "[Bin to Hex for PRIMARY (BOOT) Slot]"
if [ "$OTA_BUILD_POST_VERBOSE" == "1" ]
then
   echo "$CY_HEX_TO_BIN --change-address=$FLASH_AREA_IMG_1_PRIMARY_START_ABS -I binary -O ihex $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.signed.bin $CY_OUTPUT_FINAL_HEX"
fi
$CY_HEX_TO_BIN --change-address=$FLASH_AREA_IMG_1_PRIMARY_START_ABS -I binary -O ihex $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.signed.bin $CY_OUTPUT_FINAL_HEX

if [ "$OTA_BUILD_POST_VERBOSE" == "1" ]
then
    echo "Copy $CY_OUTPUT_FINAL_HEX to $CY_OUTPUT_FINAL_FINAL_HEX for MTB"
fi
cp $CY_OUTPUT_FINAL_HEX $CY_OUTPUT_FINAL_FINAL_HEX
echo ""

echo "------------------------------- Signing and Encrypting UPGRADE Image -------------------------------"
if [ "$CY_OTA_DIRECT_XIP" == "0" ]
then
    echo "[SIGNING UPGRADE using cysecuretools]"
else
    echo "[SIGNING UPGRADE using edgeprotecttools]"
fi

if [ "$CY_SMIF_ENC" == "1" ]
then
    if [ "$OTA_BUILD_POST_VERBOSE" == "1" ]
    then
       echo ""
       if [ "$CY_OTA_DIRECT_XIP" == "0" ]
       then
           echo -e "$CY_PYTHON_PATH -m cysecuretools -q -t $APP_PLATFORM -p $OTA_APP_POLICY_PATH sign-image \n --image-format $CY_SIGN_TYPE \n --align 1 \n --version $APP_BUILD_VERSION \n --slot-size $CY_SLOT_SIZE \n --image-id $CY_SIGN_IMG_ID \n --image $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.final.bin \n --output $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.upgrade_signed.bin \n --key-path $MCUBOOT_KEY_DIR/$MCUBOOT_KEY_FILE \n --update-key-path $CY_OEM_PRIVATE_KEY \n $CY_ADD_ARGS \n"
       else
           echo -e "edgeprotecttools sign-image \n --align 1 \n --image-version $APP_BUILD_VERSION \n --slot-size $CY_SLOT_SIZE \n --image $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.final.bin \n --output $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.upgrade_signed.bin \n --key-path $MCUBOOT_KEY_DIR/$MCUBOOT_KEY_FILE \n --protected-tlv $CY_TLV_INDEX_COMPANY_ID $CY_COMPANY_ID \n --protected-tlv $CY_TLV_INDEX_PRODUCT_ID $CY_PRODUCT_ID \n $CY_ADD_ARGS\n"
       fi
    fi
    if [ "$CY_OTA_DIRECT_XIP" == "0" ]
    then
        $CY_PYTHON_PATH -m cysecuretools -q -t $APP_PLATFORM -p $OTA_APP_POLICY_PATH sign-image \
                             --image-format $CY_SIGN_TYPE \
                             --align 1 \
                             --version $APP_BUILD_VERSION \
                             --slot-size $CY_SLOT_SIZE \
                             --image-id $CY_SIGN_IMG_ID \
                             --image $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.final.bin \
                             --output $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.upgrade_signed.bin \
                             --key-path $MCUBOOT_KEY_DIR/$MCUBOOT_KEY_FILE \
                             --update-key-path $CY_OEM_PRIVATE_KEY \
                             $CY_ADD_ARGS
    else
        edgeprotecttools sign-image --align 1 \
	                     --image-version $APP_BUILD_VERSION \
	                     --slot-size $CY_SLOT_SIZE \
	                     --image $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.final.bin \
	                     --output $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.upgrade_signed.bin \
	                     --key-path $MCUBOOT_KEY_DIR/$MCUBOOT_KEY_FILE \
	                     --protected-tlv $CY_TLV_INDEX_COMPANY_ID $CY_COMPANY_ID \
	                     --protected-tlv $CY_TLV_INDEX_PRODUCT_ID $CY_PRODUCT_ID \
	                     $CY_ADD_ARGS
    fi

    echo "[Encrypting UPGRADE using cysecuretools]"

    if [ "$OTA_BUILD_POST_VERBOSE" == "1" ]
    then
        echo ""
        echo -e "$CY_PYTHON_PATH -m cysecuretools -q -t $APP_PLATFORM encrypt \n --input $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.upgrade_signed.bin \n --output $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.upgrade_encrypted.bin \n --iv $CY_OTA_APP_SECONDARY_ADDRESS \n --enckey $CY_ENC_KEY \n --nonce $CY_NONCE \n"
    fi

    $CY_PYTHON_PATH -m cysecuretools -q -t $APP_PLATFORM encrypt \
                             --input $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.upgrade_signed.bin \
                             --output $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.upgrade_encrypted.bin \
                             --iv $CY_OTA_APP_SECONDARY_ADDRESS \
                             --enckey $CY_ENC_KEY \
                             --nonce $CY_NONCE
else
    if [ "$OTA_BUILD_POST_VERBOSE" == "1" ]
    then
       echo ""
       echo -e "$CY_PYTHON_PATH -m cysecuretools -q -t $APP_PLATFORM -p $OTA_APP_POLICY_PATH sign-image \n --image-format $CY_SIGN_TYPE \n --align 1 \n --version $APP_BUILD_VERSION \n --slot-size $CY_SLOT_SIZE \n --image $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.final.bin \n --output $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.upgrade_signed.bin \n --key-path $MCUBOOT_KEY_DIR/$MCUBOOT_KEY_FILE \n --update-key-path $CY_OEM_PRIVATE_KEY \n --image-id $CY_SIGN_IMG_ID \n --encrypt --enckey $CY_ENC_KEY \n --app-addr=$CY_OTA_APP_ADDR \n $CY_ADD_ARGS \n"
    fi
    $CY_PYTHON_PATH -m cysecuretools -q -t $APP_PLATFORM -p $OTA_APP_POLICY_PATH sign-image \
                             --image-format $CY_SIGN_TYPE \
                             --align 1 \
                             --version $APP_BUILD_VERSION \
                             --slot-size $CY_SLOT_SIZE \
                             --image $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.final.bin \
                             --output $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.upgrade_signed.bin \
                             --key-path $MCUBOOT_KEY_DIR/$MCUBOOT_KEY_FILE \
                             --update-key-path $CY_OEM_PRIVATE_KEY \
                             --image-id $CY_SIGN_IMG_ID \
                             --encrypt --enckey $CY_ENC_KEY \
                             --app-addr=$CY_OTA_APP_ADDR \
                             $CY_ADD_ARGS
fi

echo "[Bin to Hex for SECONDARY (UPDATE) Slot]"
if [ "$OTA_BUILD_POST_VERBOSE" == "1" ]
then
    echo "$CY_HEX_TO_BIN --change-address=$FLASH_AREA_IMG_1_SECONDARY_START -I binary -O ihex $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.upgrade_signed.bin $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.update.hex"
fi
$CY_HEX_TO_BIN --change-address=$FLASH_AREA_IMG_1_SECONDARY_START -I binary -O ihex $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.upgrade_signed.bin $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.update.hex

# clean up temp files
#rm $CY_OUTPUT_PATH/$CY_OUTPUT_NAME.final.bin

# get size of binary file for components.json
BIN_SIZE=$(ls -g -o $CY_OUTPUT_UPGRADE_BIN | awk '{printf $3}')

# Navigate to build directory
pushd $CY_OUTPUT_PATH
echo ""

echo "------------------------------- Creating upgrade TAR file -------------------------------"
# create "components.json" file
echo "{\"numberOfComponents\":\"2\",\"version\":\"$APP_BUILD_VERSION\",\"files\":["                    >  $CY_COMPONENTS_JSON_NAME
echo "{\"fileName\":\"components.json\",\"fileType\": \"component_list\"},"                             >> $CY_COMPONENTS_JSON_NAME
echo "{\"fileName\":\"$CY_OUTPUT_NAME.upgrade_signed.bin\",\"fileType\": \"NSPE\",\"fileSize\":\"$BIN_SIZE\"}]}" >> $CY_COMPONENTS_JSON_NAME

# create tarball for OTA
echo "Creating tarball"
tar -cf $CY_OUTPUT_FILE_NAME_TAR $CY_COMPONENTS_JSON_NAME $CY_OUTPUT_NAME.upgrade_signed.bin
echo "tarball creation completed"

echo ""
ls -l $CY_OUTPUT_FILE_WILD
echo ""

echo "---------------------------------------- Completed Signing and Encryption post build commands ----------------------------------------"
echo ""
