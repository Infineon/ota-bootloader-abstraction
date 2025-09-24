# Bootloader Support Makefile Settings for a User OTA Application

## 1. Introduction

This guide shows Makefile variables and build DEFINES used when building an application with ota-bootloader-abstraction library along with ota-update library support.


## 2. Standard MCUBootloader based OTA Application Makefile Defines

| Makefile Variable | Example | Description |
| ----------------- | ------- | ----------- |
| TARGET?=\<target_name\> | TARGET?=KIT_PSE84_EVAL_EPC2 | Target Kit for build |
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
| OTA_PLATFORM=<platform_type> | No | Depends on Target Support | <platform_type> must be one of:<br>PSOC™ Edge E84 (PSE84) platform - ex: KIT_PSE84_EVAL_EPC2, KIT_PSE84_EVAL_EPC4 <br>Default value is set for officially supported kits. For reference kit value should be set. |
| CY_TEST_APP_VERSION_IN_TAR=\<0,1\> | No | 0 | Set to 1 to enable checking application version in TAR file in OTA library when updating using a TAR file. |
| CY_PYTHON_PATH=\<Python installed path\> | Yes | Error | MCUBootloader based OTA Pre-build and Post-build scripts uses python.<br>Users is expected to use this Makefile entry to provide Python path. |
| OTA_APP_POSTBUILD=\<Application's POSTBUILD commands\> | No | Post-build commands for generating Signed BOOT and UPGRADE images. | Users can use this Makefile entry to provide their own post-build commands.<br>If this makefile entry is empty, the ota-bootloader-abstraction library uses the default POSTBUILD commands which create signed BOOT and UPGRADE images.|

<b>Notes:</b>
- ota-bootloader-abstraction repo provides makefiles/ifx_mcuboot/ifx_mcuboot_support.mk file for simplifying the ota-bootloader-abstraction library integration with the applications.
- Application Makefile needs to include ifx_mcuboot_support.mk file from the ota-bootloader-abstraction library for the Target dependent Flashmap Configurations and pre/post build scripts.
    ```
	include ../mtb_shared/ota-bootloader-abstraction/<version>/makefiles/ifx_mcuboot/ifx_mcuboot_support.mk
    ```
