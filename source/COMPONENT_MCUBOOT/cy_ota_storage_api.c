/*
 * Copyright 2024, Cypress Semiconductor Corporation (an Infineon company) or
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
 * Cypress OTA Download Storage abstraction
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "cy_ota_api.h"
#include "cy_ota_internal.h"

#include "flash_map_backend.h"
#include "cy_ota_storage_api.h"
#include "cy_ota_flash.h"
#include "cy_ota_bootloader_abstraction_log.h"

/***********************************************************************
 *
 * defines & enums
 *
 * For more info on locations within slots, please see MCUBootApp
 * bootutils_misc.c, bootutils_private.h, bootutils.h
 *
 **********************************************************************/

/***********************************************************************
 *
 * Macros
 *
 **********************************************************************/

/***********************************************************************
 *
 * Structures
 *
 **********************************************************************/

/***********************************************************************
 *
 * Variables
 *
 **********************************************************************/

/***********************************************************************
 *
 * functions
 *
 **********************************************************************/
/**
 * @brief Initialize Storage area
 *
 * NOTE: Typically, this initializes Secondary Slot storage area
 *
 *
 * @return  CY_RSLT_SUCCESS
 *          CY_RSLT_TYPE_ERROR
 */
cy_rslt_t cy_ota_storage_init(void)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;

    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "%s()\n", __func__);
    result = cy_ota_mem_init();
    return result;
}

/**
 * @brief Open Storage area for download
 *
 * NOTE: Typically, this erases Secondary Slot
 *
 * @param[in]   storage_ptr   - Pointer to the OTA Agent storage context @ref cy_ota_storage_context_t
 *
 * @return  CY_RSLT_SUCCESS
 *          CY_RSLT_OTA_ERROR_OPEN_STORAGE
 */
cy_rslt_t cy_ota_storage_open(cy_ota_storage_context_t *storage_ptr)
{
    const struct flash_area *fap;

    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "%s()\n", __func__);
    if(storage_ptr == NULL)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "Storage context pointer for %s() is invalid\n", __func__);
        return CY_RSLT_OTA_ERROR_OPEN_STORAGE;
    }

    /* clear out the stats */
    storage_ptr->total_image_size    = 0;
    storage_ptr->total_bytes_written = 0;
    storage_ptr->last_offset         = 0;
    storage_ptr->last_size           = 0;
    storage_ptr->storage_loc         = NULL;

    if(flash_area_open(FLASH_AREA_IMAGE_SECONDARY(0), &fap) != 0)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "flash_area_open(FLASH_AREA_IMAGE_SECONDARY(0) ) failed\n");
        return CY_RSLT_OTA_ERROR_OPEN_STORAGE;
    }

    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_NOTICE, "Erase secondary image slot fap->fa_off: 0x%08lx, size: 0x%08lx\n", fap->fa_off, fap->fa_size);
    if(flash_area_erase(fap, 0, fap->fa_size) != 0)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "flash_area_erase(fap, 0) failed\r\n");
        return CY_RSLT_OTA_ERROR_OPEN_STORAGE;
    }

    storage_ptr->storage_loc = (void *)fap;

    return CY_RSLT_SUCCESS;
}

/**
 * @brief Read from storage area
 *
 * @param[in]       storage_ptr - Pointer to the OTA Agent storage context @ref cy_ota_storage_context_t
 * @param[in][out]  chunk_info  - Pointer to chunk information, buffer pointer used for the read
 *
 * @return  CY_RSLT_SUCCESS
 *          CY_RSLT_OTA_ERROR_READ_STORAGE
 */
cy_rslt_t cy_ota_storage_read(cy_ota_storage_context_t *storage_ptr, cy_ota_storage_read_info_t *chunk_info)
{
    const struct flash_area *fap;

    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "%s()\n", __func__);
    if(storage_ptr == NULL)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "Storage context pointer is invalid\n");
        return CY_RSLT_OTA_ERROR_READ_STORAGE;
    }

    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_INFO, "buf:%p len:%ld off: 0x%lx (%ld)\n",
                                      chunk_info->buffer, chunk_info->size,
                                      chunk_info->offset, chunk_info->offset);

    /* Always read from secondary slot */
    fap = (const struct flash_area *)storage_ptr->storage_loc;
    if(fap != NULL)
    {
        /* read into the chunk_info buffer */
        if(flash_area_read(fap, chunk_info->offset, chunk_info->buffer, chunk_info->size) != CY_RSLT_SUCCESS)
        {
            cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "flash_area_read() failed \n");
            return CY_RSLT_OTA_ERROR_READ_STORAGE;
        }
        return CY_RSLT_SUCCESS;
    }
    else
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "%s() flash_area invalid\r\n", __func__);
        return CY_RSLT_OTA_ERROR_READ_STORAGE;
    }
}

/**
 * @brief Close Storage area for download
 *
 * @param[in]   storage_ptr     Pointer to the OTA Agent storage context @ref cy_ota_storage_context_t
 *
 * @return  CY_RSLT_SUCCESS
 *          CY_RSLT_OTA_ERROR_CLOSE_STORAGE
 */
cy_rslt_t cy_ota_storage_close(cy_ota_storage_context_t *storage_ptr)
{
    const struct flash_area *fap;

    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "%s()\n", __func__);
    if(storage_ptr == NULL)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "Storage context pointer is invalid\n");
        return CY_RSLT_OTA_ERROR_CLOSE_STORAGE;
    }

    /* close secondary slot */
    fap = (const struct flash_area *)storage_ptr->storage_loc;
    if(fap == NULL)
    {
        return CY_RSLT_OTA_ERROR_CLOSE_STORAGE;
    }
    flash_area_close(fap);

    return CY_RSLT_SUCCESS;
}

/**
 * @brief Verify download signature
 *
 * @param[in]   storage_ptr     Pointer to the OTA Agent storage context @ref cy_ota_storage_context_t
 *
 * @return      CY_RSLT_SUCCESS
 *              CY_RSLT_OTA_ERROR_GENERAL
 */
cy_rslt_t cy_ota_storage_verify(cy_ota_storage_context_t *storage_ptr)
{
    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "%s()\n", __func__);
    if(storage_ptr == NULL)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "Storage context pointer is invalid\n");
        return CY_RSLT_OTA_ERROR_VERIFY;
    }

    if(flash_area_boot_set_pending(0, (storage_ptr->validate_after_reboot == 0)) != CY_RSLT_SUCCESS)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "VERIFY flash_area_boot_set_pending() Failed \n");
        return CY_RSLT_OTA_ERROR_VERIFY;
    }
    else
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "VERIFY flash_area_boot_set_pending() completed \n");
        return CY_RSLT_SUCCESS;
    }
}

/**
 * @brief App has validated the new OTA Image
 *
 * This call needs to be after reboot and MCUBoot has copied the new Application
 *      to the Primary Slot.
 *
 * @param   N/A
 *
 * @return  CY_RSLT_SUCCESS
 *          CY_RSLT_OTA_ERROR_GENERAL
 */
cy_rslt_t cy_ota_storage_image_validate(uint16_t app_id)
{
    /* Mark Image in Primary Slot as valid */
    cy_rslt_t               result = CY_RSLT_SUCCESS;

    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "%s()\n", __func__);
    (void)app_id;

#if !(defined (XMC7100) || defined (XMC7200))
    /* we copy this to a RAM buffer so that if we are running in XIP from external flash, the write routine won't fail */
    result = flash_area_boot_set_confirmed();
    if(result != CY_RSLT_SUCCESS)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "VERIFY flash_area_boot_set_confirmed() Failed \n");
        result = CY_RSLT_OTA_ERROR_GENERAL;
    }
#else
    /* For XMC7200 platform, Remove the Boot Magic from the Image in the Secondary slot to mark the Image in the primary as permanent. */
    if((flash_area_boot_unset_pending(0)) != 0)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "VALIDATE Primary Image: flash_area_boot_unset_pending() Failed \n");
        result = CY_RSLT_OTA_ERROR_GENERAL;
    }
#endif
    return result;
}

cy_rslt_t cy_ota_storage_get_app_info(void* file_des, cy_ota_app_info_t *app_info)
{
    cy_rslt_t result = CY_RSLT_OTA_ERROR_UNSUPPORTED;
    (void)(file_des);
    (void)(app_info);

    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "%s \n", __func__);

    return result;
}
