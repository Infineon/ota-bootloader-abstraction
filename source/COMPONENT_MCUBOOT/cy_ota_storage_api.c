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
 * Cypress OTA Download Storage abstraction
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "cy_ota_api.h"
#include "cy_ota_internal.h"

#include "cy_flash_map_backend.h"
#include "cy_ota_storage_api.h"
#include "cy_ota_flash.h"
#include "cy_ota_bootloader_abstraction_log.h"

#if (defined (CY_OTA_IMAGE_VERIFICATION) || defined (CY_OTA_DIRECT_XIP))
#include "bootutil/bootutil.h"
#endif

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
 * NOTE: Typically, this initializes Secondary/Inactive Slot storage area
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
    uint8_t slot_id = 0;

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

    slot_id = CY_FLASH_UPGRADE_AREA(APP_INACTIVE_SLOT, 0);
    if(cy_flash_area_open(slot_id, &fap) != 0)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "cy_flash_area_open failed\n");
        return CY_RSLT_OTA_ERROR_OPEN_STORAGE;
    }

    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_NOTICE, "Erase secondary image slot fap->fa_off: 0x%08lx, size: 0x%08lx\n", fap->fa_off, fap->fa_size);
    if(cy_flash_area_erase(fap, 0, fap->fa_size) != 0)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "cy_flash_area_erase(fap, 0) failed\r\n");
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
        if(cy_flash_area_read(fap, chunk_info->offset, chunk_info->buffer, chunk_info->size) != CY_RSLT_SUCCESS)
        {
            cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "cy_flash_area_read() failed \n");
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
    cy_flash_area_close(fap);

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

    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_NOTICE, "Start boot_validate_slot_for_image_id() ... \n");

#ifdef CY_OTA_IMAGE_VERIFICATION
    fih_int fih_rc = 0;
    FIH_CALL(boot_validate_slot_for_image_id, fih_rc, 0, APP_INACTIVE_SLOT);
    if(fih_rc != 0)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "boot_validate_slot_for_image_id failed\n");
        return CY_RSLT_OTA_ERROR_VERIFY;
    }
    else
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_NOTICE, "boot_validate_slot_for_image_id() completed \n");
        return CY_RSLT_SUCCESS;
    }
#else
    if(cy_flash_area_boot_set_pending(0, (storage_ptr->validate_after_reboot == 0)) != CY_RSLT_SUCCESS)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "VERIFY cy_flash_area_boot_set_pending() Failed \n");
        return CY_RSLT_OTA_ERROR_VERIFY;
    }
    else
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "VERIFY cy_flash_area_boot_set_pending() completed \n");
        return CY_RSLT_SUCCESS;
    }
#endif
}

/**
 * @brief Erase slot
 *
 * @param[in]   slot_id
 * @param[in]   offset
 * @param[in]   size
 *
 * @return  CY_RSLT_SUCCESS
 *          CY_RSLT_OTA_ERROR_GENERAL
 */
#if (defined (CY_OTA_IMAGE_VERIFICATION) || defined (CY_OTA_DIRECT_XIP))
static cy_rslt_t cy_ota_storage_erase(uint16_t slot_id, uint32_t offset, uint32_t size)
{
    const struct flash_area *fap;

    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "%s()\n", __func__);

    if(cy_flash_area_open(slot_id, &fap) != 0)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "cy_flash_area_open failed\n");
        return CY_RSLT_OTA_ERROR_OPEN_STORAGE;
    }

    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_NOTICE, "Erase secondary image slot fap->fa_off: 0x%08lx, size: 0x%08lx\n", fap->fa_off, fap->fa_size);
    if(cy_flash_area_erase(fap, offset, size) != 0)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "cy_flash_area_erase(fap, 0) failed\r\n");
        return CY_RSLT_OTA_ERROR_GENERAL;
    }

    return CY_RSLT_SUCCESS;
}
#endif /* (CY_OTA_IMAGE_VERIFICATION) || (CY_OTA_DIRECT_XIP) */

/**
 * @brief Set boot pending
 *
 * @param[in]   storage_ptr     Pointer to the OTA Agent storage context @ref cy_ota_storage_context_t
 *
 * @return      CY_RSLT_SUCCESS
 *              CY_RSLT_OTA_ERROR_GENERAL
 */
cy_rslt_t cy_ota_storage_set_boot_pending(cy_ota_storage_context_t *storage_ptr)
{
    int rc = 0;
    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "%s()\n", __func__);
    if(storage_ptr == NULL)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "Storage context pointer is invalid\n");
        return CY_RSLT_OTA_ERROR_VERIFY;
    }

    rc = cy_flash_area_boot_set_pending(0, (storage_ptr->validate_after_reboot == 0));
    if(rc != 0)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "cy_flash_area_boot_set_pending() Failed \n");
        return CY_RSLT_OTA_ERROR_VERIFY;
    }
    else
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "cy_flash_area_boot_set_pending() completed \n");
        return CY_RSLT_SUCCESS;
    }
}

cy_rslt_t cy_ota_storage_get_boot_pending_status(uint16_t app_id, uint8_t *status)
{
    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "%s()\n", __func__);
    (void)app_id;

    if(cy_flash_area_boot_get_pending_status(status) != 0)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "cy_flash_area_boot_get_pending_status() Failed \n");
        return CY_RSLT_OTA_ERROR_GENERAL;
    }
    else
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "Secondary slot image boot pending status : %d \n", *status);
    }
    return CY_RSLT_SUCCESS;
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
    result = cy_flash_area_boot_set_confirmed(0);
    if(result != CY_RSLT_SUCCESS)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "cy_flash_area_boot_set_confirmed() Failed \n");
        return CY_RSLT_OTA_ERROR_GENERAL;
    }
#else
    /* Remove the Boot Magic from the Image in the Secondary slot to mark the Image in the primary as permanent. */
    if((cy_flash_area_boot_unset_pending(0)) != 0)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "Removing image header from secondary slot Failed \n");
        return CY_RSLT_OTA_ERROR_GENERAL;
    }
#endif
    return result;
}

cy_rslt_t cy_ota_storage_get_image_validate_status(uint16_t app_id, uint8_t *status)
{
    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "%s()\n", __func__);
    (void)app_id;

    if(cy_flash_area_boot_get_image_confirm_status(status) != 0)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "cy_flash_area_boot_get_image_confirm_status() Failed \n");
        return CY_RSLT_OTA_ERROR_GENERAL;
    }
    else
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "Primary slot image validate status : %d \n", *status);
    }
    return CY_RSLT_SUCCESS;
}

cy_rslt_t cy_ota_storage_get_app_info(uint16_t slot_id, uint16_t image_num, cy_ota_app_info_t *app_info)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;

    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "%s \n", __func__);

#if (defined (CY_OTA_IMAGE_VERIFICATION) || defined (CY_OTA_DIRECT_XIP))
    const struct flash_area *fap;
    struct image_version img_ver;
    uint8_t *buffer = NULL;
    uint16_t len = 0;
    uint32_t off = 0;
    int rc = 0;

    memset(&img_ver, 0x00, sizeof(img_ver));
    rc = boot_get_image_version(image_num, slot_id, &img_ver);
    if(rc == 0)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR,"Image version %x.%x.%x\r\n", img_ver.iv_major, img_ver.iv_minor, img_ver.iv_build_num);
    }
    else
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "boot_get_image_version failed\n");
        return CY_RSLT_OTA_ERROR_NO_IMAGE_INFO;
    }

    app_info->app_id = image_num;
    app_info->major = img_ver.iv_major;
    app_info->minor = img_ver.iv_minor;
    app_info->build = img_ver.iv_build_num;
    app_info->revision = (img_ver.iv_revision >> 8);
    app_info->slot = (img_ver.iv_revision & 0xFF);

    /* Retrieve Company ID */
    rc = boot_find_image_tlv_info(image_num, slot_id, CY_TLV_INDEX_COMPANY_ID, &len, &off);
    if(rc == 0)
    {
        if(cy_flash_area_open(CY_FLASH_UPGRADE_AREA(slot_id, 0), &fap) != 0)
        {
            cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "cy_flash_area_open failed\n");
            return CY_RSLT_OTA_ERROR_OPEN_STORAGE;
        }

        buffer = (uint8_t *)malloc(len);
        if(buffer == NULL)
        {
            return CY_RSLT_OTA_ERROR_OUT_OF_MEMORY;
        }
        memset(buffer, 0x00, len);

        /* read TLV info into the buffer */
        result = cy_flash_area_read(fap, off, (void *)buffer, len);
        if(result != CY_RSLT_SUCCESS)
        {
            cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "cy_flash_area_read() failed with error 0x%lx for Company ID with offset : 0x%lx\n", off, (uint32_t)result);
            result = CY_RSLT_OTA_ERROR_READ_STORAGE;
        }
        else
        {
            app_info->company_id = (uint16_t)((buffer[0] << 8) | buffer[1]);
        }
        free(buffer);
    }
    else
    {
        app_info->company_id = 0xFFFF;
    }

    /* Retrieve Product ID */
    rc = boot_find_image_tlv_info(image_num, slot_id, CY_TLV_INDEX_PRODUCT_ID, &len, &off);
    if(rc == 0)
    {
        if(cy_flash_area_open(CY_FLASH_UPGRADE_AREA(slot_id, 0), &fap) != 0)
        {
            cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "cy_flash_area_open failed\n");
            return CY_RSLT_OTA_ERROR_OPEN_STORAGE;
        }

        buffer = (uint8_t *)malloc(len);
        if(buffer == NULL)
        {
            return CY_RSLT_OTA_ERROR_OUT_OF_MEMORY;
        }
        memset(buffer, 0x00, len);

        /* read TLV info into the buffer */
        result = cy_flash_area_read(fap, off, (void *)buffer, len);
        if(result != CY_RSLT_SUCCESS)
        {
            cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "cy_flash_area_read() failed with error 0x%lx for Product ID with offset : 0x%lx\n", off, (uint32_t)result);
            result = CY_RSLT_OTA_ERROR_READ_STORAGE;
        }
        else
        {
            app_info->product_id = (uint16_t)((buffer[0] << 8) | buffer[1]);
        }
        free(buffer);
    }
    else
    {
        app_info->product_id = 0xFFFF;
    }
#endif /* (CY_OTA_IMAGE_VERIFICATION) || (CY_OTA_DIRECT_XIP) */

    return result;
}

cy_rslt_t cy_ota_storage_get_slot_state(uint16_t slot_id, uint16_t image_num, cy_ota_slot_state_t *state)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;

    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "%s \n", __func__);

#if (defined (CY_OTA_IMAGE_VERIFICATION) || defined (CY_OTA_DIRECT_XIP))
    int rc = 0;
    boot_slot_state_t slot_state;

    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "%s \n", __func__);
    rc = boot_get_image_state(image_num, slot_id, &slot_state);
    if(rc == 0)
    {
        switch(slot_state)
        {
            case MCUBOOT_SLOT_STATE_ACTIVE :
                *state = CY_OTA_SLOT_STATE_ACTIVE;
                break;

            case MCUBOOT_SLOT_STATE_NO_IMAGE :
                *state = CY_OTA_SLOT_STATE_NO_IMAGE;
                break;

            case MCUBOOT_SLOT_STATE_PENDING :
                *state = CY_OTA_SLOT_STATE_PENDING;
                break;

            case MCUBOOT_SLOT_STATE_VERIFYING :
                *state = CY_OTA_SLOT_STATE_VERIFYING;
                break;

            case MCUBOOT_SLOT_STATE_INACTIVE :
                *state = CY_OTA_SLOT_STATE_INACTIVE;
                break;

            default :
                result = CY_RSLT_OTA_ERROR_UNSUPPORTED;
                break;
        }
    }
    else
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "boot_get_image_state() failed with error %d", rc);
        result = CY_RSLT_OTA_ERROR_GENERAL;
    }
#endif /* (CY_OTA_IMAGE_VERIFICATION) || (CY_OTA_DIRECT_XIP) */

    return result;
}

#if (defined (CY_OTA_IMAGE_VERIFICATION) || defined (CY_OTA_DIRECT_XIP))
static bool cy_ota_storage_is_state_transition_allowed(cy_ota_slot_state_t active_slot_state,
                                                       cy_ota_slot_state_t alt_slot_state,
                                                       uint16_t slot_id, cy_ota_slot_state_t state)
{
    if(slot_id == APP_ACTIVE_SLOT)
    {
        return false;
    }

    else if(((slot_id == APP_INACTIVE_SLOT) &&
        ((state == CY_OTA_SLOT_STATE_NO_IMAGE)||
         (state == CY_OTA_SLOT_STATE_INACTIVE)||
         (state == CY_OTA_SLOT_STATE_PENDING))))
    {
        if(alt_slot_state == CY_OTA_SLOT_STATE_NO_IMAGE)
        {
            return false;
        }
        else
        {
            return true;
        }
    }
    else
    {
        return false;
    }
}
#endif /* (CY_OTA_IMAGE_VERIFICATION) || (CY_OTA_DIRECT_XIP) */

cy_rslt_t cy_ota_storage_set_slot_state(uint16_t slot_id, uint16_t image_num, cy_ota_slot_state_t state)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;

    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "%s \n", __func__);

#if (defined (CY_OTA_IMAGE_VERIFICATION) || defined (CY_OTA_DIRECT_XIP))
    cy_ota_slot_state_t active_slot_state = CY_OTA_SLOT_STATE_UNKNOWN, alt_slot_state = CY_OTA_SLOT_STATE_UNKNOWN;
    int rc = 0;

    if(slot_id == APP_ACTIVE_SLOT)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "Requested image state transition not allowed");
        return CY_RSLT_OTA_ERROR_GENERAL;
    }

    result = cy_ota_storage_get_slot_state(APP_INACTIVE_SLOT, image_num, &alt_slot_state);
    if(result != CY_RSLT_SUCCESS)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "Slot : %d cy_ota_storage_get_slot_state() failed with error 0x%lx", APP_INACTIVE_SLOT, result);
        return result;
    }
    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_INFO, "Slot %d current state - %d", APP_INACTIVE_SLOT, (uint8_t)alt_slot_state);

    result = cy_ota_storage_get_slot_state(APP_ACTIVE_SLOT, image_num, &active_slot_state);
    if(result != CY_RSLT_SUCCESS)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "Slot : %d cy_ota_storage_get_slot_state() failed with error 0x%lx", APP_ACTIVE_SLOT, result);
        return result;
    }
    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_INFO, "Slot %d current state - %d", APP_ACTIVE_SLOT, (uint8_t)active_slot_state);

    (void)rc;

    if(cy_ota_storage_is_state_transition_allowed(active_slot_state, alt_slot_state, slot_id, state))
    {
        switch(state)
        {
            case CY_OTA_SLOT_STATE_ACTIVE :
                /* Set Image OK */
                rc = cy_flash_area_boot_set_confirmed(image_num);
                if(rc != 0)
                {
                    result = CY_RSLT_OTA_ERROR_GENERAL;
                }
                break;

            case CY_OTA_SLOT_STATE_INACTIVE :
                rc = boot_set_inactive_slot(image_num, slot_id);
                if(rc != 0)
                {
                    result = CY_RSLT_OTA_ERROR_GENERAL;
                }
                break;

            case CY_OTA_SLOT_STATE_PENDING :
                rc = boot_set_pending_slot(image_num, slot_id);
                if(rc != 0)
                {
                    result = CY_RSLT_OTA_ERROR_GENERAL;
                }
                break;

            case CY_OTA_SLOT_STATE_VERIFYING :
                break;

            case CY_OTA_SLOT_STATE_NO_IMAGE :
                result = cy_ota_storage_erase(slot_id, 0, 0x1000);
                break;

            default :
                result = CY_RSLT_OTA_ERROR_UNSUPPORTED;
                break;
        }
    }
    else
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "Requested image state transition not allowed");
        result = CY_RSLT_OTA_ERROR_GENERAL;
    }
#endif /* (CY_OTA_IMAGE_VERIFICATION) || (CY_OTA_DIRECT_XIP) */

    return result;
}
