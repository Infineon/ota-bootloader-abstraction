# Bootloader Support Makefile Settings for a User OTA Application

## 1. Introduction

This guide shows Makefile variables and build DEFINES used when building an application with ota-bootloader-abstraction library along with ota-update library support.


## 2. Standard MCUBootloader based OTA Application Makefile Defines


| Makefile Variable | Example | Description |
| ----------------- | ------- | ----------- |
| TARGET?=\<target_name\> | TARGET?=CY8CPROTO-062-4343W | Target Kit for build |
| CORE?=\<core_type\> | CORE?=CM4 | Core CPU for Application<br>(CM33, CM4) |
| APPNAME?=\<application_name\> | APPNAME=ota-test | Name of Application |
| TOOLCHAIN?=\<toolchain\> | TOOLCHAIN?=GCC_ARM | GCC_ARM (default)<br>ARM<br> IAR |
| CONFIG?=\<build_level\> | CONFIG?=Debug | Build level<br>- Debug<br>- Release |

## 3. MCUBootloader based OTA Specific Makefile Defines

| Makefile addition | Required | Default if not defined | Description |
| ----------------- | -------- | ---------------------- | ----------- |
| CY_BUILD_LOCATION=\<Application's Build Working Directory path\> | Yes | Error | Required by flashmap.py for setting output directory for use in sign_script.bash script. |
| APP_VERSION_MAJOR=\<application_version\> | Yes | Error | Application Major version number  X.y.z|
| APP_VERSION_MINOR=\<application_version\> | Yes | Error | Application Minor version number  x.Y.z|
| APP_VERSION_BUILD=\<application_version\> | Yes | Error | Application Build version number x.y.Z |
| OTA_PLATFORM=<platform_type> | No | Depends on Target Support | <platform_type> must be one of:<br>XMC7200 - ex: KIT_XMC72_EVK<br>CYW20829 - ex: CYW920829-KEYBOARD and CYW920829-MOUSE<br>CYW89829 - ex: CYW989829M2EVB-01<br>PSOC_062_512K - ex: CY8CPROTO-062S3-4343W<br>PSOC_062_1M - ex: CY8CKIT-062-BLE<br>PSOC_062_2M    - ex: CY8CPROTO-062-4343W CY8CEVAL-062S2-LAI-4373M2 CY8CEVAL-062S2-CYW943439M2IPA1 CY8CKIT-062S2-43012 <br>PSOC_063_1M - ex: CY8CPROTO-063-BLE<br>PSOC_064_2M    - ex: CY8CKIT-064B0S2-4343W <br>Default value is set for officially supported kits. For reference kit value should be set. |
| OTA_FLASH_MAP=<flash_map.json> | No | Depends on Target Support | Default flash_maps are available [here](./../../configs/COMPONENT_MCUBOOT/flashmap) for supported targets.<br>If this makefile entry is empty then ota-bootloader-abstraction library uses target default flash map for generating flashmap.mk.<br>JSON file passed to flashmap.py that generates flashmap.mk.<br>For XMC7200, new flashmap format is used and it is parsed using flashmap_xmc.py.<br>The JSON file defines:<br>- Internal / external flash usage<br>- Flash area location and sizes<br>- Number of images / slots<br>- XIP (from external flash) if defined |
| OTA_LINKER_FILE=<ota_linker_file> | Yes | Error | Based on selected target, Create OTA linker file for XIP or Non XIP mode. <br> Template linkers for supported platforms are available [here](./../../template_linkers/COMPONENT_MCUBOOT/). |
| CY_TEST_APP_VERSION_IN_TAR=\<0,1\> | No | 0 | Set to 1 to enable checking application version in TAR file in OTA library when updating using a TAR file. |
| CY_PYTHON_PATH=\<Python installed path\> | Yes | Error | MCUBootloader based OTA Pre-build and Post-build scripts uses python.<br>Users is expected to use this Makefile entry to provide Python path. |
| CY_SIGN_KEY_PATH=\<Image Signing Key Path\> | No | Test key 'cypress-test-ec-p256.pem' is used for signing. | MCUBootloader uses the default key for validating images.<br>Users can use this Makefile entry to use their own key for signing BOOT/UPGRADE images. |
| IMG_SIGNING_SCRIPT_TOOL_PATH=\<Image Signing Tool Path\> | No | For PSoC6 - Image Signing Tool provided by MCUBootloader.<br>For 20829 - cysecuretools v4.2 or greater <br>For 89829 - cysecuretools v5.1 or greater <br>For XMC7200 - cysecuretools 5.0 | Users can use this Makefile entry to use a tool of their choice for signing update images.<br>If this makefile entry is empty, ota-bootloader-abstraction library uses the default Image signing tools depending on the Target device. |
| OTA_APP_POLICY_PATH=\<Application's Policy File Path\> | No | Depends on Target Support | User needs to define this Makefile entry to provide the policy file path for 20829 and 89829 devices which use cysecuretools for signing update images.<br>Refer to [MCUBoot App Readme](./MCUBOOT_APP_README.md).<br>This is not required for PSoC6 non-secure devices.  |
| OTA_APP_POSTBUILD=\<Application's POSTBUILD commands\> | No | Post-build commands for generating Signed BOOT and UPGRADE images. | Users can use this Makefile entry to provide their own post-build commands.<br>If this makefile entry is empty, the ota-bootloader-abstraction library uses the default POSTBUILD commands which create signed BOOT and UPGRADE images.|

<b>Notes:</b>
- ota-bootloader-abstraction repo provides makefiles/mcuboot/mcuboot_support.mk file for simplifying the ota-bootloader-abstraction library integration with the applications.
- Application Makefile needs to include mcuboot_support.mk file from the ota-bootloader-abstraction library for the Target dependent Flashmap Configurations and pre/post build scripts.
    ```
	include ../mtb_shared/ota-bootloader-abstraction/<version>/makefiles/mcuboot/mcuboot_support.mk
    ```
