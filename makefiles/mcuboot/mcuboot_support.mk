################################################################################
# \file mcuboot_support.mk
# \version 1.0
#
# \brief
# Makefile containing the MCUboot flashmap dependencies.
#
################################################################################
# \copyright
# Copyright 2018-2024, Cypress Semiconductor Corporation (an Infineon company)
# SPDX-License-Identifier: Apache-2.0
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
################################################################################

# Find the Relative path between the bootloader support Makefiles and the Application's Build Directory
UNAME_S := $(shell uname -s)
ifeq ($(UNAME_S),Darwin)

    ota_path := $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))
    ota_dir:=$(shell cd $(shell dirname $(ota_path)); pwd)
    CURRENT_DIRECTORY := $(shell PWD)

    # CURRENT_DIRECTORY = $(shell pwd -P)
    RELATIVE_FILE1_FILE2:=$(shell perl -MFile::Spec::Functions=abs2rel -E 'say abs2rel(shift, shift)' $(ota_dir) $(CURRENT_DIRECTORY))
else
    ota_dir := $(realpath $(patsubst %/,%,$(dir $(realpath $(lastword $(MAKEFILE_LIST))))))
    CURRENT_DIRECTORY = $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))
    RELATIVE_FILE1_FILE2:=$(shell realpath --relative-to $(CURRENT_DIRECTORY) $(ota_dir))
endif

###################################################################################################
# OTA SUPPORT
###################################################################################################
# This is the size of the header that imgtool will pre-pend to application
# Must match with MCUBoot
# Do not change
MCUBOOT_HEADER_SIZE=0x400

# Default Bootstrap size for MCUBootloader
APP_BOOTSTRAP_SIZE=0x2400

# Internal and external erased flash values
CY_INTERNAL_FLASH_ERASE_VALUE=0x00
CY_EXTERNAL_FLASH_ERASE_VALUE=0xFF

# Define App version
APP_BUILD_VERSION?=$(APP_VERSION_MAJOR).$(APP_VERSION_MINOR).$(APP_VERSION_BUILD)
DEFINES+=APP_BUILD_VERSION=$(APP_BUILD_VERSION)

###################################################################################################
# Default OTA_PLATFORM based on TARGET
###################################################################################################
#----------------------------------
# CYW989829M2EVB-01
#----------------------------------
ifeq ($(ACTUAL_TARGET), CYW989829M2EVB-01)
    OTA_PLATFORM?=CYW89829
    DEFINES+=CYW89829B01MKSBG
endif

#----------------------------------
# CYW920829M2EVK-02
#----------------------------------
ifeq ($(ACTUAL_TARGET), CYW920829M2EVK-02)
    OTA_PLATFORM?=CYW20829
endif

#----------------------------------
# CY8CKIT-062S2-43012
#----------------------------------
ifeq ($(ACTUAL_TARGET), CY8CKIT-062S2-43012)
    OTA_PLATFORM?=PSOC_062_2M
endif

#----------------------------------
# CY8CPROTO-062-4343W
#----------------------------------
ifeq ($(ACTUAL_TARGET), CY8CPROTO-062-4343W)
    OTA_PLATFORM?=PSOC_062_2M
endif

#----------------------------------
# CY8CPROTO-062S3-4343W
#----------------------------------
ifeq ($(ACTUAL_TARGET), CY8CPROTO-062S3-4343W)
    OTA_PLATFORM?=PSOC_062_512K
endif

#----------------------------------
# CY8CEVAL-062S2-CYW943439M2IPA1
#----------------------------------
ifeq ($(ACTUAL_TARGET), CY8CEVAL-062S2-CYW943439M2IPA1)
    OTA_PLATFORM?=PSOC_062_2M
endif

#----------------------------------
# CY8CEVAL-062S2-LAI-4373M2
#----------------------------------
ifeq ($(ACTUAL_TARGET), CY8CEVAL-062S2-LAI-4373M2)
    OTA_PLATFORM?=PSOC_062_2M
endif

#----------------------------------
# CY8CEVAL-062S2-MUR-43439M2
#----------------------------------
ifeq ($(ACTUAL_TARGET), CY8CEVAL-062S2-MUR-43439M2)
    OTA_PLATFORM?=PSOC_062_2M
endif

#----------------------------------
# CY8CKIT-062-BLE
#----------------------------------
ifeq ($(ACTUAL_TARGET), CY8CKIT-062-BLE)
    OTA_PLATFORM?=PSOC_062_1M
endif

#----------------------------------
# CY8CPROTO-063-BLE
#----------------------------------
ifeq ($(ACTUAL_TARGET), CY8CPROTO-063-BLE)
    OTA_PLATFORM?=PSOC_063_1M
endif

#----------------------------------
# CYBLE-416045-EVAL
#----------------------------------
ifeq ($(ACTUAL_TARGET), CYBLE-416045-EVAL)
    OTA_PLATFORM?=PSOC_063_1M
endif

#----------------------------------
# CY8CKIT-064B0S2-4343W
#----------------------------------
ifeq ($(ACTUAL_TARGET), CY8CKIT-064B0S2-4343W)
    OTA_PLATFORM?=PSOC_064_2M
endif

#----------------------------------
# KIT_XMC72_EVK
#----------------------------------
ifeq ($(ACTUAL_TARGET), KIT_XMC72_EVK)
    OTA_PLATFORM?=XMC7200
endif

#----------------------------------
# KIT_XMC72_EVK_MUR_43439M2
#----------------------------------
ifeq ($(ACTUAL_TARGET), KIT_XMC72_EVK_MUR_43439M2)
    OTA_PLATFORM?=XMC7200
endif

#----------------------------------
# KIT_XMC71_EVK_LITE_V1
#----------------------------------
ifeq ($(ACTUAL_TARGET), KIT_XMC71_EVK_LITE_V1)
    OTA_PLATFORM?=XMC7100
endif

#----------------------------------
# CY8CEVAL-062S2-CYW955513SDM2WLIPA
#----------------------------------
ifeq ($(ACTUAL_TARGET), CY8CEVAL-062S2-CYW955513SDM2WLIPA)
OTA_PLATFORM?=PSOC_062_2M
endif

###################################################################################################
# Default flashmaps based on OTA_PLATFORM
###################################################################################################
ifeq ($(OTA_PLATFORM),PSOC_062_2M)
    # Default to Internal + External Flash, Swap, Single Image
    OTA_FLASH_MAP?=$(RELATIVE_FILE1_FILE2)/../../configs/COMPONENT_MCUBOOT/flashmap/psoc62_2m_ext_swap_single.json
endif

ifeq ($(OTA_PLATFORM),PSOC_062_1M)
    # Default to int Swap, BLESS requires cm0p code inclusion
    OTA_FLASH_MAP?=$(RELATIVE_FILE1_FILE2)/../../configs/COMPONENT_MCUBOOT/flashmap/psoc62_1m_cm0_int_swap_single.json
endif

ifeq ($(OTA_PLATFORM),PSOC_063_1M)
    # Default to int Swap, BLESS requires cm0p code inclusion
    OTA_FLASH_MAP?=$(RELATIVE_FILE1_FILE2)/../../configs/COMPONENT_MCUBOOT/flashmap/psoc63_1m_cm0_int_swap_single.json
endif

ifeq ($(OTA_PLATFORM),PSOC_062_512K)
    OTA_FLASH_MAP?=$(RELATIVE_FILE1_FILE2)/../../configs/COMPONENT_MCUBOOT/flashmap/psoc62_512k_xip_swap_single.json
endif

ifeq ($(OTA_PLATFORM),PSOC_064_2M)
    # Default to ext Swap, we need to define for later in the Makefile
    # Although we don't actually use it.
    OTA_FLASH_MAP?=$(RELATIVE_FILE1_FILE2)/../../configs/COMPONENT_MCUBOOT/flashmap/policy_single_CM0_CM4_smif_swap.json
    # Policy name here for reference when provisioning CY8CKIT-064B0S2-4343W
    CY_SECURE_POLICY_NAME = policy_single_CM0_CM4_smif_swap
endif

ifeq ($(OTA_PLATFORM),CYW20829)
    # Default to XIP Swap
    OTA_FLASH_MAP?=$(RELATIVE_FILE1_FILE2)/../../configs/COMPONENT_MCUBOOT/flashmap/cyw20829_xip_swap_single.json
endif

ifeq ($(OTA_PLATFORM),CYW89829)
    # Default to XIP Swap
    OTA_FLASH_MAP?=$(RELATIVE_FILE1_FILE2)/../../configs/COMPONENT_MCUBOOT/flashmap/cyw89829_xip_overwrite_single.json
endif

ifeq ($(OTA_PLATFORM),XMC7100)
    OTA_FLASH_MAP?=$(RELATIVE_FILE1_FILE2)/../../configs/COMPONENT_MCUBOOT/flashmap/xmc7100_int_swap_single.json
    OTA_PLATFORM_CONFIG_JSON=$(RELATIVE_FILE1_FILE2)/../../configs/COMPONENT_MCUBOOT/flashmap/xmc7100_platform.json
endif

ifeq ($(OTA_PLATFORM),XMC7200)
    OTA_FLASH_MAP?=$(RELATIVE_FILE1_FILE2)/../../configs/COMPONENT_MCUBOOT/flashmap/xmc7200_int_swap_single.json
    OTA_PLATFORM_CONFIG_JSON=$(RELATIVE_FILE1_FILE2)/../../configs/COMPONENT_MCUBOOT/flashmap/xmc7200_platform.json
endif

#############################
# Check for Required defines
#############################

# MUST provide platform type
# Must be one of these names
OTA_PSOC_062_SUPPORTED_PLATFORMS="PSOC_062_512K PSOC_062_1M PSOC_062_2M "
OTA_PSOC_063_SUPPORTED_PLATFORMS="PSOC_063_1M "
OTA_PSOC_064_SUPPORTED_PLATFORMS="PSOC_064_2M "
OTA_PSOC_06X_SUPPORTED_PLATFORMS=$(OTA_PSOC_062_SUPPORTED_PLATFORMS)$(OTA_PSOC_063_SUPPORTED_PLATFORMS)$(OTA_PSOC_064_SUPPORTED_PLATFORMS)
OTA_OTHER_SUPPORTED_PLATFORMS="CYW20829 CYW89829 XMC7100 XMC7200"
OTA_SUPPORTED_PLATFORMS=$(OTA_PSOC_06X_SUPPORTED_PLATFORMS) $(OTA_OTHER_SUPPORTED_PLATFORMS)

# Add OTA_PLATFORM in DEFINES for platform-specific code
DEFINES+=$(OTA_PLATFORM)

# for use when running flashmap.py
FLASHMAP_PLATFORM=$(OTA_PLATFORM)

# For generalized PSOC based specifics
ifneq ($(findstring $(OTA_PLATFORM), $(OTA_PSOC_06X_SUPPORTED_PLATFORMS)),)
    #ifeq ($(OTA_BUILD_VERBOSE),2)
        $(info FOUND $(OTA_PLATFORM) Use COMPONENT_OTA_PSOC_062)
    #endif
    COMPONENTS+=OTA_PSOC_062
endif

###################################################################################################
# OTA Flashmap JSON file Usage
###################################################################################################
#
# flashmap.py creates flashmap.mk, a number of defines which are then included in the build.
# It is important to include the defines here (and not just in cy_flash_map.c) as they are used in POSTBUILD processing
#
# Look in configs/COMPONENT_MCUBOOT/flashmap for an appropriate JSON file
#
# CYW920829 and CYW89829
#    Single image JSON files, always external flash
#        flashmap/cyw20829_ext_overwrite_single.json
#        flashmap/cyw20829_ext_swap_single.json
# CY8CPROTO-062S3-4343W
#    Single image JSON files, internal flash
#        flashmap/psoc62s3_int_overwrite_single.json
#        flashmap/psoc62s3_int_swap_single.json
#    Single image JSON files, XIP from external flash
#        flashmap/psoc62s3_xip_swap_single.json
# CY8CKIT-062-BLE
#    Single image JSON files, internal flash
#        flashmap/psoc62_1m_int_swap_single.json
#    Single image JSON files, XIP from external flash
#        flashmap/psoc62_1m_xip_swap_single.json
# Other PSoC6 targets
#    Single image JSON files, internal flash only
#        flashmap/psoc62_int_overwrite_single.json
#        flashmap/psoc62_int_swap_single.json
#    Single image JSON files, internal + external flash
#        flashmap/psoc62_ext_overwrite_single.json
#        flashmap/psoc62_ext_swap_single.json
#    Single image JSON files, XIP from external flash
#        flashmap/psoc62_xip_swap_single.json
#
# CY8CPROTO-063-BLE
#    Single image JSON files, internal flash
#        flashmap/psoc63_1m_int_swap_single.json
#    Single image JSON files, XIP from external flash
#        flashmap/psoc63_1m_xip_swap_single.json
#
# CY8CKIT-064B0S2-4343W
#    DOES NOT USE JSON FILE at this time
#
# Example output that is piped into flashmap.mk for an XIP application:
#
# AUTO-GENERATED FILE, DO NOT EDIT. ALL CHANGES WILL BE LOST!
# FLASH_AREA_BOOTLOADER_DEV_ID :=FLASH_DEVICE_INTERNAL_FLASH
# FLASH_AREA_BOOTLOADER_START :=0x000000
# FLASH_AREA_BOOTLOADER_SIZE :=0x018000
# FLASH_AREA_IMG_1_PRIMARY_DEV_ID :=FLASH_DEVICE_EXTERNAL_FLASH(CY_BOOT_EXTERNAL_DEVICE_INDEX)
# FLASH_AREA_IMG_1_PRIMARY_START :=0x000000
# FLASH_AREA_IMG_1_PRIMARY_SIZE :=0x140200
# FLASH_AREA_IMG_1_SECONDARY_DEV_ID :=FLASH_DEVICE_EXTERNAL_FLASH(CY_BOOT_EXTERNAL_DEVICE_INDEX)
# FLASH_AREA_IMG_1_SECONDARY_START :=0x180000
# FLASH_AREA_IMG_1_SECONDARY_SIZE :=0x140200
# FLASH_AREA_IMAGE_SWAP_STATUS_DEV_ID :=FLASH_DEVICE_INTERNAL_FLASH
# FLASH_AREA_IMAGE_SWAP_STATUS_START :=0x018000
# FLASH_AREA_IMAGE_SWAP_STATUS_SIZE :=0x003c00
# FLASH_AREA_IMAGE_SCRATCH_DEV_ID :=FLASH_DEVICE_EXTERNAL_FLASH(CY_BOOT_EXTERNAL_DEVICE_INDEX)
# FLASH_AREA_IMAGE_SCRATCH_START :=0x440000
# FLASH_AREA_IMAGE_SCRATCH_SIZE :=0x080000
# MCUBOOT_IMAGE_NUMBER := 1
# MCUBOOT_MAX_IMG_SECTORS := 32
# USE_EXTERNAL_FLASH := 1
# USE_XIP := 1
#
# Always update the flash area definitions.
# Note: The build system will not re-build all files as the defines have not changed since last build.
#$(shell rm -f flashmap.mk)

ifeq ($(OTA_PLATFORM),PSOC_064_2M)
    # CY8CKIT-064B0S2-4343W
    # Single image JSON File, int + ext flash
    # flashmap.py does not support this platform at this time
    $(info flashmap.py does not support PSOC_064_2M - We write out directly here!)            #IMP_CHECK

    # Define FLASHMAP_PYTHON_SCRIPT as nothing so it is not executed after start.mk at end of Makefile
    FLASHMAP_PYTHON_SCRIPT=

    $(shell echo "# AUTO-GENERATED FILE, DO NOT EDIT. ALL CHANGES WILL BE LOST!" > flashmap.mk)
    $(shell echo "FLASH_AREA_BOOTLOADER_DEV_ID :=FLASH_DEVICE_INTERNAL_FLASH" >> flashmap.mk)
    $(shell echo "FLASH_AREA_BOOTLOADER_START :=0x1e0000" >> flashmap.mk)
    $(shell echo "FLASH_AREA_BOOTLOADER_SIZE :=0x018000" >> flashmap.mk)
    $(shell echo "FLASH_AREA_IMG_1_PRIMARY_DEV_ID :=FLASH_DEVICE_INTERNAL_FLASH" >> flashmap.mk)
    $(shell echo "FLASH_AREA_IMG_1_PRIMARY_START :=0x000000" >> flashmap.mk)
    $(shell echo "FLASH_AREA_IMG_1_PRIMARY_SIZE :=0x1c8000" >> flashmap.mk)
    $(shell echo "FLASH_AREA_IMG_1_SECONDARY_DEV_ID :=FLASH_DEVICE_EXTERNAL_FLASH(CY_BOOT_EXTERNAL_DEVICE_INDEX)" >> flashmap.mk)
    $(shell echo "FLASH_AREA_IMG_1_SECONDARY_START :=0x038400" >> flashmap.mk)
    $(shell echo "FLASH_AREA_IMG_1_SECONDARY_SIZE :=0x1c8000" >> flashmap.mk)
    $(shell echo "FLASH_AREA_IMAGE_SWAP_STATUS_DEV_ID :=FLASH_DEVICE_INTERNAL_FLASH" >> flashmap.mk)
    $(shell echo "FLASH_AREA_IMAGE_SWAP_STATUS_START :=0x1cc000" >> flashmap.mk)
    $(shell echo "FLASH_AREA_IMAGE_SWAP_STATUS_SIZE :=0x006c00" >> flashmap.mk)
    $(shell echo "FLASH_AREA_IMAGE_SCRATCH_DEV_ID :=FLASH_DEVICE_EXTERNAL_FLASH(CY_BOOT_EXTERNAL_DEVICE_INDEX)" >> flashmap.mk)
    $(shell echo "FLASH_AREA_IMAGE_SCRATCH_START :=0x280000" >> flashmap.mk)
    $(shell echo "FLASH_AREA_IMAGE_SCRATCH_SIZE :=0x0c0000" >> flashmap.mk)
    $(shell echo "MCUBOOT_IMAGE_NUMBER := 1" >> flashmap.mk)
    $(shell echo "MCUBOOT_MAX_IMG_SECTORS := 3648" >> flashmap.mk)
    $(shell echo "USE_EXTERNAL_FLASH := 1" >> flashmap.mk)

else
    # non-PSOC_064_2M here
    ifneq ($(findstring $(MAKECMDGOALS), build prebuild build_proj program program_proj eclipse vscode ewarm ewarm8 uvision uvision5 all),)
        ifeq ($(CY_PYTHON_PATH),)
            $(error CY_PYTHON_PATH is not configured)
        else
            $(info CY_PYTHON_PATH = $(CY_PYTHON_PATH))
        endif # checking for python path

        ifeq ($(OTA_PLATFORM),$(filter $(OTA_PLATFORM),XMC7100 XMC7200))
            # XMC7000 here
            FLASHMAP_PYTHON_SCRIPT=flashmap_xmc.py
            IMG_ID=1
            ifneq ($(FLASHMAP_PYTHON_SCRIPT),)
                $(info : OTA_FLASH_MAP = $(OTA_FLASH_MAP))
                $(info "flashmap_xmc.py $(CY_PYTHON_PATH) $(RELATIVE_FILE1_FILE2)/../../scripts/mcuboot/$(FLASHMAP_PYTHON_SCRIPT) run -p $(OTA_PLATFORM_CONFIG_JSON) -i $(OTA_FLASH_MAP) -o $(RELATIVE_FILE1_FILE2)/../../source/COMPONENT_MCUBOOT/ -n cy_flash_map -d $(IMG_ID) > flashmap.mk")
                $(shell $(CY_PYTHON_PATH) $(RELATIVE_FILE1_FILE2)/../../scripts/mcuboot/$(FLASHMAP_PYTHON_SCRIPT) run -p $(OTA_PLATFORM_CONFIG_JSON) -i $(OTA_FLASH_MAP) -o $(RELATIVE_FILE1_FILE2)/../../source/COMPONENT_MCUBOOT/ -n cy_flash_map -d $(IMG_ID) > flashmap.mk)
            endif # check for flashmap name
        else
            # Non-XMC7000 here
            FLASHMAP_PYTHON_SCRIPT=flashmap.py
            ifneq ($(FLASHMAP_PYTHON_SCRIPT),)
                $(info OTA_FLASH_MAP = $(OTA_FLASH_MAP))
                $(info Parsing flashmap and generating flashmap.mk ...)
                ifeq ($(OTA_BUILD_FLASH_VERBOSE),1)
                    $(info "flashmap.py $(CY_PYTHON_PATH) $(RELATIVE_FILE1_FILE2)/../../scripts/mcuboot/$(FLASHMAP_PYTHON_SCRIPT) -p $(FLASHMAP_PLATFORM) -i $(OTA_FLASH_MAP) -o $(RELATIVE_FILE1_FILE2)/../../source/COMPONENT_MCUBOOT/cy_flash_map.h > flashmap.mk")
                endif
                $(shell $(CY_PYTHON_PATH) $(RELATIVE_FILE1_FILE2)/../../scripts/mcuboot/$(FLASHMAP_PYTHON_SCRIPT) -p $(FLASHMAP_PLATFORM) -i $(OTA_FLASH_MAP) -o $(RELATIVE_FILE1_FILE2)/../../source/COMPONENT_MCUBOOT/cy_flash_map.h > flashmap.mk)
            endif # check for flashmap name
        endif # XMC7000 check

        flash_map_status=$(shell if [ -s "flashmap.mk" ]; then echo "success"; fi )
        ifeq ($(flash_map_status),)
            $(info "")
            $(error Failed to create flashmap.mk !)
            $(info "")
        endif # check for flashmap.mk created
    endif # check for build command
endif # PSOC_064_2M check

flash_map_mk_exists=$(shell if [ -s "flashmap.mk" ]; then echo "success"; fi )

ifneq ($(flash_map_mk_exists),)
    $(info include flashmap.mk)
    include ./flashmap.mk
endif # flash_map_mk_exists

############################
# IF FLASH_MAP sets USE_XIP,
#    we are executing code
#    from external flash
############################

ifeq ($(USE_XIP),1)
    ifeq ($(OTA_BUILD_VERBOSE),1)
        $(info Makefile: USE_XIP=1)
    endif

    CY_RUN_CODE_FROM_XIP=1

    # For IAR XIP builds, we need to move some code to RAM            #IMP_CHECK
    ifeq ($(TOOLCHAIN),IAR)
        ifeq ($(OTA_BUILD_VERBOSE),1)
            $(info Makefile: ADD CY_INIT_CODECOPY_ENABLE for IAR XIP)
        endif
        DEFINES+=CY_INIT_CODECOPY_ENABLE
    endif

    # When running from external flash
    # That we need to turn off XIP and enter critical section when accessing SMIF
    #  NOTE: CYW920829 and CYW89829 do not need this. Generalize for now,
    #        we may need to change this criteria for future devices
    ifneq ($(findstring $(OTA_PLATFORM), PSOC_062_2M PSOC_062_1M PSOC_062_512K PSOC_063_1M PSOC_064_2M XMC7100 XMC7200),)
        CY_XIP_SMIF_MODE_CHANGE=1

        # Since we are running hybrid (some in RAM, some in External FLash),    #IMP_CHECK
        # we need to override the WEAK functions in CYHAL
        DEFINES+=CYHAL_DISABLE_WEAK_FUNC_IMPL=1
    endif # Not CYW20829 or CYW89829
endif # USE_XIP

# TODO: Can we base this on the flash area at RUNTIME rather than defining it at build time?
#       Flash areas / images are allowed to be placed willy-nilly
ifeq ($(FLASH_AREA_IMG_1_SECONDARY_DEV_ID),FLASH_DEVICE_INTERNAL_FLASH)
    FLASH_ERASE_SECONDARY_SLOT_VALUE= $(CY_INTERNAL_FLASH_ERASE_VALUE)
else
    FLASH_ERASE_SECONDARY_SLOT_VALUE= $(CY_EXTERNAL_FLASH_ERASE_VALUE)
endif # SECONDARY_DEV_ID

# Application MUST provide a flash map
ifneq ($(MAKECMDGOALS),getlibs)
    ifeq ($(OTA_FLASH_MAP),)
        $(info "")
        $(error Application makefile must define OTA_FLASH_MAP. For more info, see <ota-bootloader-abstraction>/configs/COMPONENT_MCUBOOT/flashmap/MCUBOOT_BUILD_COMMANDS.md)
        $(info "")
    endif
endif

###################################
# The Defines for the flash map
###################################
DEFINES+=\
    MCUBOOT_MAX_IMG_SECTORS=$(MCUBOOT_MAX_IMG_SECTORS)\
    MCUBOOT_IMAGE_NUMBER=$(MCUBOOT_IMAGE_NUMBER)\
    FLASH_AREA_BOOTLOADER_DEV_ID="$(FLASH_AREA_BOOTLOADER_DEV_ID)"\
    FLASH_AREA_BOOTLOADER_START=$(FLASH_AREA_BOOTLOADER_START)\
    FLASH_AREA_BOOTLOADER_SIZE=$(FLASH_AREA_BOOTLOADER_SIZE)\
    FLASH_AREA_IMG_1_PRIMARY_DEV_ID="$(FLASH_AREA_IMG_1_PRIMARY_DEV_ID)"\
    FLASH_AREA_IMG_1_PRIMARY_START=$(FLASH_AREA_IMG_1_PRIMARY_START) \
    FLASH_AREA_IMG_1_PRIMARY_SIZE=$(FLASH_AREA_IMG_1_PRIMARY_SIZE) \
    FLASH_AREA_IMG_1_SECONDARY_DEV_ID="$(FLASH_AREA_IMG_1_SECONDARY_DEV_ID)"\
    FLASH_AREA_IMG_1_SECONDARY_START=$(FLASH_AREA_IMG_1_SECONDARY_START) \
    FLASH_AREA_IMG_1_SECONDARY_SIZE=$(FLASH_AREA_IMG_1_SECONDARY_SIZE)

ifneq ($(FLASH_AREA_IMAGE_SWAP_STATUS_DEV_ID),)
    DEFINES+=\
        FLASH_AREA_IMAGE_SWAP_STATUS_DEV_ID="$(FLASH_AREA_IMAGE_SWAP_STATUS_DEV_ID)"\
        FLASH_AREA_IMAGE_SWAP_STATUS_START=$(FLASH_AREA_IMAGE_SWAP_STATUS_START)\
        FLASH_AREA_IMAGE_SWAP_STATUS_SIZE=$(FLASH_AREA_IMAGE_SWAP_STATUS_SIZE)
endif

ifneq ($(FLASH_AREA_IMAGE_SCRATCH_DEV_ID),)
    DEFINES+=\
        FLASH_AREA_IMAGE_SCRATCH_DEV_ID="$(FLASH_AREA_IMAGE_SCRATCH_DEV_ID)"\
        FLASH_AREA_IMAGE_SCRATCH_START=$(FLASH_AREA_IMAGE_SCRATCH_START)\
        FLASH_AREA_IMAGE_SCRATCH_SIZE=$(FLASH_AREA_IMAGE_SCRATCH_SIZE)
endif


ifeq ($(MCUBOOT_IMAGE_NUMBER),2)
    DEFINES+=\
        FLASH_AREA_IMG_2_PRIMARY_DEV_ID=$(FLASH_AREA_IMG_2_PRIMARY_DEV_ID)\
        FLASH_AREA_IMG_2_PRIMARY_START=$(FLASH_AREA_IMG_2_PRIMARY_START) \
        FLASH_AREA_IMG_2_PRIMARY_SIZE=$(FLASH_AREA_IMG_2_PRIMARY_SIZE) \
        FLASH_AREA_IMG_2_SECONDARY_DEV_ID=$(FLASH_AREA_IMG_2_SECONDARY_DEV_ID) \
        FLASH_AREA_IMG_2_SECONDARY_START=$(FLASH_AREA_IMG_2_SECONDARY_START) \
        FLASH_AREA_IMG_2_SECONDARY_SIZE=$(FLASH_AREA_IMG_2_SECONDARY_SIZE)
endif

ifeq ($(USE_EXTERNAL_FLASH),1)
    DEFINES+=OTA_USE_EXTERNAL_FLASH=1
endif

ifeq ($(CY_RUN_CODE_FROM_XIP),1)
    DEFINES+=CY_RUN_CODE_FROM_XIP=1
endif

ifeq ($(CY_XIP_SMIF_MODE_CHANGE),1)
    DEFINES+=CY_XIP_SMIF_MODE_CHANGE=1
endif

##################################
# Additional / custom linker flags.
##################################

#IMP_CHECK : Usage of -Wl LDFlag.

ifeq ($(TOOLCHAIN),GCC_ARM)
    CY_ELF_TO_HEX=$(MTB_TOOLCHAIN_GCC_ARM__OBJCOPY)
    CY_ELF_TO_HEX_OPTIONS="-O ihex"
    CY_ELF_TO_HEX_FILE_ORDER="elf_first"
    LDFLAGS+="-Wl,--defsym,MCUBOOT_HEADER_SIZE=$(MCUBOOT_HEADER_SIZE),--defsym,FLASH_AREA_IMG_1_PRIMARY_START=$(FLASH_AREA_IMG_1_PRIMARY_START),--defsym,FLASH_AREA_IMG_1_PRIMARY_SIZE=$(FLASH_AREA_IMG_1_PRIMARY_SIZE)"
else
    ifeq ($(TOOLCHAIN),IAR)
        CY_ELF_TO_HEX=$(MTB_TOOLCHAIN_IAR__BASE_DIR)/bin/ielftool
        CY_ELF_TO_HEX_OPTIONS="--ihex"
        CY_ELF_TO_HEX_FILE_ORDER="elf_first"
        LDFLAGS+=--config_def MCUBOOT_HEADER_SIZE=$(MCUBOOT_HEADER_SIZE) --config_def FLASH_AREA_IMG_1_PRIMARY_START=$(FLASH_AREA_IMG_1_PRIMARY_START) --config_def FLASH_AREA_IMG_1_PRIMARY_SIZE=$(FLASH_AREA_IMG_1_PRIMARY_SIZE)
        ifeq ($(OTA_PLATFORM),$(filter $(OTA_PLATFORM),XMC7100 XMC7200))
            CY_OTA_APP_HEAP_SIZE?=0x40000
            LDFLAGS+=--config_def __HEAP_SIZE=$(CY_OTA_APP_HEAP_SIZE)
        endif
    else
        ifeq ($(TOOLCHAIN),ARM)
            ifeq ($(OS),Windows_NT)
                CY_ELF_TO_HEX=$(MTB_TOOLCHAIN_ARM__BASE_DIR)/bin/fromelf.exe
            else
                CY_ELF_TO_HEX=$(MTB_TOOLCHAIN_ARM__BASE_DIR)/bin/fromelf
            endif
            CY_ELF_TO_HEX_OPTIONS="--i32combined --output"
            CY_ELF_TO_HEX_FILE_ORDER="hex_first"
            LDFLAGS+=--pd=-DMCUBOOT_HEADER_SIZE=$(MCUBOOT_HEADER_SIZE) --pd=-DFLASH_AREA_IMG_1_PRIMARY_START=$(FLASH_AREA_IMG_1_PRIMARY_START) --pd=-DFLASH_AREA_IMG_1_PRIMARY_SIZE=$(FLASH_AREA_IMG_1_PRIMARY_SIZE)
            LDFLAGS+=--diag_suppress=L6314W
        else
            $(error Must define toolchain ! GCC_ARM, ARM, or IAR)
        endif #ARM
    endif #IAR
endif #GCC_ARM

# Find Linker Script using wildcard
# Directory within ota-upgrade library

ifeq ($(OTA_SUPPORT),1)
    ifeq ($(OTA_LINKER_FILE), )
        $(info ======================================================================= )
        $(error Provide OTA linker file for $(TARGET))
    else
        LINKER_SCRIPT=$(OTA_LINKER_FILE)
    endif
endif

###################################################################################################
#
# Build information
#
###################################################################################################

ifneq ($(CY_SECONDSTAGE),)
    ifeq ($(OTA_BUILD_VERBOSE),1)
        $(info Makefile:)
        $(info Makefile: TARGET                         = $(TARGET))
        $(info Makefile: OTA_PLATFORM                   = $(OTA_PLATFORM))
        $(info Makefile: OTA_FLASH_MAP                  = $(OTA_FLASH_MAP))
        $(info Makefile: APP_VERSION_MAJOR              = $(APP_VERSION_MAJOR))
        $(info Makefile: APP_VERSION_MINOR              = $(APP_VERSION_MINOR))
        $(info Makefile: APP_VERSION_BUILD              = $(APP_VERSION_BUILD))
        $(info Makefile: CY_TEST_APP_VERSION_IN_TAR     = $(CY_TEST_APP_VERSION_IN_TAR))
        $(info Makefile: USE_XIP                        = $(USE_XIP))
        $(info Makefile: CY_RUN_CODE_FROM_XIP           = $(CY_RUN_CODE_FROM_XIP))
        $(info Makefile: CY_XIP_SMIF_MODE_CHANGE        = $(CY_XIP_SMIF_MODE_CHANGE))
        $(info Makefile: USE_EXTERNAL_FLASH             = $(USE_EXTERNAL_FLASH))
    endif # OTA_BUILD_VERBOSE=1

    ifeq ($(OTA_BUILD_FLASH_VERBOSE),1)
        $(info MCUBOOT_MAX_IMG_SECTORS      = $(MCUBOOT_MAX_IMG_SECTORS))
        $(info FLASH_AREA_BOOTLOADER_DEV_ID = $(FLASH_AREA_BOOTLOADER_DEV_ID))
        $(info FLASH_AREA_BOOTLOADER_START  = $(FLASH_AREA_BOOTLOADER_START))
        $(info FLASH_AREA_BOOTLOADER_SIZE   = $(FLASH_AREA_BOOTLOADER_SIZE))
        $(info MCUBOOT_IMAGE_NUMBER         = $(MCUBOOT_IMAGE_NUMBER))
        $(info FLASH_AREA_IMG_1_PRIMARY_DEV_ID   = $(FLASH_AREA_IMG_1_PRIMARY_DEV_ID))
        $(info FLASH_AREA_IMG_1_PRIMARY_START    = $(FLASH_AREA_IMG_1_PRIMARY_START))
        $(info FLASH_AREA_IMG_1_PRIMARY_SIZE     = $(FLASH_AREA_IMG_1_PRIMARY_SIZE))
        $(info FLASH_AREA_IMG_1_SECONDARY_DEV_ID = $(FLASH_AREA_IMG_1_SECONDARY_DEV_ID))
        $(info FLASH_AREA_IMG_1_SECONDARY_START  = $(FLASH_AREA_IMG_1_SECONDARY_START))
        $(info FLASH_AREA_IMG_1_SECONDARY_SIZE   = $(FLASH_AREA_IMG_1_SECONDARY_SIZE))
        $(info FLASH_AREA_IMAGE_SCRATCH_DEV_ID = $(FLASH_AREA_IMAGE_SCRATCH_DEV_ID))
        $(info FLASH_AREA_IMAGE_SCRATCH_START  = $(FLASH_AREA_IMAGE_SCRATCH_START))
        $(info FLASH_AREA_IMAGE_SCRATCH_SIZE   = $(FLASH_AREA_IMAGE_SCRATCH_SIZE))
        ifeq ($(MCUBOOT_IMAGE_NUMBER),2)
            $(info FLASH_AREA_IMG_2_PRIMARY_DEV_ID   = $(FLASH_AREA_IMG_2_PRIMARY_DEV_ID))
            $(info FLASH_AREA_IMG_2_PRIMARY_START    = $(FLASH_AREA_IMG_2_PRIMARY_START))
            $(info FLASH_AREA_IMG_2_PRIMARY_SIZE     = $(FLASH_AREA_IMG_2_PRIMARY_SIZE))
            $(info FLASH_AREA_IMG_2_SECONDARY_DEV_ID = $(FLASH_AREA_IMG_2_SECONDARY_DEV_ID))
            $(info FLASH_AREA_IMG_2_SECONDARY_START  = $(FLASH_AREA_IMG_2_SECONDARY_START))
            $(info FLASH_AREA_IMG_2_SECONDARY_SIZE   = $(FLASH_AREA_IMG_2_SECONDARY_SIZE))
        endif
        $(info FLASH_AREA_IMAGE_SWAP_STATUS_DEV_ID = $(FLASH_AREA_IMAGE_SWAP_STATUS_DEV_ID))
        $(info FLASH_AREA_IMAGE_SWAP_STATUS_START  = $(FLASH_AREA_IMAGE_SWAP_STATUS_START))
        $(info FLASH_AREA_IMAGE_SWAP_STATUS_SIZE   = $(FLASH_AREA_IMAGE_SWAP_STATUS_SIZE))
    endif # OTA_BUILD_FLASH_VERBOSE

    ifeq ($(OTA_BUILD_COMPONENTS_VERBOSE),1)
        $(info COMPONENTS)
        $(info COMPONENTS=$(COMPONENTS))
    endif

    ifeq ($(OTA_BUILD_DEFINES_VERBOSE),1)
        $(info DEFINES)
        $(info DEFINES=$(DEFINES))
    endif

    ifeq ($(OTA_BUILD_IGNORE_VERBOSE),1)
        $(info CY_IGNORE)
        $(info CY_IGNORE=$(CY_IGNORE))
    endif # OTA_BUILD_VERBOSE = COMPONENTS
endif    # SECOND STAGE of build


###################################################################################################
# OTA POST BUILD scripting
###################################################################################################

######################################
# Build Location / Output directory
######################################

# output directory for use in the sign_script.bash
ifneq ($(CY_SECONDSTAGE),)
    OUTPUT_FILE_PATH=$(CY_BUILD_LOCATION)/$(TARGET)/$(CONFIG)
    $(info CY_BUILD_LOCATION     = $(CY_BUILD_LOCATION))
    $(info TARGET                = $(TARGET))
    $(info CONFIG                = $(CONFIG))
    $(info OUTPUT_FILE_PATH      = $(OUTPUT_FILE_PATH))
    ifeq ($(CY_COMPILER_GCC_ARM_DIR),)
        CY_COMPILER_GCC_ARM_DIR=$(CY_TOOLS_DIR)/gcc
    endif

    # POSTBUILD script is NOT provided by the Application. Use default OTA POSTBUILD scripts.
    ifeq ($(OTA_APP_POSTBUILD),)
        $(info Application Makefile has not defined OTA_APP_POSTBUILD. Using OTA library's default POSTBUILD commands.)

        CY_HEX_TO_BIN="$(MTB_TOOLCHAIN_GCC_ARM__OBJCOPY)"

        # MCUBoot flash support location
        MCUBOOT_DIR=$(RELATIVE_FILE1_FILE2)/../../scripts/mcuboot
        IMG_SIGNING_SCRIPT_TOOL_PATH?=$(MCUBOOT_DIR)/imgtool/imgtool.py
        CY_SIGN_KEY_PATH?=$(MCUBOOT_DIR)/keys/cypress-test-ec-p256.pem

        IMGTOOL_SCRIPT_NAME:= $(shell echo $(IMG_SIGNING_SCRIPT_TOOL_PATH)|rev|cut -d "/" -f1 |rev )
        MCUBOOT_SCRIPT_FILE_DIR := $(shell echo "$(IMG_SIGNING_SCRIPT_TOOL_PATH)" | sed "s@/$(IMGTOOL_SCRIPT_NAME)@@")
        MCUBOOT_KEY_FILE := $(shell echo $(CY_SIGN_KEY_PATH)|rev|cut -d "/" -f1 |rev )
        MCUBOOT_KEY_DIR := $(shell echo "$(CY_SIGN_KEY_PATH)" | sed "s@/$(MCUBOOT_KEY_FILE)@@")

        #--------------------------------------
        # CYW20829 and CYW89829 POSTBUILD
        #--------------------------------------
        ifneq ($(filter $(OTA_PLATFORM),CYW20829 CYW89829),)
            CY_DEVICE_LCS ?= NORMAL_NO_SECURE
            CY_ENC_IMG ?= 0
            CY_SMIF_ENC ?= 0
            FLASH_BASE_ADDRESS ?= 0x60000000
            FLASH_BASE_CBUS_ADDRESS ?= 0x08000000
            CY_SERVICE_APP_DESCR_ADDR ?= 0x0
            CY_SIGN_TYPE ?= mcuboot_user_app
            CY_IMG_ID ?= 1
            CY_SIGN_IMG_ID ?= $(shell expr $(CY_IMG_ID) - 1)
            CY_IMG_CONFIRM ?=0
            CY_USE_OVERWRITE ?=0
            CY_SLOT_SIZE ?= $(FLASH_AREA_IMG_1_PRIMARY_SIZE)
            APPTYPE ?= flash
            OTA_BUILD_POST_VERBOSE ?=0

            ifeq ($(CY_DEVICE_LCS), NORMAL_NO_SECURE)
                ifeq ($(OTA_APP_POLICY_PATH),)
                    $(info Application makefile has not defined OTA_APP_POLICY_PATH. Using OTA library's default Policy file for 20829 & 89829 device.)
                    OTA_APP_POLICY_PATH?=$(RELATIVE_FILE1_FILE2)/../../scripts/mcuboot/policy/policy_CM33_no_secure.json
                endif
                $(info OTA_APP_POLICY_PATH   = $(OTA_APP_POLICY_PATH) )
            else
                ifeq ($(OTA_APP_POLICY_PATH),)
                    $(info Application makefile has not defined OTA_APP_POLICY_PATH. Using OTA library's default Policy file for 20829 & 89829 device.)
                    OTA_APP_POLICY_PATH?=$(RELATIVE_FILE1_FILE2)/../../scripts/mcuboot/policy/policy_CM33_secure.json
                endif
                $(info OTA_APP_POLICY_PATH   = $(OTA_APP_POLICY_PATH) )
            endif

            ifeq ($(CY_ENC_IMG), 1)
                CY_IMG_PAD ?= 1
                DEFINES+=CY_ENC_IMG=1
                CY_OTA_APP_ADDRESS ?= $(shell expr $(shell printf "%d" $(FLASH_BASE_CBUS_ADDRESS)) + $(shell printf "%d" $(FLASH_AREA_IMG_1_PRIMARY_START)))
                ifeq ($(CY_SMIF_ENC), 1)
                    CY_OTA_APP_SECONDARY_ADDRESS ?= $(shell expr $(shell printf "%d" $(FLASH_BASE_CBUS_ADDRESS)) + $(shell printf "%d" $(FLASH_AREA_IMG_1_SECONDARY_START)))
                else
                    CY_OTA_APP_SECONDARY_ADDRESS ?= 0
                endif

                ifeq ($(CY_OEM_PRIVATE_KEY),)
                    ifeq ($(CY_DEVICE_LCS), SECURE)
                        $(error Application makefile must define CY_OEM_PRIVATE_KEY <path to the provisioned OEM key/OEM key>. For more info, see <ota-bootloader-abstraction>/configs/COMPONENT_MCUBOOT/flashmap/MCUBOOT_BUILD_COMMANDS.md)
                    else
                        CY_OEM_PRIVATE_KEY?=$(RELATIVE_FILE1_FILE2)/../../scripts/mcuboot/keys/priv_oem_0.pem
                        $(info Using default OEM Key available in <ota-bootloader-abstraction>/scripts/mcuboot/keys/priv_oem_0.pem)
                        $(info CY_OEM_PRIVATE_KEY    = $(CY_OEM_PRIVATE_KEY))
                    endif
                else
                    $(info CY_OEM_PRIVATE_KEY    = $(CY_OEM_PRIVATE_KEY))
                endif

                ifeq ($(CY_ENC_KEY),)
                    ifeq ($(CY_DEVICE_LCS), SECURE)
                        ifeq ($(CY_SMIF_ENC), 1)
                            $(error Application makefile must define CY_ENC_KEY <path to the provisioned Encryption key/Encryption key> for Encrypting image. For more info, see <ota-bootloader-abstraction>/configs/COMPONENT_MCUBOOT/flashmap/MCUBOOT_BUILD_COMMANDS.md)
                        else
                            CY_ENC_KEY=$(RELATIVE_FILE1_FILE2)/../../scripts/mcuboot/keys/enc-ec256-pub.pem
                            $(info Using default Encryption Key available in <ota-bootloader-abstraction>/scripts/mcuboot/keys/enc-ec256-pub.pem)
                            $(info CY_ENC_KEY            = $(CY_ENC_KEY))
                        endif
                    else
                        CY_ENC_KEY=$(RELATIVE_FILE1_FILE2)/../../scripts/mcuboot/keys/enc-ec256-pub.pem
                        $(info Using default Encryption Key available in <ota-bootloader-abstraction>/scripts/mcuboot/keys/enc-ec256-pub.pem)
                        $(info CY_ENC_KEY             = $(CY_ENC_KEY))
                    endif
                else
                    $(info CY_ENC_KEY            = $(CY_ENC_KEY))
                endif

                ifeq ($(CY_DEVICE_LCS), NORMAL_NO_SECURE)
                    MCUBOOT_SCRIPT_FILE_PATH?=$(RELATIVE_FILE1_FILE2)/../../scripts/mcuboot/sign_script_20829_ns.bash
                else
                    ifeq ($(CY_SMIF_ENC), 1)
                        ifeq ($(CY_NONCE),)
                            $(error Application makefile must define CY_NONCE for Encrypting image. For more info, see <ota-bootloader-abstraction>/configs/COMPONENT_MCUBOOT/flashmap/MCUBOOT_BUILD_COMMANDS.md)
                        else
                            $(info CY_NONCE              = $(CY_NONCE))
                        endif
                    else
                        CY_NONCE ?= "NA"
                    endif
                    MCUBOOT_SCRIPT_FILE_PATH?=$(RELATIVE_FILE1_FILE2)/../../scripts/mcuboot/sign_script_20829_s_enc.bash
                endif
            else
                CY_IMG_PAD ?=0
                ifeq ($(CY_DEVICE_LCS), SECURE)
                    $(error Current version of <ota-bootloader-abstraction> doesnot support default post build scripts for Secure LCS without Encryption... )
                else
                    CY_OTA_APP_ADDRESS ?= 0
                    CY_OEM_PRIVATE_KEY?=$(RELATIVE_FILE1_FILE2)/../../scripts/mcuboot/keys/priv_oem_0.pem
                    CY_ENC_KEY?="NA"
                    MCUBOOT_SCRIPT_FILE_PATH?=$(RELATIVE_FILE1_FILE2)/../../scripts/mcuboot/sign_script_20829_ns.bash
                endif
            endif

            CY_ELF_TO_HEX=$(CY_HEX_TO_BIN)

            CY_TOC2_GENERATOR = $(SEARCH_recipe-make-cat1b)/make/scripts/20829/run_toc2_generator.sh

            _MTB_RECIPE_20829_SREC_CAT_UTIL=$(CY_TOOL_srec_cat_EXE_ABS)

            ifeq ($(TOOLCHAIN),ARM)
                POSTBUILD=$(MTB_TOOLCHAIN_ARM__BASE_DIR)/bin/fromelf $(MTB_TOOLS__OUTPUT_CONFIG_DIR)/$(APPNAME).elf --bin --output=$(MTB_TOOLS__OUTPUT_CONFIG_DIR)/$(APPNAME).bin;
                POSTBUILD+=$(MTB_TOOLS__BASH_CMD) $(MTB_TOOLS__RECIPE_DIR)/make/scripts/20829/flash_postbuild.sh "$(TOOLCHAIN)" "$(MTB_TOOLS__OUTPUT_CONFIG_DIR)" "$(APPNAME)" "$(MTB_TOOLCHAIN_GCC_ARM__BASE_DIR)/bin" "$(_MTB_RECIPE_20829_SREC_CAT_UTIL)" "$(BOOTSTRAP_SIZE)";
            else
                ifeq ($(TOOLCHAIN),IAR)
                    ifeq ($(APPTYPE),flash)
                        OTA_POSTBUILD_PARAM=--bin-multi
                    else
                        OTA_POSTBUILD_PARAM=--bin
                    endif
                    POSTBUILD=$(MTB_TOOLCHAIN_IAR__BASE_DIR)/bin/ielftool $(MTB_TOOLS__OUTPUT_CONFIG_DIR)/$(APPNAME).elf $(OTA_POSTBUILD_PARAM) $(MTB_TOOLS__OUTPUT_CONFIG_DIR)/$(APPNAME).bin;
                    POSTBUILD+=$(MTB_TOOLCHAIN_IAR__BASE_DIR)/bin/ielfdumparm  -a $(MTB_TOOLS__OUTPUT_CONFIG_DIR)/$(APPNAME).elf > $(MTB_TOOLS__OUTPUT_CONFIG_DIR)/$(APPNAME).dis;
                    POSTBUILD+=$(MTB_TOOLS__BASH_CMD) $(MTB_TOOLS__RECIPE_DIR)/make/scripts/20829/flash_postbuild.sh "$(TOOLCHAIN)" "$(MTB_TOOLS__OUTPUT_CONFIG_DIR)" "$(APPNAME)" "$(MTB_TOOLCHAIN_GCC_ARM__BASE_DIR)/bin" "$(_MTB_RECIPE_20829_SREC_CAT_UTIL)" "$(BOOTSTRAP_SIZE)";
                else
                    ifeq ($(TOOLCHAIN),GCC_ARM)
                        POSTBUILD="$(MTB_TOOLCHAIN_GCC_ARM__OBJCOPY)" "$(MTB_TOOLS__OUTPUT_CONFIG_DIR)/$(APPNAME).elf" -S -O binary "$(MTB_TOOLS__OUTPUT_CONFIG_DIR)/$(APPNAME).bin";
                    endif # GCC_ARM
                endif # IAR
            endif # ARM

            CY_ADD_ARGS+= --erased-val $(CY_EXTERNAL_FLASH_ERASE_VALUE)

            ifeq ($(CY_IMG_PAD), 1)
            CY_ADD_ARGS+= --pad
            endif

            ifeq ($(CY_IMG_CONFIRM), 1)
            CY_ADD_ARGS+= --confirm
            endif

            ifeq ($(CY_USE_OVERWRITE), 1)
            CY_ADD_ARGS+= --overwrite-only
            DEFINES_APP += -DMCUBOOT_OVERWRITE_ONLY
            endif

            CY_ADD_ARG="$(CY_ADD_ARGS)"

            # If POSTBUILD fails, Turn this on to check that all args are present
            ifneq ($(CY_SECONDSTAGE),)
                ifeq ($(OTA_BUILD_POST_VERBOSE),1)
                    # Check that all arguments for the POSTBUILD functionality are present
                    $(info ======================================================================= )
                    $(info CURRENT_DIRECTORY                = $(CURRENT_DIRECTORY))
                    $(info MCUBOOT_SCRIPT_FILE_PATH         = $(MCUBOOT_SCRIPT_FILE_PATH))
                    $(info APPNAME                          = $(APPNAME))
                    $(info APPTYPE                          = $(APPTYPE))
                    $(info CY_ELF_TO_HEX                    = $(CY_ELF_TO_HEX))
                    $(info CY_ELF_TO_HEX_OPTIONS            = $(CY_ELF_TO_HEX_OPTIONS))
                    $(info CY_ELF_TO_HEX_FILE_ORDER         = $(CY_ELF_TO_HEX_FILE_ORDER))
                    $(info CY_HEX_TO_BIN                    = $(CY_HEX_TO_BIN))
                    $(info FLASH_ERASE_SECONDARY_SLOT_VALUE = $(FLASH_ERASE_SECONDARY_SLOT_VALUE))
                    $(info MCUBOOT_HEADER_SIZE              = $(MCUBOOT_HEADER_SIZE))
                    $(info APP_BUILD_VERSION                = $(APP_BUILD_VERSION))
                    $(info FLASH_BASE_ADDRESS               = $(FLASH_BASE_ADDRESS))
                    $(info FLASH_BASE_CBUS_ADDRESS          = $(FLASH_BASE_CBUS_ADDRESS))
                    $(info FLASH_AREA_IMG_1_PRIMARY_START   = $(FLASH_AREA_IMG_1_PRIMARY_START))
                    $(info MCUBOOT_KEY_DIR                  = $(MCUBOOT_KEY_DIR))
                    $(info MCUBOOT_KEY_FILE                 = $(MCUBOOT_KEY_FILE))
                    $(info CY_TOC2_GENERATOR                = $(CY_TOC2_GENERATOR))
                    $(info CY_DEVICE_LCS                    = $(CY_DEVICE_LCS))
                    $(info CY_SIGN_TYPE                     = $(CY_SIGN_TYPE))
                    $(info DEVICE_(MPN_LIST)_SRAM_KB        = $(DEVICE_$(MPN_LIST)_SRAM_KB))
                    $(info MTB_TOOLS__OUTPUT_CONFIG_DIR     = $(MTB_TOOLS__OUTPUT_CONFIG_DIR))
                    $(info SMIF_CRYPTO_CONFIG               = $(SMIF_CRYPTO_CONFIG))
                    $(info MTB_TOOLCHAIN_GCC_ARM__BASE_DIR  = $(MTB_TOOLCHAIN_GCC_ARM__BASE_DIR))
                    $(info OTA_APP_POLICY_PATH              = $(OTA_APP_POLICY_PATH))
                    $(info CY_SLOT_SIZE                     = $(CY_SLOT_SIZE))
                    $(info CY_SERVICE_APP_DESCR_ADDR        = $(CY_SERVICE_APP_DESCR_ADDR))
                    $(info BOOTSTRAP_SIZE                   = $(BOOTSTRAP_SIZE))
                    $(info CY_IMG_PAD                       = $(CY_IMG_PAD))
                    $(info CY_IMG_CONFIRM                   = $(CY_IMG_CONFIRM))
                    $(info CY_USE_OVERWRITE                 = $(CY_USE_OVERWRITE))
                    $(info CY_SIGN_IMG_ID                   = $(CY_SIGN_IMG_ID))
                    $(info CY_ENC_IMG                       = $(CY_ENC_IMG))
                    ifeq ($(CY_ENC_IMG),1)
                        $(info CY_ENC_KEY                       = $(CY_ENC_KEY))
                        $(info CY_OTA_APP_ADDRESS               = $(CY_OTA_APP_ADDRESS))
                        $(info CY_OEM_PRIVATE_KEY               = $(CY_OEM_PRIVATE_KEY))
                        $(info CY_SMIF_ENC                      = $(CY_SMIF_ENC))
                        ifeq ($(CY_SMIF_ENC),1)
                            $(info CY_OTA_APP_SECONDARY_ADDRESS     = $(CY_OTA_APP_SECONDARY_ADDRESS))
                            $(info CY_NONCE                         = $(CY_NONCE))
                        endif
                    endif
                    $(info CY_ADD_ARG                       = $(CY_ADD_ARG))
                endif # OTA_BUILD_VERBOSE = POST
            endif # SECOND STAGE

            ifeq ($(CY_DEVICE_LCS), SECURE)
                # CYW920829/CYW89829 POSTBUILD call for Secure LCS and Encryption
                POSTBUILD+=$(MCUBOOT_SCRIPT_FILE_PATH) $(MTB_TOOLCHAIN_GCC_ARM__BASE_DIR) \
                $(CY_PYTHON_PATH) $(MTB_TOOLS__OUTPUT_CONFIG_DIR) $(APPNAME) \
                $(CY_ELF_TO_HEX) $(CY_ELF_TO_HEX_OPTIONS) $(CY_ELF_TO_HEX_FILE_ORDER) $(CY_HEX_TO_BIN) \
                $(FLASH_ERASE_SECONDARY_SLOT_VALUE) $(MCUBOOT_HEADER_SIZE) $(APP_BUILD_VERSION) \
                $(OTA_PLATFORM) $(FLASH_BASE_ADDRESS) $(FLASH_BASE_CBUS_ADDRESS) \
                $(FLASH_AREA_IMG_1_PRIMARY_START) $(FLASH_AREA_IMG_1_PRIMARY_SIZE) $(FLASH_AREA_IMG_1_SECONDARY_START) \
                $(MCUBOOT_KEY_DIR) $(MCUBOOT_KEY_FILE) \
                $(CY_TOC2_GENERATOR) $(CY_DEVICE_LCS) $(CY_SIGN_TYPE) \
                $(APPNAME) $(APPTYPE) $(CURRENT_DIRECTORY) \
                $(SMIF_CRYPTO_CONFIG) \
                $(OTA_APP_POLICY_PATH) \
                $(CY_SERVICE_APP_DESCR_ADDR) $(BOOTSTRAP_SIZE) $(DEVICE_$(MPN_LIST)_SRAM_KB) \
                $(CY_SIGN_IMG_ID) $(OTA_BUILD_POST_VERBOSE) \
                $(CY_ENC_IMG) $(CY_ENC_KEY) $(CY_OEM_PRIVATE_KEY) $(CY_OTA_APP_ADDRESS) \
                $(CY_OTA_APP_SECONDARY_ADDRESS) $(CY_SMIF_ENC) $(CY_NONCE) \
                $(CY_ADD_ARG);
            else
                POSTBUILD+=$(MCUBOOT_SCRIPT_FILE_PATH) $(MTB_TOOLCHAIN_GCC_ARM__BASE_DIR) \
                $(CY_PYTHON_PATH) $(MTB_TOOLS__OUTPUT_CONFIG_DIR) $(APPNAME) \
                $(CY_ELF_TO_HEX) $(CY_ELF_TO_HEX_OPTIONS) $(CY_ELF_TO_HEX_FILE_ORDER) $(CY_HEX_TO_BIN) \
                $(FLASH_ERASE_SECONDARY_SLOT_VALUE) $(MCUBOOT_HEADER_SIZE) $(APP_BUILD_VERSION) \
                $(OTA_PLATFORM) $(FLASH_BASE_ADDRESS) $(FLASH_BASE_CBUS_ADDRESS) \
                $(FLASH_AREA_IMG_1_PRIMARY_START) $(FLASH_AREA_IMG_1_PRIMARY_SIZE) $(FLASH_AREA_IMG_1_SECONDARY_START) \
                $(MCUBOOT_KEY_DIR) $(MCUBOOT_KEY_FILE) \
                $(CY_TOC2_GENERATOR) $(CY_DEVICE_LCS) $(CY_SIGN_TYPE) \
                $(APPNAME) $(APPTYPE) $(CURRENT_DIRECTORY) \
                $(SMIF_CRYPTO_CONFIG) \
                $(OTA_APP_POLICY_PATH) \
                $(CY_SERVICE_APP_DESCR_ADDR) $(BOOTSTRAP_SIZE) $(DEVICE_$(MPN_LIST)_SRAM_KB) \
                $(CY_SIGN_IMG_ID) $(OTA_BUILD_POST_VERBOSE) \
                $(CY_ENC_IMG) $(CY_ENC_KEY) $(CY_OEM_PRIVATE_KEY) $(CY_OTA_APP_ADDRESS) \
                $(CY_ADD_ARG);
            endif
        endif# end (CYW920829/CYW89829) section

        #--------------------------------------
        # XMC7000 POSTBUILD
        #--------------------------------------
        ifeq ($(OTA_PLATFORM),$(filter $(OTA_PLATFORM),XMC7100 XMC7200))
            $(info Makefile: XMC7000 POSTBUILD )

            MCUBOOT_XMC_SCRIPT_FILE_PATH?=$(RELATIVE_FILE1_FILE2)/../../scripts/mcuboot/sign_script_xmc7000.bash
            CY_ELF_TO_HEX=$(CY_HEX_TO_BIN)

            ifeq ($(TOOLCHAIN),ARM)
                POSTBUILD=$(MTB_TOOLCHAIN_ARM__BASE_DIR)/bin/fromelf $(MTB_TOOLS__OUTPUT_CONFIG_DIR)/$(APPNAME).elf --bin --output=$(MTB_TOOLS__OUTPUT_CONFIG_DIR)/$(APPNAME).unsigned.bin;
            else
                ifeq ($(TOOLCHAIN),IAR)
                    ifeq ($(APPTYPE),flash)
                        OTA_POSTBUILD_PARAM=--bin-multi
                    else
                        OTA_POSTBUILD_PARAM=--bin
                    endif
                    POSTBUILD=$(MTB_TOOLCHAIN_IAR__BASE_DIR)/bin/ielftool $(MTB_TOOLS__OUTPUT_CONFIG_DIR)/$(APPNAME).elf $(OTA_POSTBUILD_PARAM) $(MTB_TOOLS__OUTPUT_CONFIG_DIR)/$(APPNAME).unsigned.bin;
                    POSTBUILD+=$(MTB_TOOLCHAIN_IAR__BASE_DIR)/bin/ielfdumparm  -a $(MTB_TOOLS__OUTPUT_CONFIG_DIR)/$(APPNAME).elf > $(MTB_TOOLS__OUTPUT_CONFIG_DIR)/$(APPNAME).dis;
                else
                    ifeq ($(TOOLCHAIN),GCC_ARM)
                        POSTBUILD=$(MTB_TOOLCHAIN_GCC_ARM__OBJCOPY) $(MTB_TOOLS__OUTPUT_CONFIG_DIR)/$(APPNAME).elf -S -O binary $(MTB_TOOLS__OUTPUT_CONFIG_DIR)/$(APPNAME).unsigned.bin;
                        $(info Makefile: after GCC POSTBUILD )
                    endif # GCC_ARM
                endif # IAR
            endif # ARM

            CY_ENC_IMG ?= 0
            FLASH_BASE_ADDRESS ?= 0x10000000
            CY_SERVICE_APP_DESCR_ADDR = 0x0
            CY_LCS? = NORMAL_NO_SECURE
            ERASED_VALUE = 0xFF

            ifeq ($(USE_OVERWRITE), 1)
                UPGRADE_TYPE := --overwrite-only
                DEFINES_APP += -DMCUBOOT_OVERWRITE_ONLY
            endif

            # If POSTBUILD fails, Turn this on to check that all args are present
            ifneq ($(CY_SECONDSTAGE),)
                ifeq ($(OTA_BUILD_POST_VERBOSE),1)
                    # Check that all arguments for the POSTBUILD functionality are present
                    $(info MCUBOOT_XMC_SCRIPT_FILE_PATH     =$(MCUBOOT_XMC_SCRIPT_FILE_PATH))
                    $(info APPNAME                          =$(APPNAME))
                    $(info CY_ELF_TO_HEX                    =$(CY_ELF_TO_HEX))
                    $(info CY_ELF_TO_HEX_OPTIONS            =$(CY_ELF_TO_HEX_OPTIONS))
                    $(info CY_ELF_TO_HEX_FILE_ORDER         =$(CY_ELF_TO_HEX_FILE_ORDER))
                    $(info CY_HEX_TO_BIN                    =$(CY_HEX_TO_BIN))
                    $(info FLASH_ERASE_SECONDARY_SLOT_VALUE =$(FLASH_ERASE_SECONDARY_SLOT_VALUE))
                    $(info MCUBOOT_HEADER_SIZE              =$(MCUBOOT_HEADER_SIZE))
                    $(info APP_BUILD_VERSION                =$(APP_BUILD_VERSION))
                    $(info FLASH_BASE_ADDRESS               =$(FLASH_BASE_ADDRESS))
                    $(info FLASH_AREA_IMG_1_PRIMARY_START   =$(FLASH_AREA_IMG_1_PRIMARY_START))
                    $(info MCUBOOT_KEY_DIR                  =$(MCUBOOT_KEY_DIR))
                    $(info MCUBOOT_KEY_FILE                 =$(MCUBOOT_KEY_FILE))
                    $(info CY_TOC2_GENERATOR                =$(CY_TOC2_GENERATOR))
                    $(info CY_LCS                           =$(CY_LCS))
                    $(info DEVICE_(MPN_LIST)_SRAM_KB        =$(DEVICE_$(MPN_LIST)_SRAM_KB))
                    $(info MTB_TOOLS__OUTPUT_CONFIG_DIR     =$(MTB_TOOLS__OUTPUT_CONFIG_DIR))
                    $(info APPTYPE                          =$(APPTYPE))
                    $(info CURRENT_DIRECTORY                =$(CURRENT_DIRECTORY))
                    $(info SMIF_CRYPTO_CONFIG               =$(SMIF_CRYPTO_CONFIG))
                    $(info MTB_TOOLCHAIN_GCC_ARM__BASE_DIR  =$(MTB_TOOLCHAIN_GCC_ARM__BASE_DIR))
                    $(info OTA_APP_POLICY_PATH              =$(OTA_APP_POLICY_PATH))
                    $(info CY_SLOT_SIZE                     =$(CY_SLOT_SIZE))
                    $(info CY_SERVICE_APP_DESCR_ADDR        =$(CY_SERVICE_APP_DESCR_ADDR))
                    $(info BOOTSTRAP_SIZE                   =$(BOOTSTRAP_SIZE))
                endif # OTA_BUILD_VERBOSE = POST
            endif # SECOND STAGE

            # XMC7000 POSTBUILD call
            POSTBUILD+=$(MCUBOOT_XMC_SCRIPT_FILE_PATH) $(MTB_TOOLCHAIN_GCC_ARM__BASE_DIR) $(CY_PYTHON_PATH) $(MTB_TOOLS__OUTPUT_CONFIG_DIR) $(APPNAME) \
                $(CY_ELF_TO_HEX) $(CY_ELF_TO_HEX_OPTIONS) $(CY_ELF_TO_HEX_FILE_ORDER) $(CY_HEX_TO_BIN) \
                $(FLASH_ERASE_SECONDARY_SLOT_VALUE) $(MCUBOOT_HEADER_SIZE) $(APP_BUILD_VERSION) $(FLASH_BASE_ADDRESS) \
                $(FLASH_AREA_IMG_1_PRIMARY_START) $(FLASH_AREA_IMG_1_PRIMARY_SIZE) $(FLASH_AREA_IMG_1_SECONDARY_START) \
                $(OTA_PLATFORM) $(MCUBOOT_KEY_DIR) $(MCUBOOT_KEY_FILE)\
            POSTBUILD+=rm -rf $(MTB_TOOLS__OUTPUT_CONFIG_DIR)/$(APPNAME).bin;
        endif

        #--------------------------------------
        # PSoC POSTBUILD
        #--------------------------------------
        ifneq ($(OTA_PLATFORM),$(filter $(OTA_PLATFORM),XMC7100 XMC7200 CYW20829 CYW89829))
            # PSoC 062, PSoC 062S3, PSoC 064B0S2 Here
            SIGN_SCRIPT_FILE_PATH=$(RELATIVE_FILE1_FILE2)/../../scripts/mcuboot/sign_script.bash
            IMGTOOL_COMMAND_ARG=sign
            CY_SIGNING_KEY_ARG="-k $(MCUBOOT_KEY_DIR)/$(MCUBOOT_KEY_FILE)"

            ifeq ($(ACTUAL_TARGET),CY8CKIT-064B0S2-4343W)        #IMP_CHECK
                # values changed for 064B0S2 board
                IMGTOOL_COMMAND_ARG=do_not_sign
            endif

            # If POSTBUILD fails, Turn this on to check all args are present
            ifneq ($(CY_SECONDSTAGE),)
                ifeq ($(OTA_BUILD_POST_VERBOSE),1)
                    $(info SIGN_SCRIPT_FILE_PATH            =$(SIGN_SCRIPT_FILE_PATH))
                    $(info OUTPUT_FILE_PATH                 =$(OUTPUT_FILE_PATH))
                    $(info APPNAME                          =$(APPNAME))
                    $(info CY_ELF_TO_HEX                    =$(CY_ELF_TO_HEX))
                    $(info CY_ELF_TO_HEX_OPTIONS            =$(CY_ELF_TO_HEX_OPTIONS))
                    $(info CY_ELF_TO_HEX_FILE_ORDER         =$(CY_ELF_TO_HEX_FILE_ORDER))
                    $(info MCUBOOT_SCRIPT_FILE_DIR          =$(MCUBOOT_SCRIPT_FILE_DIR))
                    $(info IMGTOOL_SCRIPT_NAME              =$(IMGTOOL_SCRIPT_NAME))
                    $(info IMGTOOL_COMMAND_ARG              =$(IMGTOOL_COMMAND_ARG))
                    $(info FLASH_ERASE_SECONDARY_SLOT_VALUE =$(FLASH_ERASE_SECONDARY_SLOT_VALUE))
                    $(info MCUBOOT_HEADER_SIZE              =$(MCUBOOT_HEADER_SIZE))
                    $(info MCUBOOT_MAX_IMG_SECTORS          =$(MCUBOOT_MAX_IMG_SECTORS))
                    $(info APP_BUILD_VERSION                =$(APP_BUILD_VERSION))
                    $(info FLASH_AREA_IMG_1_PRIMARY_START   =$(FLASH_AREA_IMG_1_PRIMARY_START))
                    $(info FLASH_AREA_IMG_1_PRIMARY_SIZE    =$(FLASH_AREA_IMG_1_PRIMARY_SIZE))
                    $(info CY_HEX_TO_BIN                    =$(CY_HEX_TO_BIN))
                    $(info CY_SIGNING_KEY_ARG               =$(CY_SIGNING_KEY_ARG))
                endif # OTA_BUILD_VERBOSE = POST
            endif # SECOND STAGE

            POSTBUILD=$(SIGN_SCRIPT_FILE_PATH) $(OUTPUT_FILE_PATH) $(APPNAME) $(CY_PYTHON_PATH)\
                        $(CY_ELF_TO_HEX) $(CY_ELF_TO_HEX_OPTIONS) $(CY_ELF_TO_HEX_FILE_ORDER)\
                        $(MCUBOOT_SCRIPT_FILE_DIR) $(IMGTOOL_SCRIPT_NAME) $(IMGTOOL_COMMAND_ARG) $(FLASH_ERASE_SECONDARY_SLOT_VALUE) $(MCUBOOT_HEADER_SIZE)\
                        $(MCUBOOT_MAX_IMG_SECTORS) $(APP_BUILD_VERSION) $(FLASH_AREA_IMG_1_PRIMARY_START) $(FLASH_AREA_IMG_1_PRIMARY_SIZE)\
                        $(CY_HEX_TO_BIN) $(CY_SIGNING_KEY_ARG)
        endif # Not PSoC
    else
        # POSTBUILD script provided by the Application.
        ifneq ($(CY_SECONDSTAGE),)
            $(info "")
            $(info OTA_APP_POSTBUILD is defined by Application Makefile.)
            $(info "")
        endif # SECOND STAGE
        POSTBUILD=$(OTA_APP_POSTBUILD)
    endif #OTA_APP_POSTBUILD
endif #CY_SECONDSTAGE
