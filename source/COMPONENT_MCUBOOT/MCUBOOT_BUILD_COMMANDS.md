# MCUBoot Build Commands

The JSON flash maps were verified to work with MCUBoot v1.9.4-cypress. The JSON flash map files are available in [flashmap](./../../configs/COMPONENT_MCUBOOT/flashmap/) folder.

Choose the configuration to use from the information below and copy the flash map file to your **\<mcuboot\>/boot/cypress/** directory before building MCUBootApp.

## 1. PSoC™ 62 2M Internal Flash Platforms
- CY8CKIT-062S2-43012
- CY8CPROTO-062-4343W
- CY8CEVAL-062S2-MUR-43439M2
- CY8CEVAL-062S2-LAI-4373M2
- CY8CEVAL-062S2-CYW955513SDM2WLIPA

There are flash maps for the upgrade slot in internal flash and in external flash. There are also versions for SWAP and OVERWRITE upgrade methods.

<b>Building MCUBoot</b>

To build MCUBoot for internal only flash SWAP configuration:

```
make clean app APP_NAME=MCUBootApp PLATFORM=PSOC_062_2M FLASH_MAP=./psoc62_2m_int_swap_single.json
```

To build MCUBoot for internal only flash OVERWRITE configuration:

```
make clean app APP_NAME=MCUBootApp PLATFORM=PSOC_062_2M FLASH_MAP=./psoc62_2m_int_overwrite_single.json
```

To build MCUBoot for internal + external flash SWAP configuration:

```
make clean app APP_NAME=MCUBootApp PLATFORM=PSOC_062_2M FLASH_MAP=./psoc62_2m_ext_swap_single.json
```

To build MCUBoot for internal + external flash OVERWRITE configuration:

```
make clean app APP_NAME=MCUBootApp PLATFORM=PSOC_062_2M FLASH_MAP=./psoc62_2m_ext_overwrite_single.json
```

## 2. PSoC™ 62 1M Internal Flash Platforms
- CY8CKIT-062-BLE

<b>Building MCUBoot</b>

To build MCUBoot for internal + internal flash SWAP configuration:

```
make clean app APP_NAME=MCUBootApp PLATFORM=PSOC_062_1M USE_CUSTOM_DEBUG_UART=1 FLASH_MAP=./psoc62_1m_cm0_int_swap_single.json
```

## 3. PSoC™ 62 512K Internal Flash Platforms
- CY8CPROTO-062S3-4343W

<b>Building MCUBoot</b>

To build MCUBoot for internal + external flash OVERWRITE configuration:

```
make clean app APP_NAME=MCUBootApp PLATFORM=PSOC_062_512K USE_CUSTOM_DEBUG_UART=1 FLASH_MAP=./psoc62_512k_ext_overwrite_single.json
```

To build MCUBoot for internal + external flash SWAP configuration:

```
make clean app APP_NAME=MCUBootApp PLATFORM=PSOC_062_512K USE_CUSTOM_DEBUG_UART=1 FLASH_MAP=./psoc62_512k_ext_swap_single.json
```

To build MCUBoot for external flash execute (XIP) SWAP configuration:

```
make clean app APP_NAME=MCUBootApp PLATFORM=PSOC_062_512K USE_CUSTOM_DEBUG_UART=1 FLASH_MAP=./psoc62_512k_xip_swap_single.json
```

## 4. PSoC™ 63 1M Internal Flash Platforms
- CY8CPROTO-063-BLE
- CYBLE-416045-EVAL

<b>Building MCUBoot</b>

To build MCUBoot for internal + internal flash SWAP configuration:

```
make clean app APP_NAME=MCUBootApp PLATFORM=PSOC_063_1M USE_CUSTOM_DEBUG_UART=1 FLASH_MAP=./psoc63_1m_cm0_int_swap_single.json
```

## 5. PSoC™ 64 Platform
- CY8CKIT-064B0S2-4343W

<b>Building MCUBoot</b>

There are no flash maps for this device, the parameters are in the Makefile. We use cysecuretools to provision CY8CPROTO-064B0S2-4343W targets. Please see MCUBOOT_APP_README.md for more details.


## 6. CYW20829 Platforms

The CYW920829M2EVK-02 platform only supports an external flash execution(XIP) configuration.

### NORMAL_NO_SECURE Mode
<b>Building MCUBoot</b>

To build MCUBoot for external flash execute (XIP) SWAP configuration:

```
make clean app APP_NAME=MCUBootApp PLATFORM=CYW20829 USE_CUSTOM_DEBUG_UART=1 USE_EXTERNAL_FLASH=1 USE_XIP=1 FLASH_MAP=./cyw20829_xip_swap_single.json
```

To build MCUBoot for external flash execute (XIP) OVERWRITE configuration:

```
make clean app APP_NAME=MCUBootApp PLATFORM=CYW20829 USE_CUSTOM_DEBUG_UART=1 USE_EXTERNAL_FLASH=1 USE_XIP=1 FLASH_MAP=./cyw20829_xip_overwrite_single.json
```

### SECURE Mode
<b>Building MCUBoot with encryption support</b>

To build MCUBoot for external flash execute (XIP) SWAP configuration with encryption:

```
make clean app APP_NAME=MCUBootApp PLATFORM=CYW20829 USE_CUSTOM_DEBUG_UART=1 USE_EXTERNAL_FLASH=1 USE_XIP=1 LCS=SECURE SMIF_ENC=1 FLASH_MAP=./cyw20829_xip_swap_single.json
```

To build MCUBoot for external flash execute (XIP) OVERWRITE configuration with encryption:

```
make clean app APP_NAME=MCUBootApp PLATFORM=CYW20829 USE_CUSTOM_DEBUG_UART=1 USE_EXTERNAL_FLASH=1 USE_XIP=1 LCS=SECURE SMIF_ENC=1 FLASH_MAP=./cyw20829_xip_overwrite_single.json
```

## 7. XMC7200 Evaluation Kit
- KIT_XMC72_EVK

The XMC7200 platform currently supports only an internal flash configuration.

<b>Building MCUBoot</b>

To build MCUBoot for internal only flash SWAP configuration:

```
make clean app APP_NAME=MCUBootApp PLATFORM=XMC7200 BUILDCFG=Debug FLASH_MAP=./xmc7200_int_swap_single.json PLATFORM_CONFIG=platforms/memory/XMC7000/flashmap/xmc7200_platform.json CORE=CM0P APP_CORE=CM7 APP_CORE_ID=0
```

To build MCUBoot for internal only flash OVERWRITE configuration:

```
make clean app APP_NAME=MCUBootApp PLATFORM=XMC7200 BUILDCFG=Debug FLASH_MAP=./xmc7200_int_swap_single.json PLATFORM_CONFIG=platforms/memory/XMC7000/flashmap/xmc7200_platform.json CORE=CM0P APP_CORE=CM7 APP_CORE_ID=0
```

## 8. CYW89829 Platforms

The CYW989829M2EVB-01 platform only supports an external flash execution(XIP) configuration.

<b>Building MCUBoot</b>

To build MCUBoot for external flash execute (XIP) SWAP configuration:

```
make clean app APP_NAME=MCUBootApp PLATFORM=CYW89829 USE_CUSTOM_DEBUG_UART=1 USE_EXTERNAL_FLASH=1 USE_XIP=1 FLASH_MAP=./cyw89829_xip_swap_single.json
```

To build MCUBoot for external flash execute (XIP) OVERWRITE configuration:

```
make clean app APP_NAME=MCUBootApp PLATFORM=CYW89829 USE_CUSTOM_DEBUG_UART=1 USE_EXTERNAL_FLASH=1 USE_XIP=1 FLASH_MAP=./cyw89829_xip_overwrite_single.json
```

# UART differences

The UART Tx and Rx pins are common across most TARGET boards. However, there are exceptions. If "printf()" output are not seen on the terminal, check the UART settings. The default UART Tx and Rx can be changed for your device. </b>

To change default UART Tx and Rx, pass the `UART_RX_DEFAULT=XX` and `UART_TX_DEFAULT=xx` parameters to the MCUBootApp `make` command.
```
    make clean app APP_NAME=MCUBootApp PLATFORM=XXXX FLASH_MAP=XXXX UART_TX_DEFAULT?=P10_1 UART_RX_DEFAULT?=P10_0
    `UART_RX_DEFAULT` - Sets the pin number in the GPIO port used as RX of the debug serial port.
    `UART_TX_DEFAULT` - Sets the pin number in the GPIO port used as TX of the debug serial port.
```
