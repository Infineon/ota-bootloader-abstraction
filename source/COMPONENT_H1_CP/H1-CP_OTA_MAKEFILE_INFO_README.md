# H1-CP Bootloader Support Makefile Settings for a User OTA Application

## 1. Introduction

This guide shows Makefile variables and build DEFINES used when building an application with ota-bootloader-abstraction library along with ota-update library support.


## 2. Standard H1-CP Bootloader based OTA Application Makefile Defines


| Makefile Variable | Example | Description |
| ----------------- | ------- | ----------- |
| TARGET?=\<target_name\> | TARGET?=APP_CYW955913EVK-01 | Target Kit for build |
| CORE?=\<core_type\> | CORE?=CM33 | Core CPU for Application<br>(CM33, CM4) |
| APPNAME?=\<application_name\> | APPNAME=ota-test | Name of Application |
| TOOLCHAIN?=\<toolchain\> | TOOLCHAIN?=GCC_ARM | GCC_ARM |
| CONFIG?=\<build_level\> | CONFIG?=Debug | Build level<br>- Debug |

## 3. H1-CP Bootloader based OTA Specific Makefile Defines

| Makefile addition | Required | Default if not defined | Description |
| ----------------- | -------- | ---------------------- | ----------- |
| CY_BUILD_LOCATION=\<Application's Build Working Directory path\> | Yes | Error | - |
| APP_VERSION_MAJOR=\<application_version\> | Yes | Error | Application Major version number  X.y.z|
| APP_VERSION_MINOR=\<application_version\> | Yes | Error | Application Minor version number  x.Y.z|
| APP_VERSION_BUILD=\<application_version\> | Yes | Error | Application Build version number x.y.Z |
