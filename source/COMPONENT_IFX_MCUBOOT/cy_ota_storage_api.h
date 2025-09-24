/*
 * Copyright 2025, Cypress Semiconductor Corporation (an Infineon company) or
 * an affiliate of Cypress Semiconductor Corporation.  All rights reserved.
 *
 * This software, including source code, documentation and related
 * materials ("Software") is owned by Cypress Semiconductor Corporation
 * or one of its affiliates ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products.  Any reproduction, modification, translation,
 * compilation, or representation of this Software except as specified
 * above is prohibited without the express written permission of Cypress.
 *
 * Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 * reserves the right to make changes to the Software without notice. Cypress
 * does not assume any liability arising out of the application or use of the
 * Software or any product or circuit described in the Software. Cypress does
 * not authorize its products for use in any products where a malfunction or
 * failure of the Cypress product may reasonably be expected to result in
 * significant property damage, injury or death ("High Risk Product"). By
 * including Cypress's product in a High Risk Product, the manufacturer
 * of such system or application assumes all risk of such use and in doing
 * so agrees to indemnify Cypress against all liability.
 */

/*
 *  Cypress OTA platform storage API header file
 */

/** @file
 *
 * Bootloader based OTA platform storage API header file.
 *
 */

#ifndef CY_OTA_STORAGE_API_H_
#define CY_OTA_STORAGE_API_H_

#include "cy_ota_api.h"

/**
 * \addtogroup group_ota_bootsupport Infineon OTA Bootloader Support API
 * \{
 * OTA Bootloader support for handling and installing firmware updates.
 *
 * \defgroup group_ota_bootsupport_macros OTA Bootloader Support Macros
 * Macros used to define the OTA Bootloader Support behavior.
 *
 * \defgroup group_ota_bootsupport_typedefs Bootloader Support Typedefs
 * Typedefs used by the OTA Bootloader Support.
 *
 * \defgroup group_ota_bootsupport_functions Bootloader Support Functions
 * Functions for handling and installing firmware updates.
 *
 */

/***********************************************************************
 *
 * defines & enums
 *
 **********************************************************************/
/**
 * \addtogroup group_ota_bootsupport_typedefs
 * \{
 */

/**
 * @brief Enumeration of MCUboot based image state.
 */
typedef enum
{
    CY_OTA_SLOT_STATE_NO_IMAGE = 0,   /**<  Corrupted or no image. */
    CY_OTA_SLOT_STATE_ACTIVE,         /**<  Confirmed Image. Active slot. */
    CY_OTA_SLOT_STATE_PENDING,        /**<  Boot in next reboot. */
    CY_OTA_SLOT_STATE_VERIFYING,      /**<  Image in slot being verified. */
    CY_OTA_SLOT_STATE_INACTIVE,       /**<  Verified image but in disabled slot. */
    CY_OTA_SLOT_STATE_UNKNOWN         /**<  Reserved. */
} cy_ota_slot_state_t;

/** \} group_ota_typedefs */

#define APP_INACTIVE_SLOT   (APP_ACTIVE_SLOT ^ 1)

/***********************************************************************
 *
 * Functions
 *
 **********************************************************************/
/**
 * \addtogroup group_ota_bootsupport_functions
 * \{
 * Bootloader based storage interface APIs for handling downloaded UPGRADE image of OTA application.
 * These callbacks are defined in ota-update library and expected to register these callbacks during OTA agent start.
 * ota-bootloader-abstraction library has implementation of these bootloader specific storage interface APIs.
 *
 */

/**
 * @brief Initialize Storage area
 *
 * NOTE: Typically, this initializes flash hardware and Application is expected to call this API
 *       once before any other flash operation.
 *
 * @return      CY_RSLT_SUCCESS
 *              CY_RSLT_TYPE_ERROR
 */
cy_rslt_t cy_ota_storage_init(void);

/**
 * @brief Open storage area for storing OTA UPGRADE images.
 *
 * NOTE: Typically, This erases UPGRADE Slots.
 *
 * @param[in]   storage_ptr    Pointer to the OTA Agent storage context 'cy_ota_storage_context_t'
 *
 * @return      CY_RSLT_SUCCESS
 *              CY_RSLT_OTA_ERROR_OPEN_STORAGE
 */
cy_rslt_t cy_ota_storage_open(cy_ota_storage_context_t *storage_ptr);

/**
 * @brief Read data from storage area
 *
 * @param[in]   storage_ptr  Pointer to the OTA Agent storage context 'cy_ota_storage_context_t'
 * @param[in]   chunk_info   Pointer to read chunk information, buffer pointer used for the read
 *
 * @return      CY_RSLT_SUCCESS
 *              CY_RSLT_OTA_ERROR_READ_STORAGE
 */
cy_rslt_t cy_ota_storage_read(cy_ota_storage_context_t *storage_ptr, cy_ota_storage_read_info_t *chunk_info);

/**
 * @brief Write data to configured storage area
 *
 * @param[in]   storage_ptr     Pointer to the OTA Agent storage context 'cy_ota_storage_context_t'
 * @param[in]   chunk_info      Pointer to write data chunk information
 *
 * @return      CY_UNTAR_SUCCESS
 *              CY_UNTAR_ERROR
 */
cy_rslt_t cy_ota_storage_write(cy_ota_storage_context_t *storage_ptr, cy_ota_storage_write_info_t *chunk_info);

/**
 * @brief Close Storage area for download
 *
 * @param[in]   storage_ptr     Pointer to the OTA Agent storage context 'cy_ota_storage_context_t'
 *
 * @return      CY_RSLT_SUCCESS
 *              CY_RSLT_OTA_ERROR_CLOSE_STORAGE
 */
cy_rslt_t cy_ota_storage_close(cy_ota_storage_context_t *storage_ptr);

/**
 * @brief Verify downloaded UPGRADE OTA image
 *
 * @param[in]   storage_ptr     Pointer to the OTA Agent storage context 'cy_ota_storage_context_t'
 *
 * @return      CY_RSLT_SUCCESS
 *              CY_RSLT_OTA_ERROR_GENERAL
 */
cy_rslt_t cy_ota_storage_verify(cy_ota_storage_context_t *storage_ptr);

/**
 * @brief Set boot pending
 *
 * Marks the image in the inactive slot as pending.
 * On the next reboot, the system will perform a one-time boot of the the inactive slot image.
 *
 * @param[in]   app_id          Application ID.
 * @param[in]   storage_ptr     Pointer to the OTA Agent storage context @ref cy_ota_storage_context_t
 *
 * @return      CY_RSLT_SUCCESS
 *              CY_RSLT_OTA_ERROR_GENERAL
 */
cy_rslt_t cy_ota_storage_set_boot_pending(uint16_t app_id, cy_ota_storage_context_t *storage_ptr);

/**
 * @brief Get boot pending status of the image in inactive slot
 *
 * @param[in]   app_id          Application ID.
 * @param[out]  status          Pointer to the status variable to store boot pending status.
 *                              CY_MCUBOOT_SWAP_TYPE_NONE(1)
 *                              CY_MCUBOOT_SWAP_TYPE_TEST(2)
 *                              CY_MCUBOOT_SWAP_TYPE_PERM(3)
 *                              CY_MCUBOOT_SWAP_TYPE_REVERT(4)
 *
 * @return      CY_RSLT_SUCCESS
 *              CY_RSLT_OTA_ERROR_GENERAL
 */
cy_rslt_t cy_ota_storage_get_boot_pending_status(uint16_t app_id, uint8_t *status);

/**
 * @brief Application has validated the new OTA Image in active slot and committing it.
 *
 * This call needs to be after reboot and Bootloader has started the upgrade version of Application.
 *
 * @param[in]   app_id          Application ID.
 *
 * @return      CY_RSLT_SUCCESS
 *              CY_RSLT_OTA_ERROR_GENERAL
 */
cy_rslt_t cy_ota_storage_image_validate(uint16_t app_id);

/**
 * @brief Get validate/commit status of the OTA Image in active slot.
 *
 * This call needs to be after reboot and Bootloader has started the upgrade version of Application.
 *
 * @param[in]   app_id          Application ID.
 * @param[out]  status          Pointer to the status variable to store image validate status.
 *                              CY_MCUBOOT_FLAG_SET(1)
 *                              CY_MCUBOOT_FLAG_BAD(2)
 *                              CY_MCUBOOT_FLAG_UNSET(3)
 *                              CY_MCUBOOT_FLAG_ANY(4)
 *
 * @return      CY_RSLT_SUCCESS
 *              CY_RSLT_OTA_ERROR_GENERAL
 */
cy_rslt_t cy_ota_storage_get_image_validate_status(uint16_t app_id, uint8_t *status);

/**
 * @brief Get Application image information
 *
 * This call needs to be after reboot and Bootloader has started the upgrade version of Application.
 *
 * @param[in]        slot_id         Memory slot ID.
 * @param[in]        image_num       Image number.
 * @param[out]       app_info        Pointer to the OTA Application information structure.
 *
 * @return  CY_RSLT_SUCCESS
 *          CY_RSLT_OTA_ERROR_GENERAL
 */
cy_rslt_t cy_ota_storage_get_app_info(uint16_t slot_id, uint16_t image_num, cy_ota_app_info_t *app_info);

/**
 * @brief Get Application slot state information
 *
 * @param[in]        slot_id         Slot ID.
 * @param[in]        image_num       Image number.
 * @param[out]       state           Pointer to the OTA Application state variable.
 *
 * @return  CY_RSLT_SUCCESS
 *          CY_RSLT_OTA_ERROR_GENERAL
 */
cy_rslt_t cy_ota_storage_get_slot_state(uint16_t slot_id, uint16_t image_num, cy_ota_slot_state_t *state);

/**
 * @brief Set Application slot state information
 *
 * @param[in]        slot_id         Slot ID.
 * @param[in]        image_num       Image number.
 * @param[out]       state           OTA Application state variable.
 *
 * @return  CY_RSLT_SUCCESS
 *          CY_RSLT_OTA_ERROR_GENERAL
 */
cy_rslt_t cy_ota_storage_set_slot_state(uint16_t slot_id, uint16_t image_num, cy_ota_slot_state_t state);


/** \} group_ota_bootsupport_functions */

/** \} group_ota_bootsupport */

#endif /* CY_OTA_STORAGE_API_H_ */
