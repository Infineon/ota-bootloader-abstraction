# MCUBootApp Usage Instructions


## 1. Clone MCUBoot

- You need to first build and program the bootloader app *MCUBootApp* that is available in the [MCUBoot GitHub repo](https://github.com/mcu-tools/mcuboot) before using an MCUBootloader based OTA application.

    - MCUBootApp runs on the CM0+ CPU and starts any OTA enabled application on CM4 core in case of multicore Psoc6 devices.
    - MCUBootApp runs on the CM0+ CPU and starts any OTA enabled application on CM7 core in case of multicore XMC7200 devices.
    - MCUBootApp runs on CM33 CPU along with the OTA enabled applications in the case of 20829 and 89829 devices.

- Clone the MCUBoot repository onto your local machine, **outside of your application directory.**

-  Open a CLI terminal and run the following command:

   ```
   git clone https://github.com/JuulLabs-OSS/mcuboot.git
   ```

- Navigate to the cloned *mcuboot* folder:
   ```
   cd mcuboot
   ```

- Change the branch to get the appropriate version:

   ```
   git checkout v1.9.2-cypress
   ```

- Pull in sub-modules to build mcuboot:

   ```
   git submodule update --init --recursive
   ```

- Install the required Python packages mentioned in *mcuboot/scripts/requirements.txt*. Be sure to use the python instance in ModusToolbox/tools_XX/python:
   ```
   cd mcuboot/scripts

   pip install -r requirements.txt
   ```

<b>Note:</b>
- The **CY8CKIT-064B0S2-4343W** is a special kit that uses cysecureboot. See [cysecuretools for PSoC™ 64, CYW20829 and CYW89829 devices](#5-cysecuretools-for-psoc-64-and-cyw20829-devices) information section below.

## 2. UART differences

The UART Tx and Rx pins are common across most TARGET boards. However, there are exceptions. If "printf()" output are not seen on the terminal, check the UART settings. The default UART Tx and Rx can be changed for your device. </b>

To change default UART Tx and Rx, pass the `UART_RX_DEFAULT=XX` and `UART_TX_DEFAULT=xx` parameters to the MCUBootApp `make` command.
```
    make clean app APP_NAME=MCUBootApp PLATFORM=XXXX FLASH_MAP=XXXX UART_TX_DEFAULT?=P10_1 UART_RX_DEFAULT?=P10_0
    `UART_RX_DEFAULT` - Sets the pin number in the GPIO port used as RX of the debug serial port.
    `UART_TX_DEFAULT` - Sets the pin number in the GPIO port used as TX of the debug serial port.
```

## 3. Build MCUBoot

MCUBootApp supports using a JSON document to define the flash regions used for updating the application.

Read **[MCUBoot Build Commands](./MCUBOOT_BUILD_COMMANDS.md)** to see the flashmaps and build command to use. This ****must**** be the same flashmap used to buid your OTA application.

<b>Notes:</b>
- The JSON files use ABSOLUTE address, which indicate which flash part is to be used for the particular section. The usage in MCUBootApp and OTA Bootloader Support library defines the location by device_id and RELATIVE address. You do not need to do anything, this information is to reduce confusion.

- Watch the MCUBootApp build output. If the python script flashmap.py detects errors in the flash layout, it will report them to stderr and stop the build.

## 4. Program MCUBootApp

- Connect the board to your PC using the provided USB cable through the USB connector.

- Erase flash area before programming MCUBootApp.

- From the terminal, execute the make program command to program the application using the default toolchain to the default target.
    ```
    make program TARGET=<BSP> TOOLCHAIN=<toolchain>
    ```

- Alternatively, the board can be programmed using CyProgrammer, OpenOCD commands or the instructions in your Customer Example Application notes.
    - [OpenOCD commands for PSOC6 devices](https://github.com/mcu-tools/mcuboot/blob/v1.9.2-cypress/boot/cypress/platforms/PSOC6.md#using-openocd-from-command-line)
    - [OpenOCD commands for 20829 devices](https://github.com/mcu-tools/mcuboot/blob/v1.9.2-cypress/boot/cypress/platforms/CYW20829.md#using-openocd-from-command-line)
    - [OpenOCD commands for XMC7200 devices](https://github.com/mcu-tools/mcuboot/blob/v1.9.2-cypress/boot/cypress/platforms/XMC7000.md#xmc7000-secure-boot-configuration-description)

<b>Notes:</b>
- For XMC7200 devices, once the 'status_area' of the MCUBootApp has been erased, this flash area should be filled with the value '0xFF'.
    ```
    flash fillb 0x14030000 0xFF 0x2800
    ```

## 5. cysecuretools for PSoC™ 64, CYW20829 and CYW89829 devices

For CY8CKIT-062B0S2-4343W kit, CYW20829 and CYW89829 devices (SECURE), we use a procedure called "provisioning" to put the CyBootloader into the device. Please refer to the following cysecuretools documents for this procedure.

[Provisioning Steps for CY8CKIT-064B0S2-4343W](https://github.com/Infineon/cysecuretools/blob/master/docs/README_PSOC64.md)

[Provisioning Steps for CYW20829 and CYW89829 devices](https://github.com/Infineon/cysecuretools/blob/master/docs/README_CYW20829.md)

<b>Note:</b>
- For 20829 and 89829 NORMAL_NO_SECURE mode, MCUBootApp should be built and programmed separately.

- The Windows version of ModusToolbox 3.2 does not include a Python package. Therefore, users are required to install python version 3.10 and the python module CySecureTools 5.0.0. For the mcuboot v1.9.2-cypress, CySecureTools v5.0.0 or later is necessary for the 20829 and XMC7200 platforms. Additionally, the 89829 platform specifically requires CySecureTools v5.1.0.

- The following command can be used in the modus-shell terminal to find the current version of CySecureTools.
    ````
    cysecuretools version
    ````

- If the current CySecureTools is not the required version, the following command can be used in modus-shell terminal to update it.
    ````
    pip install --upgrade --force-reinstall cysecuretools
    ````

- Additionally, the following command can be used to view the supported targets and families of devices.
    ````
    cysecuretools device-list
    ````
