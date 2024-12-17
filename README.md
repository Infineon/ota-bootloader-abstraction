# Over-the-Air (OTA) Bootloader Abstraction Middleware Library

The OTA Bootloader Abstraction library has implementation for bootloader specific storage interface APIs for handling OTA upgrade images.
This library is expected to work only with the Infineon [ota-update](https://github.com/Infineon/ota-update/) library which downloads OTA upgrade images on PSoC6, 20829, 89829, XMC7100 and XMC7200 platforms.
Application should not directly call the bootloader-specific storage interface APIs. Rather, during Infineon OTA Agent initialization, the application needs to register these bootloader-specific storage interface APIs as callback APIs.
See the [ota-update](https://github.com/Infineon/ota-update/) library documentation for more details.

## Library Versions

| Library Version                 | Supported MTB version    | Remarks                                   |
|---------------------------------| -------------------------|-------------------------------------------|
| ota-bootloader-abstraction v1.5  | ModusToolbox 3.3      | XMC7100(KIT_XMC71_EVK_LITE_V1) platform support added.<br>Image encryption support added for 20829 platform. |
| ota-bootloader-abstraction v1.4  | ModusToolbox 3.2      | CY8CEVAL-062S2-CYW955513SDM2WLIPA kit support added.<br>Support for secure LCS and OTA image encryption has been added to the CYW920829 platform.<br>cysecuretools v6.1 or greater is required. |
| ota-bootloader-abstraction v1.2  | ModusToolbox 3.2      | CYW955913EVK-01 platform support added |
| ota-bootloader-abstraction v1.1  | ModusToolbox 3.2      | CYW89829 platform support added |
| ota-bootloader-abstraction v1.0  | ModusToolbox 3.1      | cysecuretools v5.0 or greater is required |

## 1. Bootloader Support
To add different bootloader support on Infineon connectivity-enabled MCU platforms, [ota-update](https://github.com/Infineon/ota-update/) library offloads bootloader specific storage interface APIs using callback mechanism.
To handle downloaded OTA upgrade images *ota-update* library calls registered bootloader-specific storage interface callbacks.
The ota-bootloader-abstraction library includes an implementation of bootloader-specific storage interface APIs for various bootloaders. Additionally, it provides the necessary build environment to create BOOT and UPGRADE images for OTA applications.

<b> 1.1 OTA storage interface callback APIs structure </b><br>
OTA storage interface callback APIs structure and callback APIs syntax are defined in *ota-update* library.
Refer [ota-update library API header file](https://github.com/Infineon/ota-update/blob/master/include/cy_ota_api.h) for details.

```
typedef struct cy_ota_storage_interface_s
{
    cy_ota_file_open           ota_file_open;         /**< Creates and open new receive file for the data chunks as they come in. */
    cy_ota_file_read           ota_file_read;         /**< Reads the flash data starting from the given offset.                   */
    cy_ota_file_write          ota_file_write;        /**< Write a data to the flash at the given offset.                         */
    cy_ota_file_close          ota_file_close;        /**< Close the underlying receive file in the specified OTA context.        */
    cy_ota_file_verify         ota_file_verify;       /**< Authenticate received OTA update file specified in the OTA context.    */
    cy_ota_file_validate       ota_file_validate;     /**< The application has validated the new OTA image.                       */
    cy_ota_file_get_app_info   ota_file_get_app_info; /**< Get Application info like Application version and Application ID.      */
} cy_ota_storage_interface_t;
```
<br>

<b> 1.2 OTA storage interface APIs implementation </b><br>

| Storage operation callback |   Bootloader   |             API                  |   Remarks        |
|----------------------------| ---------------|----------------------------------|------------------|
| ota_file_open              | MCUBootloader  |  cy_ota_storage_open()           | Available in source/COMPONENT_MCUBOOT/cy_ota_storage_api.c |
| ota_file_read              | MCUBootloader  |  cy_ota_storage_read()           | Available in source/COMPONENT_MCUBOOT/cy_ota_storage_api.c |
| ota_file_write             | MCUBootloader  |  cy_ota_storage_write()          | Available in source/COMPONENT_MCUBOOT/cy_ota_storage_api.c |
| ota_file_close             | MCUBootloader  |  cy_ota_storage_close()          | Available in source/COMPONENT_MCUBOOT/cy_ota_storage_api.c |
| ota_file_verify            | MCUBootloader  |  cy_ota_storage_verify()         | Available in source/COMPONENT_MCUBOOT/cy_ota_storage_api.c |
| ota_file_validate          | MCUBootloader  |  cy_ota_storage_image_validate() | Available in source/COMPONENT_MCUBOOT/cy_ota_storage_api.c |
| ota_file_get_app_info      | MCUBootloader  |  cy_ota_storage_get_app_info()   | Available in source/COMPONENT_MCUBOOT/cy_ota_storage_api.c |

| Storage operation callback |   Bootloader                 |             API                  |   Remarks        |
|----------------------------| -----------------------------|----------------------------------|------------------|
| ota_file_open              | CYW955913SDCM2WLIPA          |  cy_ota_storage_open()           | Available in source/COMPONENT_CYW955913SDCM2WLIPA/cy_ota_storage_api.c |
| ota_file_read              | CYW955913SDCM2WLIPA          |  cy_ota_storage_read()           | Available in source/COMPONENT_CYW955913SDCM2WLIPA/cy_ota_storage_api.c |
| ota_file_write             | CYW955913SDCM2WLIPA          |  cy_ota_storage_write()          | Available in source/COMPONENT_CYW955913SDCM2WLIPA/cy_ota_storage_api.c |
| ota_file_close             | CYW955913SDCM2WLIPA          |  cy_ota_storage_close()          | Available in source/COMPONENT_CYW955913SDCM2WLIPA/cy_ota_storage_api.c |
| ota_file_verify            | CYW955913SDCM2WLIPA          |  cy_ota_storage_verify()         | Available in source/COMPONENT_CYW955913SDCM2WLIPA/cy_ota_storage_api.c |
| ota_file_validate          | CYW955913SDCM2WLIPA          |  cy_ota_storage_image_validate() | Available in source/COMPONENT_CYW955913SDCM2WLIPA/cy_ota_storage_api.c |
| ota_file_get_app_info      | CYW955913SDCM2WLIPA          |  cy_ota_storage_get_app_info()   | Available in source/COMPONENT_CYW955913SDCM2WLIPA/cy_ota_storage_api.c |

<b>NOTE:</b>  The current version of ota-bootloader-abstraction library implements only storage interface APIs for MCUBootloader based OTA and CYW955913EVK-01 in-built Bootloader based OTA.

## 2. MCUBootloader Support

*ota-bootloader-abstraction* library has below support for MCUBootloader based OTA on PSoC6, 20829, 89829, XMC7100 and XMC7200 platforms.

- Template flashmaps for PSoC6, 20829, 89829, XMC7100 and XMC7200 platforms.

- Template linker files for GCC_ARM, ARM, and IAR toolchains.

- Storage operation callback APIs to handle MCUBootloader based upgrade image.

- Prebuild and Postbuild scripts for generating and signing MCUBootloader based BOOT and UPGRADE image of an OTA Application.

<b> Supported devices: </b><br>
- PSoC™ 6 MCU with
  - AIROC™ CYW4343W
  - CYW43012 Wi-Fi & Bluetooth® combo chip
- CYW920829M2EVK-02 evaluation board.
- CY8CKIT-062-BLE
- CY8CPROTO-063-BLE
- CYBLE-416045-EVAL
- KIT_XMC72_EVK
- KIT_XMC72_EVK_MUR_43439M2
- KIT_XMC71_EVK_LITE_V1
- CYW989829M2EVB-01
- CY8CEVAL-062S2-CYW955513SDM2WLIPA

The *ota-update* along with *ota-bootloader-abstraction* library works in concert with MCUBootloader to provide a no-fail solution to updating device software in the field.

MCUBootloader is a secure bootloader application for 32-bit microcontrollers and users should build it outside of the user OTA application. It is programmed separately to the device before flashing the user OTA application and is not updated for the life of the device.

User OTA application will include the *ota-update* library along with *ota-bootloader-abstraction* library, which will set the appropriate flags so that MCUBootloader knows when to perform an update of your application. The OTA library will download a new (updated) application, store it in flash, and set flags so that on the next reset, MCUBootloader will see there is an update available.

<b> The basic device boot sequence is as follows: </b>
1. ROM boot will start MCUBootloader
2. If no update is downloaded, MCUBootloader starts the current application.
3. MCUBootloader determines if there is an update in the Secondary Slot
4. If there is an update available:<br>
   a. MCUBootloader verifies the update.<br>
   b. MCUBootloader SWAPs (or OVERWRITEs) the current with the update applications.<br>
   c. MCUBootloader starts the new (updated) application.<br>
   d. The updated application must call cy_ota_storage_image_validate() to validate the update.<br>
   e. For SWAP, if the new application does not call cy_ota_storage_image_validate(), MCUBootloader will REVERT on the next reset.<br>

<b> NOTE:</b> On secure MCUs such as PSoC™ 64, MCUBootloader is flashed as part of the provisioning step performed by the provisioning tools, and does not need to be built separately. For more information see [MCUBootloader App Information](./source/COMPONENT_MCUBOOT/MCUBOOT_APP_README.md#5-cysecuretools-for-psoc-64-and-cyw20829-devices).<br><br>

<b> 2.1 Flash layout requirements</b><br>

MCUBootApp and the OTA enabled application must use the same flash layout.

The flash is separated into sections also called slots. The current application occupies the Primary (BOOT) Slot. The update application (new version) will be downloaded into the Secondary (UPGRADE) Slot.
Besides primary and secondary slots, MCUBootApp has a code section, a section for a "Swap Buffer" and a section for "Status Info".

| Section         | Size                 | Description                |
|-----------------| ---------------------|----------------------------|
| MCUBootApp      | 0x00028000           | MCUBootApp code            |
| Primary Slot    | Varies by flash size | User Application (current) |
| Secondary Slot  | Same size as Primary | Update Application (new)   |
| Swap Buffer     | Varies by flash size | Used by MCUBootApp for SWAP operation |
| Status          | Varies by size of Slots | Used by MCUBootApp for update status |

For more information, please see [OTA Flash Layout Information](./source/COMPONENT_MCUBOOT/MCUBOOT_OTA_FLASH_LAYOUT_README.md).<br><br>

<b> 2.2 Target and Flashmap Use</b><br>

Template flashmaps for the supported targets as of v1.0.0 release. These flashmaps are available in [MCUBootloader based OTA Flashmap folder](./configs/COMPONENT_MCUBOOT/flashmap/).

| Target | Internal<br>Flash size | OTA_PLATFORM | Flashmaps |
|-------------------|---------------------|----|------|
| CY8CPROTO-062-4343W<br>CY8CKIT-062S2-43012<br>CY8CPROTO-062-4343W<br>CY8CEVAL-062S2-LAI-4373M2<br>CY8CEVAL-062S2-MUR-43439M2<br>CY8CEVAL-062S2-CYW955513SDM2WLIPA| 2M | PSOC_062_2M | Default - psoc62_2m_ext_swap_single.json<br>psoc62_2m_ext_overwrite_single.json<br>psoc62_2m_int_overwrite_single.json<br>psoc62_2m_int_swap_single.json|
| CY8CKIT-062-BLE | 1M | PSOC_062_1M |Default - psoc62_1m_cm0_int_swap_single.json |
| CY8CPROTO-063-BLE<br>CYBLE-416045-EVAL | 1M | PSOC_063_1M | Default - psoc63_1m_cm0_int_swap_single.json |
| CY8CPROTO-062S3-4343W  | 512K | PSOC_062_512K | Default - psoc62_512k_xip_swap_single.json<br> psoc62_512k_ext_overwrite_single.json<br>psoc62_512k_ext_swap_single.json |
| CY8CKIT-064B0S2-4343W | 2M | PSOC_064_2M | Default - policy_single_CM0_CM4_smif_swap.json |
| CYW920829M2EVK-02 | 0K | CYW20829 | Default - cyw20829_xip_swap_single.json<br> cyw20829_xip_overwrite_single.json |
| CYW989829M2EVB-01 | 0K | CYW89829 | Default - cyw89829_xip_swap_single.json<br> cyw89829_xip_overwrite_single.json |
| KIT_XMC71_EVK_LITE_V1 | 4MB | XMC7100 | Default - xmc7100_int_swap_single.json<br> xmc7100_int_overwrite_single.json |
| KIT_XMC72_EVK<br>KIT_XMC72_EVK_MUR_43439M2 | 8MB | XMC7200 | Default - xmc7200_int_swap_single.json<br> xmc7200_int_overwrite_single.json |

<br>

| 2M Internal Flash Maps | Memory Usage |
|--------------------------------|--------------|
| psoc62_2m_ext_overwrite_single.json   | Internal primary, external secondary  |
| psoc62_2m_ext_swap_single.json        | Internal primary, external secondary  |
| psoc62_2m_int_overwrite_single.json   | Internal only (primary and secondary) |
| psoc62_2m_int_swap_single.json        | Internal only (primary and secondary) |

| 1M Internal Flash Maps | Memory Usage |
|--------------------------------|--------------|
| psoc62_1m_cm0_int_swap_single.json    | Internal only (primary and secondary) |
| psoc63_1m_cm0_int_swap_single.json    | Internal only (primary and secondary) |

| 512K Internal Flash Maps | Memory Usage |
|--------------------------------|--------------|
| psoc62_512k_ext_overwrite_single.json | Internal primary, external secondary |
| psoc62_512k_ext_swap_single.json      | Internal primary, external secondary |
| psoc62_512k_xip_swap_single.json      | External only (primary and secondary) |

| 0K Internal Flash Maps | Memory Usage |
|--------------------------------|--------------|
| cyw20829_xip_overwrite_single.json       | External only (primary and secondary) |
| cyw20829_xip_swap_single.json            | External only (primary and secondary) |
| cyw89829_xip_overwrite_single.json       | External only (primary and secondary) |
| cyw89829_xip_swap_single.json            | External only (primary and secondary) |

| 4M Internal Flash Maps | Memory Usage |
|--------------------------------|--------------|
| xmc7100_int_swap_single.json         | Internal only (primary and secondary) |
| xmc7100_int_overwrite_single.json    | Internal only (primary and secondary) |

| 8M Internal Flash Maps | Memory Usage |
|--------------------------------|--------------|
| xmc7200_int_swap_single.json         | Internal only (primary and secondary) |
| xmc7200_int_overwrite_single.json    | Internal only (primary and secondary) |

<b> 2.3 MCUBootApp Cloning and Building</b><br>

MCUBootloader Application i.e MCUBootApp is a standalone application. It is an open source software taken and customized in terms of Flash map and is built outside of ModusToolbox. MCUBootApp is programmed/flashed on the device one time, at manufacture (or for development).

- MCUBootApp runs on the CM0+ CPU and starts any OTA enabled application on CM4 core in case of multicore Psoc6 devices and on CM7 core in case of XMC7100 and XMC7200 devices.
- In case of 20829 and 89829 devices, MCUBootApp runs on CM33 CPU along with the OTA enabled applications.

MCUBoot itself is not OTA upgradable.

For cloning and building MCUBootApp refer to [MCUBootApp README](./source/COMPONENT_MCUBOOT/MCUBOOT_APP_README.md).

## 3. CYW955913EVK-01 in-built Bootloader Support

*ota-bootloader-abstraction* library has below support for CYW955913EVK-01 in-built Bootloader based OTA.

- Storage operation callback APIs to handle the in-built Bootloader based upgrade image.

<b> Supported devices: </b><br>
- CYW955913EVK-01.

The *ota-update* along with *ota-bootloader-abstraction* library works in concert with CYW955913EVK-01 in-built Bootloader to provide a no-fail solution to updating device software in the field.

An in-built Bootloader is preprogrammed on CYW955913EVK-01 and is not updated for the life of the device.

User OTA application will include the *ota-update* library along with *ota-bootloader-abstraction* library, which will set the appropriate flags so that CYW955913EVK-01 in-built bootloader knows when to perform an update of your application. The OTA library will download a new (updated) application, store it in flash, and set flags so that on the next reset, Bootloader will see there is an update available.

<b> The basic device boot sequence is as follows: </b>
1. ROM boot will start CYW955913EVK-01 in-built Bootloader.
2. The in-built Bootloader starts the current application in Active DS.
3. If an update is available, the OTA application uses the *ota-update* and *ota-bootloader-abstraction* library to download and store it in the Non-Active DS. Subsequently,<br>
   a. When the switch image API (cy_ota_storage_switch_to_new_image) is called, the CYW955913EVK-01 in-built Bootloader verifies the update image headers and certificates.<br>
   b. Upon finding a valid upgrade image, the CYW955913EVK-01 in-built Bootloader activates the Non-Active DS and designates the active DS as Non-Active DS.<br>
   c. Following a reset, the CYW955913EVK-01 in-built Bootloader initiates the application (Upgraded Image) in the Active DS.<br>

## 4. Enabling MCUBootloader based OTA in an Application

- Before enabling MCUBootloader based OTA in an application, the user must build and program MCUBootApp on the chosen platform(s). Refer to refer to [MCUBootApp README](./source/COMPONENT_MCUBOOT/MCUBOOT_APP_README.md).

- Create an *ota-bootloader-abstraction.mtb* file to pull *ota-bootloader-abstraction* library which has storage APIs to handle the MCUBootloader based OTA upgrade files. Place *ota-bootloader-abstraction.mtb* in the application *deps* folder. The contents of *ota-bootloader-abstraction.mtb* should be as follows:
    ```
    https://github.com/Infineon/ota-bootloader-abstraction#latest-v1.X#$$ASSET_REPO$$/ota-bootloader-abstraction/latest-v1.X
    ```

- After adding *ota-bootloader-abstraction.mtb* file to the *deps* folder of your application, Run the command 'make getlibs' to fetch *ota-bootloader-abstraction* library and its dependencies.
    ```
    make getlibs
    ```

- Update OTA Application makefile by referring [OTA Bootloader Abstraction Makefile Readme](./source/COMPONENT_MCUBOOT/MCUBOOT_OTA_MAKEFILE_INFO_README.md).

- For *MCUBootloader based OTA Support*, add the following components to the Application makefile.
    ```
    COMPONENTS+=MCUBOOT
    ```

- Make sure that the Python module is installed on your computer, preferably Python version 3.8. Additionally, Application need to provide the path to the installed Python module by setting CY_PYTHON_PATH in the OTA Application makefile. The pre and post build scripts of the *ota-bootloader-abstraction* library use this Python module to parse memory configurations.
    ```
    CY_PYTHON_PATH = <Path to Python module>
    ```

- *ota-bootloader-abstraction* library has makefile *mcuboot_support.mk* located in [makefiles folder](./makefiles/mcuboot/) which has necessary pre and post build scripts for generating MCUBootloader based BOOT and UPGRADE images of OTA application.

- Include *mcuboot_support.mk* which is located in [makefiles folder](./makefiles/mcuboot/) from OTA Application makefile.
    ```
    include ../mtb_shared/ota-bootloader-abstraction/<version>/makefiles/mcuboot/mcuboot_support.mk
    ```

- Configure OTA storage interface callback APIs in OTA application, And pass the same storage interface while starting OTA agent.
    ```
    /* OTA storage interface callbacks */
    cy_ota_storage_interface_t ota_interfaces =
    {
       .ota_file_open            = cy_ota_storage_open,
       .ota_file_read            = cy_ota_storage_read,
       .ota_file_write           = cy_ota_storage_write,
       .ota_file_close           = cy_ota_storage_close,
       .ota_file_verify          = cy_ota_storage_verify,
       .ota_file_validate        = cy_ota_storage_image_validate,
       .ota_file_get_app_info    = cy_ota_storage_get_app_info
    };

    /* Register OTA storage interface callbacks with OTA agent */
    cy_ota_agent_start(&ota_test_network_params, &ota_test_agent_params, &ota_interfaces, &ota_context);

    ```

- OTA storage interface calls Flash operations(Read, Write, erase) to store upgrade images in UPGRADE slot.
 *ota-bootloader-abstraction* implements flash operation using *mtb-pdl-cat1* library APIs and implementation is available in [configs folder](./configs/COMPONENT_MCUBOOT/flash/).
 User can use same implementation by copying contents of [configs folder](./configs/COMPONENT_MCUBOOT/flash/) to application space or Can implement flash operation APIs defined in [cy_ota_flash.h](./source/COMPONENT_MCUBOOT/cy_ota_flash.h).

- For Secure platforms like PSoC64, MCUBootloader will get programmed during kit provisioning process. Follow steps in 3.1 before building OTA application.

- During application build, Required BOOT and UPGRADE images(.bin) of OTA application will get generated in configured build directory.

- Program the BOOT image and use UPGRADE image (<App name>.bin) for OTA.

<b> 4.1 Provisioning PSoC64</b><br>
- For Secure platforms like PSoC64, MCUBootloader will get programmed during kit provisioning process.

- Prior to provisioning the PSoC64 kit (CY8CKIT-064B0S2-4343W), User need to create *policies* and *keys* for provisioning. To do this, execute the following 'init' command from application root folder.
    ```
    cysecuretools -t CY8CKIT-064B0S2-4343W init
    ```

- The *init* command creates the *keys*, *packets*, *policy*, and *prebuilt* directories in application root folder. These keys and policies are utilized during the provisioning and OTA image signing process.

- Copy policy *policy_single_CM0_CM4_smif_swap.json* from [configs folder](./configs/COMPONENT_MCUBOOT/flashmap/) and replace it with the one in *policy* folder which is generated after executing *init* command.

- Please copy the *policy_single_CM0_CM4_smif_swap.json* file from the [configs folder](./configs/COMPONENT_MCUBOOT/flashmap/) and replace it with the version in the *policy* folder, which will be generated after running the *init* command.

- The public key is programmed during the provisioning for further image verification. The private key is used to sign the image with the user application. Create a private key using below command. The public part of the key will be taken from the private during provisioning.
    ```
    cysecuretools -t CY8CKIT-064B0S2-4343W -p policy/policy_single_CM0_CM4_swap.json create-keys
    ```

- Provision the kit with policy *policy_single_CM0_CM4_smif_swap.json*. Refer [Provisioning Steps for CY8CKIT-064B0S2-4343W](https://github.com/Infineon/cysecuretools/blob/master/docs/README_PSOC64.md) for complete provisionig steps.

- During provisioning, the MCU bootloader is also loaded and executed from the CM0 core upon reboot.

## 5. Enabling OTA on CYW955913EVK-01 applications

- Create an *ota-bootloader-abstraction.mtb* file to pull *ota-bootloader-abstraction* library which has storage APIs to handle the MCUBootloader based OTA upgrade files. Place *ota-bootloader-abstraction.mtb* in the application *deps* folder. The contents of *ota-bootloader-abstraction.mtb* should be as follows:
    ```
    https://github.com/Infineon/ota-bootloader-abstraction#latest-v1.X#$$ASSET_REPO$$/ota-bootloader-abstraction/latest-v1.X
    ```

- After adding *ota-bootloader-abstraction.mtb* file to the *deps* folder of your application, Run the command 'make getlibs' to fetch *ota-bootloader-abstraction* library and its dependencies.
    ```
    make getlibs
    ```

- Update OTA Application makefile by referring [OTA Bootloader Abstraction Makefile Readme](./source/COMPONENT_COMPONENT_CYW955913SDCM2WLIPA/COMPONENT_CYW955913SDCM2WLIPA_OTA_MAKEFILE_INFO_README.md).

- For *CYW955913EVK-01 in-built Bootloader based OTA Support*, add the following components to the Application makefile.
    ```
    COMPONENTS+=CYW955913SDCM2WLIPA
    ```

- Configure OTA storage interface callback APIs in OTA application, And pass the same storage interface while starting OTA agent.
    ```
    /* OTA storage interface callbacks */
    cy_ota_storage_interface_t ota_interfaces =
    {
       .ota_file_open            = cy_ota_storage_open,
       .ota_file_read            = cy_ota_storage_read,
       .ota_file_write           = cy_ota_storage_write,
       .ota_file_close           = cy_ota_storage_close,
       .ota_file_verify          = cy_ota_storage_verify,
       .ota_file_validate        = cy_ota_storage_image_validate,
       .ota_file_get_app_info    = cy_ota_storage_get_app_info
    };

    /* Register OTA storage interface callbacks with OTA agent */
    cy_ota_agent_start(&ota_test_network_params, &ota_test_agent_params, &ota_interfaces, &ota_context);

    ```

- OTA storage interface calls Flash operations(Read, Write, erase) to store upgrade images in Non active DS.

- Once after succesfully downloading OTA upgrade image, switch to new image by calling cy_ota_storage_switch_to_new_image() which is available in *source/COMPONENT_CYW955913SDCM2WLIPA/cy_ota_storage_api.c*.

- During application build, Required BOOT and UPGRADE images(.bin) of OTA application will get generated in configured build directory.

- Program the BOOT image and use UPGRADE image (<App name>.bin) for OTA.

## 6. Enabling Debug Output

The *ota-bootloader-abstraction* library disables all debug log messages by default. Do the following to enable log messages:

1. Add the `ENABLE_OTA_BOOTLOADER_ABSTRACTION_LOGS` macro to the *DEFINES* in the code example's makefile. The makefile entry should look like as follows:
    ```
    DEFINES+=ENABLE_OTA_BOOTLOADER_ABSTRACTION_LOGS
    ```

2. Call the `cy_log_init()` function provided by the *cy-log* module. cy-log is part of the *connectivity-utilities* library. See [connectivity-utilities library API documentation](https://infineon.github.io/connectivity-utilities/api_reference_manual/html/group__logging__utils.html) for cy-log details.

## 7. Note on Using Windows 10

When using ModusToolbox, you will need to install the pip requirements to Python in the ModusToolbox installation.

```
\<ModusToolbox\>/tools_3.*/python/python -m pip install -r \<ota-bootloader-abstraction\>scripts/mcuboot/imgtool/requirements.txt
```

## 8. Supported Toolchains

- GCC
- IAR
- ARM C

For the toolchain version information, please refer to [ota-bootloader-abstraction Release.md](./RELEASE.md).

## 9. Supported OS

- FreeRTOS
- ThreadX

## 10. Supported Kits

- [PSoC™ 6 Wi-Fi BT Prototyping Kit](https://www.infineon.com/cms/en/product/evaluation-boards/cy8cproto-062-4343w/) (CY8CPROTO-062-4343W)
- [PSoC™ 62S2 Wi-Fi BT Pioneer Kit](https://www.infineon.com/cms/en/product/evaluation-boards/cy8ckit-062s2-43012/) (CY8CKIT-062S2-43012)
- [PSoC™ 62S3 Wi-Fi BT Prototyping Kit ](https://www.infineon.com/cms/en/product/evaluation-boards/cy8cproto-062s3-4343w/)(CY8CPROTO-062S3-4343W)
- [PSoC™ 64 Secure Boot Wi-Fi BT Pioneer Kit](https://www.infineon.com/cms/en/product/evaluation-boards/cy8ckit-064b0s2-4343w/) (CY8CKIT-064B0S2-4343W)
- [CY8CEVAL-062S2 Evaluation Kit](https://www.infineon.com/cms/en/product/evaluation-boards/cy8ceval-062s2/)(CY8CEVAL-062S2-LAI-4373M2, CY8CEVAL-062S2-MUR-43439M2 and CY8CEVAL-062S2-CYW955513SDM2WLIPA)
- [PSoC™ 6-BLE Pioneer Kit](https://www.infineon.com/cms/en/product/evaluation-boards/cy8ckit-062-ble/) (CY8CKIT-062-BLE)
- [PSoC™ 6 BLE Prototyping Kit](https://www.infineon.com/cms/en/product/evaluation-boards/cy8cproto-063-ble/) (CY8CPROTO-063-BLE)
- [EZ-BLE Arduino Evaluation Board](https://www.infineon.com/cms/en/product/evaluation-boards/cyble-416045-eval/) (CYBLE-416045-EVAL)
- [AIROC™ CYW20829 Bluetooth® LE SoC](https://www.infineon.com/cms/en/product/promopages/airoc20829/) (CYW920829M2EVK-02)
- [XMC7200 Evaluation Kit](https://www.infineon.com/KIT_XMC72_EVK) (KIT_XMC72_EVK)
- [XMC7200 Evaluation Kit](https://www.infineon.com/KIT_XMC72_EVK) (KIT_XMC72_EVK_MUR_43439M2)
- [XMC7100 Evaluation Kit](https://www.infineon.com/KIT_XMC71_EVK_LITE_V1) (KIT_XMC71_EVK_LITE_V1)
- [AIROC™ CYW989820M2EVB-01 Evaluation kit](https://www.infineon.com/cms/en/product/wireless-connectivity/airoc-bluetooth-le-bluetooth-multiprotocol/airoc-bluetooth-le/cyw20829/)(CYW989820M2EVB-01)
- [CYW955913EVK-01 Wi-Fi Bluetooth&reg; Prototyping Kit (CYW955913EVK-01)](https://www.infineon.com/CYW955913EVK-01)

## 9. Hardware Setup

This example uses the board's default configuration. See the kit user guide to ensure the board is configured correctly.

**Note:** Before using this code example, make sure that the board is upgraded to KitProg3. The tool and instructions are available in the [Firmware Loader](https://github.com/Infineon/Firmware-loader) GitHub repository. If you do not upgrade, you will see an error like "unable to find CMSIS-DAP device" or "KitProg firmware is out of date".

## 10. Additional Information

- [OTA bootloader Abstraction Library RELEASE.md](./RELEASE.md)

- [OTA bootloader Abstraction Library version](./version.xml)

- [OTA bootloader Abstraction Library Makefile Information](./source/COMPONENT_MCUBOOT/MCUBOOT_OTA_MAKEFILE_INFO_README.md)

- [Connectivity utilities API documentation - for cy-log details](https://Infineon.github.io/connectivity-utilities/api_reference_manual/html/group__logging__utils.html)

- [ModusToolbox Software Environment, Quick Start Guide, Documentation, and Videos](https://www.infineon.com/modustoolbox)

- [ModusToolbox&trade; code examples](https://github.com/Infineon/Code-Examples-for-ModusToolbox-Software)

Infineon also provides a wealth of data at www.infineon.com to help you select the right device, and quickly and effectively integrate it into your design.

For PSoC™ 6 MCU devices, see [How to Design with PSoC 6 MCU - KBA223067](https://community.infineon.com/t5/Knowledge-Base-Articles/How-to-Design-with-PSoC-6-MCU-KBA223067/ta-p/248857) in the Infineon community.
