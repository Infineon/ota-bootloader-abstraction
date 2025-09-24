################################################################################
# \file ifx_mcuboot_support.mk
# \version 1.0
#
# \brief
# Makefile containing the MCUboot flashmap dependencies.
#
################################################################################
# \copyright
# Copyright 2025, Cypress Semiconductor Corporation (an Infineon company) or
# an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
#
# This software, including source code, documentation and related
# materials ("Software") is owned by Cypress Semiconductor Corporation
# or one of its affiliates ("Cypress") and is protected by and subject to
# worldwide patent protection (United States and foreign),
# United States copyright laws and international treaty provisions.
# Therefore, you may use this Software only as provided in the license
# agreement accompanying the software package from which you
# obtained this Software ("EULA").
# If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
# non-transferable license to copy, modify, and compile the Software
# source code solely for use in connection with Cypress's
# integrated circuit products.  Any reproduction, modification, translation,
# compilation, or representation of this Software except as specified
# above is prohibited without the express written permission of Cypress.
#
# Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
# EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
# WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
# reserves the right to make changes to the Software without notice. Cypress
# does not assume any liability arising out of the application or use of the
# Software or any product or circuit described in the Software. Cypress does
# not authorize its products for use in any products where a malfunction or
# failure of the Cypress product may reasonably be expected to result in
# significant property damage, injury or death ("High Risk Product"). By
# including Cypress's product in a High Risk Product, the manufacturer
# of such system or application assumes all risk of such use and in doing
# so agrees to indemnify Cypress against all liability.
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
CY_IFX_MCUBOOT_HEADER_SIZE?=0x400

# Default Bootstrap size for MCUBootloader
APP_BOOTSTRAP_SIZE=0x2400

# Internal and external erased flash values
CY_INTERNAL_FLASH_ERASE_VALUE=0x00
CY_EXTERNAL_FLASH_ERASE_VALUE=0xFF

APP_VERSION_REVISION?=0
APP_VERSION_SLOT?=0
CY_COMPANY_ID?=0x1234
CY_PRODUCT_ID?=0x5678
CY_TLV_INDEX_COMPANY_ID ?=0x80
CY_TLV_INDEX_PRODUCT_ID ?=0x81

ifeq ($(CY_MCUBOOT_OTA_IMAGE_VERIFICATION),1)
    CY_OTA_IMAGE_VERIFICATION ?=1
    CY_OTA_DIRECT_XIP ?=1
    ifeq ($(CY_OTA_IMAGE_VERIFICATION),1)
        DEFINES+=CY_OTA_IMAGE_VERIFICATION=$(CY_OTA_IMAGE_VERIFICATION)
    endif

    ifeq ($(ENABLE_ON_THE_FLY_ENCRYPTION),1)
        MCUBOOT_ENC_IMAGES_SMIF =1
        DEFINES+=MCUBOOT_ENC_IMAGES_SMIF=$(MCUBOOT_ENC_IMAGES_SMIF)
    endif

    ifeq ($(CY_OTA_DIRECT_XIP),1)
        DEFINES+=CY_OTA_DIRECT_XIP=$(CY_OTA_DIRECT_XIP)
        # APP revision is combination of app revision and app slot id(0 or 1)
        NEW_REVISION := $(shell echo $$(( $(APP_VERSION_REVISION) << 8 | $(APP_VERSION_SLOT) )))
        override APP_VERSION_REVISION := $(NEW_REVISION)
    endif
    # Define App version
    APP_BUILD_VERSION=$(APP_VERSION_MAJOR).$(APP_VERSION_MINOR).$(APP_VERSION_REVISION)+$(APP_VERSION_BUILD)
else
    CY_OTA_DIRECT_XIP ?=0
    CY_OTA_IMAGE_VERIFICATION ?=0
    APP_BUILD_VERSION?=$(APP_VERSION_MAJOR).$(APP_VERSION_MINOR).$(APP_VERSION_BUILD)
endif

DEFINES+=APP_BUILD_VERSION=$(APP_BUILD_VERSION)

###################################################################################################
# Default OTA_PLATFORM based on TARGET
###################################################################################################

#--------------------------------------
# KIT_PSE84_EVAL_EPC2
#--------------------------------------
ifeq ($(ACTUAL_TARGET), KIT_PSE84_EVAL_EPC2)
    OTA_PLATFORM?=PSE84
    COMPONENTS+=IFX_MCUBOOT CY_MW_IFX_MCUBOOT_PSE84
endif

#--------------------------------------
# KIT_PSE84_EVAL_EPC4
#--------------------------------------
ifeq ($(ACTUAL_TARGET), KIT_PSE84_EVAL_EPC4)
    OTA_PLATFORM?=PSE84
    COMPONENTS+=IFX_MCUBOOT CY_MW_IFX_MCUBOOT_PSE84
endif

#############################
# Check for Required defines
#############################

# Add OTA_PLATFORM in DEFINES for platform-specific code
DEFINES+=$(OTA_PLATFORM)

ifeq ($(CY_OTA_IMAGE_VERIFICATION), 1)
    COMPONENTS+=OTA_VERIFY
endif

###################################################################################################
# OTA Flashmap JSON file Usage
###################################################################################################

ifneq ($(findstring $(MAKECMDGOALS), build prebuild build_proj program program_proj eclipse vscode ewarm ewarm8 uvision uvision5 all),)
    ifeq ($(CY_PYTHON_PATH),)
        $(error CY_PYTHON_PATH is not configured)
    else
        $(info CY_PYTHON_PATH = $(CY_PYTHON_PATH))
    endif # checking for python path

    ifeq ($(OTA_PLATFORM),PSE84)
        #Converting the mcuboot_config.h file to memorymap.mk file
        MEMORYMAP_CONVERTER_SCRIPT?=convert_header_to_mk.bash
        GENERATED_SOURCE_PATH?=$(CURRENT_DIRECTORY)/../bsps/TARGET_APP_$(ACTUAL_TARGET)/config/GeneratedSource
        $(info $(RELATIVE_FILE1_FILE2)/../../scripts/ifx_mcuboot/$(MEMORYMAP_CONVERTER_SCRIPT)  $(GENERATED_SOURCE_PATH)/mcuboot_config.h ./flashmap.mk  $(RELATIVE_FILE1_FILE2)/../../source/COMPONENT_IFX_MCUBOOT/cy_flash_map.h)
        $(shell $(RELATIVE_FILE1_FILE2)/../../scripts/ifx_mcuboot/$(MEMORYMAP_CONVERTER_SCRIPT)  $(GENERATED_SOURCE_PATH)/mcuboot_config.h ./flashmap.mk $(RELATIVE_FILE1_FILE2)/../../source/COMPONENT_IFX_MCUBOOT/cy_flash_map.h)
    endif

    flash_map_status=$(shell if [ -s "flashmap.mk" ]; then echo "success"; fi )
    ifeq ($(flash_map_status),)
        $(info "")
        $(error Failed to create flashmap.mk !)
        $(info "")
    endif # check for flashmap.mk created
endif # check for build command


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
    #  NOTE: Generalize for now, we may need to change this criteria for future devices
    ifneq ($(findstring $(OTA_PLATFORM), PSE84),)
        CY_XIP_SMIF_MODE_CHANGE=1

        # Since we are running hybrid (some in RAM, some in External FLash),    #IMP_CHECK
        # we need to override the WEAK functions in CYHAL
        DEFINES+=CYHAL_DISABLE_WEAK_FUNC_IMPL=1
    endif
endif # USE_XIP

# # TODO: Can we base this on the flash area at RUNTIME rather than defining it at build time?
# #       Flash areas / images are allowed to be placed willy-nilly
# ifeq ($(FLASH_AREA_IMG_1_SECONDARY_DEV_ID),CY_FLASH_DEVICE_INTERNAL_FLASH)
#     FLASH_ERASE_SECONDARY_SLOT_VALUE= $(CY_INTERNAL_FLASH_ERASE_VALUE)
# else
#     FLASH_ERASE_SECONDARY_SLOT_VALUE= $(CY_EXTERNAL_FLASH_ERASE_VALUE)
# endif # SECONDARY_DEV_ID

###################################
# The Defines for the flash map
###################################
DEFINES+=\
    CY_IFX_MCUBOOT_MAX_IMG_SECTORS=$(CY_IFX_MCUBOOT_MAX_IMG_SECTORS)\
    CY_IFX_MCUBOOT_IMAGE_NUMBER=$(CY_IFX_MCUBOOT_IMAGE_NUMBER)\
    CY_IFX_MCUBOOT_BOOTLOADER_DEVICE="$(CY_IFX_MCUBOOT_BOOTLOADER_DEVICE)"\
    CY_IFX_MCUBOOT_BOOTLOADER_OFFSET=$(CY_IFX_MCUBOOT_BOOTLOADER_OFFSET)\
    CY_IFX_MCUBOOT_BOOTLOADER_SLOT_SIZE=$(CY_IFX_MCUBOOT_BOOTLOADER_SLOT_SIZE)\
    FLASH_AREA_IMG_1_PRIMARY_DEVICE="$(FLASH_AREA_IMG_1_PRIMARY_DEVICE)"\
    FLASH_AREA_IMG_1_PRIMARY_OFFSET=$(FLASH_AREA_IMG_1_PRIMARY_OFFSET) \
    FLASH_AREA_IMG_1_PRIMARY_SLOT_SIZE=$(FLASH_AREA_IMG_1_PRIMARY_SLOT_SIZE) \
    FLASH_AREA_IMG_1_SECONDARY_DEVICE="$(FLASH_AREA_IMG_1_SECONDARY_DEVICE)"\
    FLASH_AREA_IMG_1_SECONDARY_OFFSET=$(FLASH_AREA_IMG_1_SECONDARY_OFFSET) \
    FLASH_AREA_IMG_1_SECONDARY_SLOT_SIZE=$(FLASH_AREA_IMG_1_SECONDARY_SLOT_SIZE)

ifneq ($(FLASH_AREA_IMAGE_SWAP_STATUS_DEVICE),)
    DEFINES+=\
        FLASH_AREA_IMAGE_SWAP_STATUS_DEVICE="$(FLASH_AREA_IMAGE_SWAP_STATUS_DEVICE)"\
        FLASH_AREA_IMAGE_SWAP_STATUS_OFFSET=$(FLASH_AREA_IMAGE_SWAP_STATUS_OFFSET)\
        FLASH_AREA_IMAGE_SWAP_STATUS_SLOT_SIZE=$(FLASH_AREA_IMAGE_SWAP_STATUS_SLOT_SIZE)
endif

ifneq ($(FLASH_AREA_IMAGE_SCRATCH_DEVICE),)
    DEFINES+=\
        FLASH_AREA_IMAGE_SCRATCH_DEVICE="$(FLASH_AREA_IMAGE_SCRATCH_DEVICE)"\
        FLASH_AREA_IMAGE_SCRATCH_OFFSET=$(FLASH_AREA_IMAGE_SCRATCH_OFFSET)\
        FLASH_AREA_IMAGE_SCRATCH_SLOT_SIZE=$(FLASH_AREA_IMAGE_SCRATCH_SLOT_SIZE)
endif

ifeq ($(shell test $(CY_IFX_MCUBOOT_IMAGE_NUMBER) -gt 1; echo $$?),0)
    DEFINES+=\
        FLASH_AREA_IMG_2_PRIMARY_DEVICE="$(FLASH_AREA_IMG_2_PRIMARY_DEVICE)" \
        FLASH_AREA_IMG_2_PRIMARY_OFFSET=$(FLASH_AREA_IMG_2_PRIMARY_OFFSET) \
        FLASH_AREA_IMG_2_PRIMARY_SLOT_SIZE=$(FLASH_AREA_IMG_2_PRIMARY_SLOT_SIZE) \
        FLASH_AREA_IMG_2_SECONDARY_DEVICE="$(FLASH_AREA_IMG_2_SECONDARY_DEVICE)" \
        FLASH_AREA_IMG_2_SECONDARY_OFFSET=$(FLASH_AREA_IMG_2_SECONDARY_OFFSET) \
        FLASH_AREA_IMG_2_SECONDARY_SLOT_SIZE=$(FLASH_AREA_IMG_2_SECONDARY_SLOT_SIZE)
endif

ifeq ($(OTA_PLATFORM),PSE84)
    # PSE84 platform is expected to support Multi-Image Updates.
    ifeq ($(shell test $(CY_IFX_MCUBOOT_IMAGE_NUMBER) -gt 2; echo $$?),0)
        DEFINES+=\
            FLASH_AREA_IMG_3_PRIMARY_DEVICE="$(FLASH_AREA_IMG_3_PRIMARY_DEVICE)" \
            FLASH_AREA_IMG_3_PRIMARY_OFFSET=$(FLASH_AREA_IMG_3_PRIMARY_OFFSET) \
            FLASH_AREA_IMG_3_PRIMARY_SLOT_SIZE=$(FLASH_AREA_IMG_3_PRIMARY_SLOT_SIZE) \
            FLASH_AREA_IMG_3_SECONDARY_DEVICE="$(FLASH_AREA_IMG_3_SECONDARY_DEVICE)" \
            FLASH_AREA_IMG_3_SECONDARY_OFFSET=$(FLASH_AREA_IMG_3_SECONDARY_OFFSET) \
            FLASH_AREA_IMG_3_SECONDARY_SLOT_SIZE=$(FLASH_AREA_IMG_3_SECONDARY_SLOT_SIZE)
    endif

    ifeq ($(shell test $(CY_IFX_MCUBOOT_IMAGE_NUMBER) -gt 3; echo $$?),0)
        DEFINES+=\
            FLASH_AREA_IMG_4_PRIMARY_DEVICE="$(FLASH_AREA_IMG_4_PRIMARY_DEVICE)" \
            FLASH_AREA_IMG_4_PRIMARY_OFFSET=$(FLASH_AREA_IMG_4_PRIMARY_OFFSET) \
            FLASH_AREA_IMG_4_PRIMARY_SLOT_SIZE=$(FLASH_AREA_IMG_4_PRIMARY_SLOT_SIZE) \
            FLASH_AREA_IMG_4_SECONDARY_DEVICE="$(FLASH_AREA_IMG_4_SECONDARY_DEVICE)" \
            FLASH_AREA_IMG_4_SECONDARY_OFFSET=$(FLASH_AREA_IMG_4_SECONDARY_OFFSET) \
            FLASH_AREA_IMG_4_SECONDARY_SLOT_SIZE=$(FLASH_AREA_IMG_4_SECONDARY_SLOT_SIZE)
    endif

    ifeq ($(shell test $(CY_IFX_MCUBOOT_IMAGE_NUMBER) -gt 4; echo $$?),0)
        DEFINES+=\
            FLASH_AREA_IMG_5_PRIMARY_DEVICE="$(FLASH_AREA_IMG_5_PRIMARY_DEVICE)" \
            FLASH_AREA_IMG_5_PRIMARY_OFFSET=$(FLASH_AREA_IMG_5_PRIMARY_OFFSET) \
            FLASH_AREA_IMG_5_PRIMARY_SLOT_SIZE=$(FLASH_AREA_IMG_5_PRIMARY_SLOT_SIZE) \
            FLASH_AREA_IMG_5_SECONDARY_DEVICE="$(FLASH_AREA_IMG_5_SECONDARY_DEVICE)" \
            FLASH_AREA_IMG_5_SECONDARY_OFFSET=$(FLASH_AREA_IMG_5_SECONDARY_OFFSET) \
            FLASH_AREA_IMG_5_SECONDARY_SLOT_SIZE=$(FLASH_AREA_IMG_5_SECONDARY_SLOT_SIZE)
    endif

    ifeq ($(shell test $(CY_IFX_MCUBOOT_IMAGE_NUMBER) -gt 5; echo $$?),0)
        DEFINES+=\
            FLASH_AREA_IMG_6_PRIMARY_DEVICE="$(FLASH_AREA_IMG_6_PRIMARY_DEVICE)" \
            FLASH_AREA_IMG_6_PRIMARY_OFFSET=$(FLASH_AREA_IMG_6_PRIMARY_OFFSET) \
            FLASH_AREA_IMG_6_PRIMARY_SLOT_SIZE=$(FLASH_AREA_IMG_6_PRIMARY_SLOT_SIZE) \
            FLASH_AREA_IMG_6_SECONDARY_DEVICE="$(FLASH_AREA_IMG_6_SECONDARY_DEVICE)" \
            FLASH_AREA_IMG_6_SECONDARY_OFFSET=$(FLASH_AREA_IMG_6_SECONDARY_OFFSET) \
            FLASH_AREA_IMG_6_SECONDARY_SLOT_SIZE=$(FLASH_AREA_IMG_6_SECONDARY_SLOT_SIZE)
    endif

    ifeq ($(shell test $(CY_IFX_MCUBOOT_IMAGE_NUMBER) -gt 6; echo $$?),0)
        DEFINES+=\
            FLASH_AREA_IMG_7_PRIMARY_DEVICE="$(FLASH_AREA_IMG_7_PRIMARY_DEVICE)" \
            FLASH_AREA_IMG_7_PRIMARY_OFFSET=$(FLASH_AREA_IMG_7_PRIMARY_OFFSET) \
            FLASH_AREA_IMG_7_PRIMARY_SLOT_SIZE=$(FLASH_AREA_IMG_7_PRIMARY_SLOT_SIZE) \
            FLASH_AREA_IMG_7_SECONDARY_DEVICE="$(FLASH_AREA_IMG_7_SECONDARY_DEVICE)" \
            FLASH_AREA_IMG_7_SECONDARY_OFFSET=$(FLASH_AREA_IMG_7_SECONDARY_OFFSET) \
            FLASH_AREA_IMG_7_SECONDARY_SLOT_SIZE=$(FLASH_AREA_IMG_7_SECONDARY_SLOT_SIZE)
    endif

    ifeq ($(shell test $(CY_IFX_MCUBOOT_IMAGE_NUMBER) -gt 7; echo $$?),0)
        DEFINES+=\
            FLASH_AREA_IMG_8_PRIMARY_DEVICE="$(FLASH_AREA_IMG_8_PRIMARY_DEVICE)" \
            FLASH_AREA_IMG_8_PRIMARY_OFFSET=$(FLASH_AREA_IMG_8_PRIMARY_OFFSET) \
            FLASH_AREA_IMG_8_PRIMARY_SLOT_SIZE=$(FLASH_AREA_IMG_8_PRIMARY_SLOT_SIZE) \
            FLASH_AREA_IMG_8_SECONDARY_DEVICE="$(FLASH_AREA_IMG_8_SECONDARY_DEVICE)" \
            FLASH_AREA_IMG_8_SECONDARY_OFFSET=$(FLASH_AREA_IMG_8_SECONDARY_OFFSET) \
            FLASH_AREA_IMG_8_SECONDARY_SLOT_SIZE=$(FLASH_AREA_IMG_8_SECONDARY_SLOT_SIZE)
    endif
endif


ifeq ($(OTA_PLATFORM),PSE84)
    DEFINES+=PSE84
    DEFINES+=OTA_USE_EXTERNAL_FLASH=1
endif

ifeq ($(CY_RUN_CODE_FROM_XIP),1)
    DEFINES+=CY_RUN_CODE_FROM_XIP=1
    ifeq ($(CY_OTA_IMAGE_VERIFICATION), 1)
        DEFINES+=USE_XIP
    endif
endif

ifeq ($(CY_XIP_SMIF_MODE_CHANGE),1)
    DEFINES+=CY_XIP_SMIF_MODE_CHANGE=1
endif

##################################
# Additional / custom linker flags.
##################################

#IMP_CHECK : Usage of -Wl LDFlag.

ifneq ($(filter $(TOOLCHAIN),GCC_ARM LLVM_ARM),)
    CY_ELF_TO_HEX=$(MTB_TOOLCHAIN_GCC_ARM__OBJCOPY)
    CY_ELF_TO_HEX_OPTIONS="-O ihex"
    CY_ELF_TO_HEX_FILE_ORDER="elf_first"
    ifneq ($(OTA_PLATFORM), PSE84)
        LDFLAGS+="-Wl,--defsym,CY_IFX_MCUBOOT_HEADER_SIZE=$(CY_IFX_MCUBOOT_HEADER_SIZE)"
    endif
    LDFLAGS+="-Wl,--defsym,CY_IFX_MCUBOOT_IMAGE_NUMBER=$(CY_IFX_MCUBOOT_IMAGE_NUMBER),--defsym,FLASH_AREA_IMG_1_PRIMARY_OFFSET=$(FLASH_AREA_IMG_1_PRIMARY_OFFSET),--defsym,FLASH_AREA_IMG_1_SLOT_SIZE=$(FLASH_AREA_IMG_1_SLOT_SIZE)"
    ifeq ($(OTA_PLATFORM),PSE84)
        #LDFLAGS+="-Wl,--defsym,FLASH_AREA_BOOTLOADER_START=$(FLASH_AREA_BOOTLOADER_START)"
        ifeq ($(BOOTLOADER_AREA), INTERNAL_RRAM)
            LDFLAGS+="-Wl,--defsym,BOOTLOADER_AREA_INTERNAL_RRAM=1"
        endif
    endif
    ifeq ($(OTA_PLATFORM),PSE84)
        ifeq ($(shell test $(CY_IFX_MCUBOOT_IMAGE_NUMBER) -gt 1; echo $$?),0)
            LDFLAGS+="-Wl,--defsym,FLASH_AREA_IMG_2_PRIMARY_OFFSET=$(FLASH_AREA_IMG_2_PRIMARY_OFFSET),--defsym,FLASH_AREA_IMG_2_SLOT_SIZE=$(FLASH_AREA_IMG_2_SLOT_SIZE)"
        endif
        ifeq ($(shell test $(CY_IFX_MCUBOOT_IMAGE_NUMBER) -gt 2; echo $$?),0)
            LDFLAGS+="-Wl,--defsym,FLASH_AREA_IMG_3_PRIMARY_OFFSET=$(FLASH_AREA_IMG_3_PRIMARY_OFFSET),--defsym,FLASH_AREA_IMG_3_SLOT_SIZE=$(FLASH_AREA_IMG_3_SLOT_SIZE)"
        endif
        ifeq ($(shell test $(CY_IFX_MCUBOOT_IMAGE_NUMBER) -gt 3; echo $$?),0)
            LDFLAGS+="-Wl,--defsym,FLASH_AREA_IMG_4_PRIMARY_OFFSET=$(FLASH_AREA_IMG_4_PRIMARY_OFFSET),--defsym,FLASH_AREA_IMG_4_SLOT_SIZE=$(FLASH_AREA_IMG_4_SLOT_SIZE)"
        endif
        ifeq ($(shell test $(CY_IFX_MCUBOOT_IMAGE_NUMBER) -gt 4; echo $$?),0)
            LDFLAGS+="-Wl,--defsym,FLASH_AREA_IMG_5_PRIMARY_OFFSET=$(FLASH_AREA_IMG_5_PRIMARY_OFFSET),--defsym,FLASH_AREA_IMG_5_SLOT_SIZE=$(FLASH_AREA_IMG_5_SLOT_SIZE)"
        endif
        ifeq ($(shell test $(CY_IFX_MCUBOOT_IMAGE_NUMBER) -gt 5; echo $$?),0)
            LDFLAGS+="-Wl,--defsym,FLASH_AREA_IMG_6_PRIMARY_OFFSET=$(FLASH_AREA_IMG_6_PRIMARY_OFFSET),--defsym,FLASH_AREA_IMG_6_SLOT_SIZE=$(FLASH_AREA_IMG_6_SLOT_SIZE)"
        endif
        ifeq ($(shell test $(CY_IFX_MCUBOOT_IMAGE_NUMBER) -gt 6; echo $$?),0)
            LDFLAGS+="-Wl,--defsym,FLASH_AREA_IMG_7_PRIMARY_OFFSET=$(FLASH_AREA_IMG_7_PRIMARY_OFFSET),--defsym,FLASH_AREA_IMG_7_SLOT_SIZE=$(FLASH_AREA_IMG_7_SLOT_SIZE)"
        endif
        ifeq ($(shell test $(CY_IFX_MCUBOOT_IMAGE_NUMBER) -gt 7; echo $$?),0)
            LDFLAGS+="-Wl,--defsym,FLASH_AREA_IMG_8_PRIMARY_OFFSET=$(FLASH_AREA_IMG_8_PRIMARY_OFFSET),--defsym,FLASH_AREA_IMG_8_SLOT_SIZE=$(FLASH_AREA_IMG_8_SLOT_SIZE)"
        endif
    endif
else
    ifeq ($(TOOLCHAIN),IAR)
        CY_ELF_TO_HEX=$(MTB_TOOLCHAIN_IAR__BASE_DIR)/bin/ielftool
        CY_ELF_TO_HEX_OPTIONS="--ihex"
        CY_ELF_TO_HEX_FILE_ORDER="elf_first"
        ifneq ($(OTA_PLATFORM), PSE84)
            LDFLAGS+=--config_def CY_IFX_MCUBOOT_HEADER_SIZE=$(CY_IFX_MCUBOOT_HEADER_SIZE)
        endif
        LDFLAGS+=--config_def CY_IFX_MCUBOOT_IMAGE_NUMBER=$(CY_IFX_MCUBOOT_IMAGE_NUMBER) --config_def FLASH_AREA_IMG_1_PRIMARY_OFFSET=$(FLASH_AREA_IMG_1_PRIMARY_OFFSET) --config_def FLASH_AREA_IMG_1_SLOT_SIZE=$(FLASH_AREA_IMG_1_SLOT_SIZE)
        ifeq ($(CY_IFX_MCUBOOT_IMAGE_NUMBER),2)
            LDFLAGS+=--config_def FLASH_AREA_IMG_2_PRIMARY_OFFSET=$(FLASH_AREA_IMG_2_PRIMARY_OFFSET) --config_def FLASH_AREA_IMG_2_SLOT_SIZE=$(FLASH_AREA_IMG_2_SLOT_SIZE)
        endif
        ifeq ($(CY_IFX_MCUBOOT_IMAGE_NUMBER),3)
            LDFLAGS+=--config_def FLASH_AREA_IMG_3_PRIMARY_OFFSET=$(FLASH_AREA_IMG_3_PRIMARY_OFFSET) --config_def FLASH_AREA_IMG_3_SLOT_SIZE=$(FLASH_AREA_IMG_3_SLOT_SIZE)
        endif
        ifeq ($(CY_IFX_MCUBOOT_IMAGE_NUMBER),4)
            LDFLAGS+=--config_def FLASH_AREA_IMG_4_PRIMARY_OFFSET=$(FLASH_AREA_IMG_4_PRIMARY_OFFSET) --config_def FLASH_AREA_IMG_4_SLOT_SIZE=$(FLASH_AREA_IMG_4_SLOT_SIZE)
        endif
        ifeq ($(CY_IFX_MCUBOOT_IMAGE_NUMBER),5)
            LDFLAGS+=--config_def FLASH_AREA_IMG_5_PRIMARY_OFFSET=$(FLASH_AREA_IMG_5_PRIMARY_OFFSET) --config_def FLASH_AREA_IMG_5_SLOT_SIZE=$(FLASH_AREA_IMG_5_SLOT_SIZE)
        endif
        ifeq ($(CY_IFX_MCUBOOT_IMAGE_NUMBER),6)
            LDFLAGS+=--config_def FLASH_AREA_IMG_6_PRIMARY_OFFSET=$(FLASH_AREA_IMG_6_PRIMARY_OFFSET) --config_def FLASH_AREA_IMG_6_SLOT_SIZE=$(FLASH_AREA_IMG_6_SLOT_SIZE)
        endif
        ifeq ($(CY_IFX_MCUBOOT_IMAGE_NUMBER),7)
            LDFLAGS+=--config_def FLASH_AREA_IMG_7_PRIMARY_OFFSET=$(FLASH_AREA_IMG_7_PRIMARY_OFFSET) --config_def FLASH_AREA_IMG_7_SLOT_SIZE=$(FLASH_AREA_IMG_7_SLOT_SIZE)
        endif
        ifeq ($(CY_IFX_MCUBOOT_IMAGE_NUMBER),8)
            LDFLAGS+=--config_def FLASH_AREA_IMG_8_PRIMARY_OFFSET=$(FLASH_AREA_IMG_8_PRIMARY_OFFSET) --config_def FLASH_AREA_IMG_8_SLOT_SIZE=$(FLASH_AREA_IMG_8_SLOT_SIZE)
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
            ifneq ($(OTA_PLATFORM), PSE84)
                LDFLAGS+=--pd=-DCY_IFX_MCUBOOT_HEADER_SIZE=$(CY_IFX_MCUBOOT_HEADER_SIZE)
            endif
            LDFLAGS+=--pd=-DCY_IFX_MCUBOOT_IMAGE_NUMBER=$(CY_IFX_MCUBOOT_IMAGE_NUMBER) --pd=-DFLASH_AREA_IMG_1_PRIMARY_OFFSET=$(FLASH_AREA_IMG_1_PRIMARY_OFFSET) --pd=-DFLASH_AREA_IMG_1_SLOT_SIZE=$(FLASH_AREA_IMG_1_SLOT_SIZE)
            ifeq ($(OTA_PLATFORM),PSE84)
                ifeq ($(shell test $(CY_IFX_MCUBOOT_IMAGE_NUMBER) -gt 1; echo $$?),0)
                    LDFLAGS+=--pd=-DFLASH_AREA_IMG_2_PRIMARY_OFFSET=$(FLASH_AREA_IMG_2_PRIMARY_OFFSET) --pd=-DFLASH_AREA_IMG_2_SLOT_SIZE=$(FLASH_AREA_IMG_2_SLOT_SIZE)
                endif
                ifeq ($(shell test $(CY_IFX_MCUBOOT_IMAGE_NUMBER) -gt 2; echo $$?),0)
                    LDFLAGS+=--pd=-DFLASH_AREA_IMG_3_PRIMARY_OFFSET=$(FLASH_AREA_IMG_3_PRIMARY_OFFSET) --pd=-DFLASH_AREA_IMG_3_SLOT_SIZE=$(FLASH_AREA_IMG_3_SLOT_SIZE)
                endif
                ifeq ($(shell test $(CY_IFX_MCUBOOT_IMAGE_NUMBER) -gt 3; echo $$?),0)
                    LDFLAGS+=--pd=-DFLASH_AREA_IMG_4_PRIMARY_OFFSET=$(FLASH_AREA_IMG_4_PRIMARY_OFFSET) --pd=-DFLASH_AREA_IMG_4_SLOT_SIZE=$(FLASH_AREA_IMG_4_SLOT_SIZE)
                endif
                ifeq ($(shell test $(CY_IFX_MCUBOOT_IMAGE_NUMBER) -gt 4; echo $$?),0)
                    LDFLAGS+=--pd=-DFLASH_AREA_IMG_5_PRIMARY_OFFSET=$(FLASH_AREA_IMG_5_PRIMARY_OFFSET) --pd=-DFLASH_AREA_IMG_5_SLOT_SIZE=$(FLASH_AREA_IMG_5_SLOT_SIZE)
                endif
                ifeq ($(shell test $(CY_IFX_MCUBOOT_IMAGE_NUMBER) -gt 5; echo $$?),0)
                    LDFLAGS+=--pd=-DFLASH_AREA_IMG_6_PRIMARY_OFFSET=$(FLASH_AREA_IMG_6_PRIMARY_OFFSET) --pd=-DFLASH_AREA_IMG_6_SLOT_SIZE=$(FLASH_AREA_IMG_6_SLOT_SIZE)
                endif
                ifeq ($(shell test $(CY_IFX_MCUBOOT_IMAGE_NUMBER) -gt 6; echo $$?),0)
                    LDFLAGS+=--pd=-DFLASH_AREA_IMG_7_PRIMARY_OFFSET=$(FLASH_AREA_IMG_7_PRIMARY_OFFSET) --pd=-DFLASH_AREA_IMG_7_SLOT_SIZE=$(FLASH_AREA_IMG_7_SLOT_SIZE)
                endif
                ifeq ($(shell test $(CY_IFX_MCUBOOT_IMAGE_NUMBER) -gt 7; echo $$?),0)
                    LDFLAGS+=--pd=-DFLASH_AREA_IMG_8_PRIMARY_OFFSET=$(FLASH_AREA_IMG_8_PRIMARY_OFFSET) --pd=-DFLASH_AREA_IMG_8_SLOT_SIZE=$(FLASH_AREA_IMG_8_SLOT_SIZE)
                endif
            endif
            LDFLAGS+=--diag_suppress=L6314W
        else
            $(error Must define toolchain ! GCC_ARM, ARM, or IAR)
        endif #ARM
    endif #IAR
endif #GCC_ARM

###################################################################################################
#
# Build information
#
###################################################################################################

ifneq ($(findstring $(MAKECMDGOALS), build prebuild build_proj program program_proj eclipse vscode ewarm ewarm8 uvision uvision5 all),)
    ifeq ($(OTA_BUILD_VERBOSE),1)
        $(info Makefile:)
        $(info Makefile: TARGET                         = $(TARGET))
        $(info Makefile: OTA_PLATFORM                   = $(OTA_PLATFORM))
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
        $(info CY_IFX_MCUBOOT_MAX_IMG_SECTORS         = $(CY_IFX_MCUBOOT_MAX_IMG_SECTORS))
        $(info FLASH_AREA_BOOTLOADER_DEVICE           = $(FLASH_AREA_BOOTLOADER_DEVICE))
        $(info FLASH_AREA_BOOTLOADER_OFFSET           = $(FLASH_AREA_BOOTLOADER_OFFSET))
        $(info FLASH_AREA_BOOTLOADER_SLOT_SIZE        = $(FLASH_AREA_BOOTLOADER_SLOT_SIZE))
        $(info CY_IFX_MCUBOOT_IMAGE_NUMBER            = $(CY_IFX_MCUBOOT_IMAGE_NUMBER))
        $(info FLASH_AREA_IMG_1_PRIMARY_DEVICE        = $(FLASH_AREA_IMG_1_PRIMARY_DEVICE))
        $(info FLASH_AREA_IMG_1_PRIMARY_OFFSET        = $(FLASH_AREA_IMG_1_PRIMARY_OFFSET))
        $(info FLASH_AREA_IMG_1_PRIMARY_SLOT_SIZE     = $(FLASH_AREA_IMG_1_PRIMARY_SLOT_SIZE))
        $(info FLASH_AREA_IMG_1_SECONDARY_DEVICE      = $(FLASH_AREA_IMG_1_SECONDARY_DEVICE))
        $(info FLASH_AREA_IMG_1_SECONDARY_OFFSET      = $(FLASH_AREA_IMG_1_SECONDARY_OFFSET))
        $(info FLASH_AREA_IMG_1_SECONDARY_SLOT_SIZE   = $(FLASH_AREA_IMG_1_SECONDARY_SLOT_SIZE))
        $(info FLASH_AREA_IMAGE_SCRATCH_DEVICE        = $(FLASH_AREA_IMAGE_SCRATCH_DEVICE))
        $(info FLASH_AREA_IMAGE_SCRATCH_OFFSET        = $(FLASH_AREA_IMAGE_SCRATCH_OFFSET))
        $(info FLASH_AREA_IMAGE_SCRATCH_SLOT_SIZE     = $(FLASH_AREA_IMAGE_SCRATCH_SLOT_SIZE))
        ifeq ($(MCUBOOT_IMAGE_NUMBER),2)
            $(info FLASH_AREA_IMG_2_PRIMARY_DEVICE        = $(FLASH_AREA_IMG_2_PRIMARY_DEVICE))
            $(info FLASH_AREA_IMG_2_PRIMARY_OFFSET        = $(FLASH_AREA_IMG_2_PRIMARY_OFFSET))
            $(info FLASH_AREA_IMG_2_PRIMARY_SLOT_SIZE     = $(FLASH_AREA_IMG_2_PRIMARY_SLOT_SIZE))
            $(info FLASH_AREA_IMG_2_SECONDARY_DEVICE      = $(FLASH_AREA_IMG_2_SECONDARY_DEVICE))
            $(info FLASH_AREA_IMG_2_SECONDARY_OFFSET      = $(FLASH_AREA_IMG_2_SECONDARY_OFFSET))
            $(info FLASH_AREA_IMG_2_SECONDARY_SLOT_SIZE   = $(FLASH_AREA_IMG_2_SECONDARY_SLOT_SIZE))
        endif
        ifeq ($(MCUBOOT_IMAGE_NUMBER),3)
            $(info FLASH_AREA_IMG_3_PRIMARY_DEVICE        = $(FLASH_AREA_IMG_3_PRIMARY_DEVICE))
            $(info FLASH_AREA_IMG_3_PRIMARY_OFFSET        = $(FLASH_AREA_IMG_3_PRIMARY_OFFSET))
            $(info FLASH_AREA_IMG_3_PRIMARY_SLOT_SIZE     = $(FLASH_AREA_IMG_3_PRIMARY_SLOT_SIZE))
            $(info FLASH_AREA_IMG_3_SECONDARY_DEVICE      = $(FLASH_AREA_IMG_3_SECONDARY_DEVICE))
            $(info FLASH_AREA_IMG_3_SECONDARY_OFFSET      = $(FLASH_AREA_IMG_3_SECONDARY_OFFSET))
            $(info FLASH_AREA_IMG_3_SECONDARY_SLOT_SIZE   = $(FLASH_AREA_IMG_3_SECONDARY_SLOT_SIZE))
        endif
        $(info FLASH_AREA_IMAGE_SWAP_STATUS_DEVICE     = $(FLASH_AREA_IMAGE_SWAP_STATUS_DEVICE))
        $(info FLASH_AREA_IMAGE_SWAP_STATUS_OFFSET     = $(FLASH_AREA_IMAGE_SWAP_STATUS_OFFSET))
        $(info FLASH_AREA_IMAGE_SWAP_STATUS_SLOT_SIZE  = $(FLASH_AREA_IMAGE_SWAP_STATUS_SLOT_SIZE))
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
endif # MAKECMDGOALS


###################################################################################################
# OTA POST BUILD scripting
###################################################################################################

######################################
# Build Location / Output directory
######################################

# output directory for use in the sign_script.bash
ifneq ($(findstring $(MAKECMDGOALS), build prebuild build_proj program program_proj eclipse vscode ewarm ewarm8 uvision uvision5 all),)
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
        #--------------------------------------
        # PSE84 POSTBUILD
        #--------------------------------------
        ifeq ($(OTA_PLATFORM), PSE84)
            $(info Makefile: PSE84 POSTBUILD )
            # PSE84 POSTBUILD call
            $(shell echo "APP_BUILD_VERSION=$(APP_BUILD_VERSION)" >> flashmap.mk)
            $(shell echo "APP_1_NAME=$(APP_1_NAME)" >> flashmap.mk)
            $(shell echo "APP_2_NAME=$(APP_2_NAME)" >> flashmap.mk)
            $(shell echo "APP_3_NAME=$(APP_3_NAME)" >> flashmap.mk)
            $(shell echo "CORE=$(CORE)" >> flashmap.mk)
            $(shell echo "CY_COMPILER_GCC_ARM_DIR=$(CY_COMPILER_GCC_ARM_DIR)" >> flashmap.mk)
        endif
    else
        # POSTBUILD script provided by the Application.
        ifneq ($(findstring $(MAKECMDGOALS), build prebuild build_proj program program_proj eclipse vscode ewarm ewarm8 uvision uvision5 all),)
            $(info "")
            $(info OTA_APP_POSTBUILD is defined by Application Makefile.)
            $(info "")
        endif # MAKECMDGOALS
        POSTBUILD=$(OTA_APP_POSTBUILD)
    endif #OTA_APP_POSTBUILD
endif # MAKECMDGOALS
