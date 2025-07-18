# Infineon OTA Bootloader Support Library

## What's Included?

See the [README.md](./README.md) for a complete description of the OTA bootloader support library.

## Changelog

### v1.6.0

- Implemented 'direct-XIP OTA' support for MCUBootloader based OTA on CYW20829(CYW920829M2EVK-02) kits.
- Introduced 'on-the-fly encryption' support for the CYW20829 platform operating in Non-Secure LCS mode life cycle.
- Minor Documentation updates.

### v1.5.3

- Minor postbuild script update for PSOC62.
- Minor Documentation updates.

### v1.5.2

- Template linkers updated for XMC7200 and XMC7100.
- Minor Documentation updates.

### v1.5.1

- Bug fixes.
- Minor Documentation updates.

### v1.5.0

- Added support for XMC7100(KIT_XMC71_EVK_LITE_V1) kit.
- Added OTA image encryption support for 20829(CYW920829M2EVK-02) platform.
- Minor Documentation updates.

### v1.4.0

- Added support for CY8CEVAL-062S2-CYW955513SDM2WLIPA kit.
- Support for secure LCS and OTA image encryption has been added to the CYW920829 platform.

### v1.2.1

- Minor Documentation update.

### v1.2.0

- Added support for CYW955913EVK-01 kit.

### v1.1.0

- Added support for CYW89829(CYW989829M2EVB-01) kits.
- IAR support added for 20829 platform.

### v1.0.0

- New ota-bootloader-abstraction library.
- This ota-bootloader-abstraction has mcubootloader support.
- ota-bootloader-abstraction v1.0.0 works with MTB 3.1
- Legacy support for these devices:
    - CY8CKIT-062S2-43012
    - CY8CKIT-064B0S2-4343W
    - CY8CPROTO-062-4343W
    - CY8CPROTO-062S3-4343W
    - CY8CEVAL-062S2-CYW943439M2IPA1
    - CY8CEVAL-062S2-LAI-4373M2
    - CY8CEVAL-062S2-MUR-43439M2
    - CYW920829M2EVK-02
    - XMC7200(KIT_XMC72_EVK)

## Supported Software and Tools
This version of the library was validated for compatibility with the following software and tools:

| Software and Tools                                        | Version |
| :---                                                      | :----:  |
| ModusToolbox&trade; Software Environment                  | 3.5     |
| - ModusToolbox&trade; Device Configurator                 | 5.30    |
| - ModusToolbox&trade; Bluetooth Configurator              | 3.30    |
| - ModusToolbox&trade; CapSense Configurator / Tuner tools | 6.40.0  |
| Peripheral Driver Library (PDL CAT1)                      | 3.16    |
| GCC Compiler                                              | 14.2.1  |
| IAR Compiler                                              | 9.50.2  |
| Arm Compiler 6                                            | 6.22    |
