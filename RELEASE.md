# Infineon OTA Bootloader Support Library

## What's Included?

See the [README.md](./README.md) for a complete description of the OTA bootloader support library.

## Known issues

| Problem | Workaround |
| ------- | ---------- |
| OTA Bootloader Support Middleware is currently not supported on IAR toolchain with CYW20829B0 kit. | No workaround. Support will be added in a future release. |


## Changelog

### v1.0.0

- New ota-bootloader-abstraction library
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
| ModusToolbox&trade; Software Environment                  | 3.1     |
| - ModusToolbox&trade; Device Configurator                 | 4.10    |
| - ModusToolbox&trade; CapSense Configurator / Tuner tools | 6.10.0  |
| Peripheral Driver Library (PDL CAT1)                      | 3.9.0   |
| GCC Compiler                                              | 11.3.1  |
| IAR Compiler                                              | 9.30.1  |
| Arm Compiler 6                                            | 6.16    |
