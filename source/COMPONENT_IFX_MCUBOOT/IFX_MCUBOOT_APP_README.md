# IFX MCUBootApp Usage Instructions


## 1. Clone EdgeProtectBootloader CE

MCUBootApp runs on the CM33 Secure CPU and starts any OTA enabled application on CM33 Nonsecure or CM55 CPU in case of multicore PSOC™ Edge E84 (PSE84) devices.

<b>Note:</b>
To build MCUboot for PSE84 platform, PSoC™ Edge Protect Bootloader Code Example should be used. For more details, read **[MCUBoot Build Commands](./MCUBOOT_BUILD_COMMANDS.md)**.

## 2. UART differences

The UART Tx and Rx pins are common across most TARGET boards. However, there are exceptions. If "printf()" output are not seen on the terminal, check the UART settings. The default UART Tx and Rx can be changed for your device. </b>

To change default UART Tx and Rx, pass the `UART_RX_DEFAULT=XX` and `UART_TX_DEFAULT=xx` parameters to the MCUBootApp `make` command.
```
    make clean app APP_NAME=MCUBootApp PLATFORM=XXXX FLASH_MAP=XXXX UART_TX_DEFAULT?=P10_1 UART_RX_DEFAULT?=P10_0
    `UART_RX_DEFAULT` - Sets the pin number in the GPIO port used as RX of the debug serial port.
    `UART_TX_DEFAULT` - Sets the pin number in the GPIO port used as TX of the debug serial port.
```

## 3. Build MCUBoot

- TO DO: Add Steps to add proj_bootloader to Explore Hello World CE and update the Bootloader Configurator settings based on the use-case.

## 4. Program MCUBootApp

- Connect the board to your PC using the provided USB cable through the USB connector.

- Erase flash area before programming MCUBootApp.

- From the terminal, execute the make qprogram command to program the application combined hex file containing proj_bootloader, proj_cm33_s, proj_cm33_ns and proj_cm55 using the default toolchain to the default target.
    ```
    make program TARGET=<BSP> TOOLCHAIN=<toolchain>
    ```

- For PSE84 devices, ensure below jumper and pin configuration on board.
    ````
    If the bootloader is not launching from External flash, ensure Boot.1(Boot SW) Pin(P17.6) should be in 'Low'/OFF position else Boot.1(Boot SW) Pin(P17.6) should be in 'High'/ON position.
    Ensure J20 and J21 should be in Tristate/Not Connected position.
    ````

## 5. Edgeprotecttools for PSE84 devices

Edge Protect Tools is required to sign and/or encrypt the images or convert the images to the required MCUboot format. The EdgeProtectTools package is available as part of the Early Access Pack.

- The following command can be used in the modus-shell to install the EdgeProtectTools package.
    ````
    pip install --upgrade --force-reinstall <install-path>/ModusToolbox/packs/ModusToolbox-PSoC-E84-Early-Access-Pack/tools/edgeprotecttools
    ````
<b>Note:</b>
- The default <install-path> of ModusToolbox™ in Windows operating system is C:/Users/<USER>

- Update the COMBINE_SIGN_JSON path in common.mk as following for signing the Boot/Upgrade Images.
    ````
    # BOOT IMG
    COMBINE_SIGN_JSON?=bsps/TARGET_$(TARGET)/config/GeneratedSource/boot_with_bldr.json
    ````

    ````
    # UPGRADE IMG
    COMBINE_SIGN_JSON?=bsps/TARGET_$(TARGET)/config/GeneratedSource/boot_with_bldr_upgr.json
    ````

- For creating the TAR file with the upgrade signed images of each of the CM33_Secure, CM33_Non-Secure and CM55 projects in the  Multi-project Application, user needs to copy the **[sign_script_pse84.bash](../../scripts/sign_script_pse84.bash)** to the root folder of the user application and run the script as following:

    ````
    ./sign_script_pse84.bash <path_of_flashmap_makefile> <path_to_proj_cm33_s signed upgrade file> <path_to_proj_cm33_ns signed upgrade file> <path_to_proj_cm55 signed upgrade file>
    ````

    <b>For example: (If OTA is running in CM33 Nonsecure CPU on KIT_PSE84_EVAL_EPC2/KIT_PSE84_EVAL_EPC4 kit)</b>
    ````
    ./sign_script_pse84.bash ./proj_cm33_ns/flashmap.mk ./build/project_hex/proj_cm33_s_signed.hex ./build/project_hex/proj_cm33_ns_signed.hex ./build/project_hex/proj_cm55_signed.hex
    ````

    **Note:** Script `sign_script_pse84.bash` supports only default cnfigurations of the Explorer (PSE84) code examples with 3 projects, namely proj_cm33_s, proj_cm33_ns and proj_cm55. If user has customized any memory configurations, or size of the application etc, signing script needs to be updated accordingly.
