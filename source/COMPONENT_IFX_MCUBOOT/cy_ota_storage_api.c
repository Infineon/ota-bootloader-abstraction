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
#include "cy_ota_untar.h"

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
/**
 * @brief Tarball support file types recognized in components.json file
 *
 * The file type in the tarball
 */
#define CY_FILE_TYPE_SPE        "SPE"       /**< Secure Programming Environment (TFM) code type                 */
#define CY_FILE_TYPE_NSPE       "NSPE"      /**< Non-Secure Programming Environment (application) code type     */

#ifndef CY_FLASH_SECTOR_SIZE
#define CY_FLASH_SECTOR_SIZE    0x40000UL   /**< Sector Size                                */
#endif

/***********************************************************************
 *
 * Structures
 *
 **********************************************************************/
/**
 * @brief Structure for tracking upgrade slot.
 */
typedef struct cy_ota_storage_erase_info
{
    uint8_t image_num;
    uint32_t total_sectors;
    uint32_t erased_sectors;
    uint32_t erase_offset;
    bool erased_complete;
} cy_ota_storage_erase_info_t;

/**
 * @brief Structure for handling TAR Header for MTU Sizes less than 512.
 */
typedef struct update_file_header
{
    uint8_t *buffer;            /** Temporary Buffer to handle TAR Header. */
    uint32_t buffer_size;       /** Size of data in temporary Buffer. */
    bool is_tar_header_checked; /** Indicates if the TAR header check is completed. */
} cy_ota_tar_file_header_t;

/***********************************************************************
 *
 * Variables
 *
 **********************************************************************/
/**
 * @brief Context structure for parsing the tar file header.
 */
static cy_ota_tar_file_header_t file_header;

/**
 * @brief Structure for tracking upgrade slot erase status.
 */
static cy_ota_storage_erase_info_t slot_erase_info[MCUBOOT_IMAGE_NUMBER];

/**
 * @brief Context structure for handling the tar file.
 */
static cy_untar_context_t  ota_untar_context;

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
 * @brief callback to handle tar data
 *
 * @param ctxt          untar context
 * @param file_index    index into ctxt->files for the data
 * @param buffer        data to use
 * @param file_offset   offset into the file to store data
 * @param chunk_size    amount of data in buffer to use
 * @param cb_arg        argument passed into initialization
 *
 * return   CY_UNTAR_SUCCESS
 *          CY_UNTAR_ERROR
 */
static cy_untar_result_t ota_untar_write_callback(cy_untar_context_ptr ctxt, uint16_t file_index,
                                                  uint8_t *buffer, uint32_t file_offset,
                                                  uint32_t chunk_size, void *cb_arg)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;
    uint16_t image_index = 0;
    const struct flash_area *fap;
    cy_ota_storage_context_t *storage_ptr = (cy_ota_storage_context_t *)cb_arg;

    (void)image_index;
    if((ctxt == NULL) || (buffer == NULL) || (storage_ptr == NULL))
    {
        return CY_UNTAR_ERROR;
    }

#if !CY_OTA_DIRECT_XIP
    image_index = (ctxt->files[ctxt->current_file].img_id - 1);
#endif

    if((file_offset + chunk_size) > ctxt->files[file_index].size)
    {
        chunk_size = ctxt->files[file_index].size - file_offset;
    }

    if(ctxt->files[ctxt->current_file].is_valid_img == true)
    {
#if CY_OTA_DIRECT_XIP
        if(cy_flash_area_open(CY_FLASH_UPGRADE_AREA(APP_INACTIVE_SLOT, image), &fap) != 0)
#else
        if(cy_flash_area_open(CY_FLASH_AREA_IMAGE_SECONDARY(image_index), &fap) != 0)
#endif
        {
            cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "%s() cy_flash_area_open(%d) failed\n", __func__, image_index);
            return CY_UNTAR_ERROR;
        }

        if(slot_erase_info[image_index].erased_complete == false)
        {
            cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "Erase secondary image slot offset: 0x%08lx, size: 0x%08lx\n", slot_erase_info[image_index].erase_offset, CY_FLASH_SECTOR_SIZE);
            if(cy_flash_area_erase(fap, slot_erase_info[image_index].erase_offset, CY_FLASH_SECTOR_SIZE) != 0)
            {
                cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "cy_flash_area_erase(fap, %d) failed\r\n", image_index);
                return CY_UNTAR_ERROR;
            }
            else
            {
                slot_erase_info[image_index].erased_sectors++;
                slot_erase_info[image_index].erase_offset = ((slot_erase_info[image_index].erased_sectors) * CY_FLASH_SECTOR_SIZE);
                if(slot_erase_info[image_index].erased_sectors == slot_erase_info[image_index].total_sectors)
                {
                    slot_erase_info[image_index].erased_complete = true;
                }
            }
        }

        if(cy_flash_area_write(fap, file_offset, buffer, chunk_size) != 0)
        {
            result = CY_RSLT_OTA_ERROR_WRITE_STORAGE;
        }

        if(result != CY_RSLT_SUCCESS)
        {
            cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "%s() failed\n", __func__);
            cy_flash_area_close(fap);
            return CY_UNTAR_ERROR;
        }

        cy_flash_area_close(fap);
    }
    else
    {
        printf("\nImage Need not be updated, Not writing to Flash!!!!!");
    }

    return CY_UNTAR_SUCCESS;
}

/**
 * @brief Initialization routine for handling tarball OTA file
 *
 * @param ctx_ptr               Pointer to OTA agent context
 * @param ctx_untar[in,out]     pointer to untar context to be initialized
 *
 * return   CY_UNTAR_SUCCESS
 *          CY_UNTAR_ERROR
 */
static cy_untar_result_t cy_ota_untar_init_context(cy_ota_storage_context_t *storage_ptr, cy_untar_context_t* ctx_untar )
{
    if(cy_untar_init( ctx_untar, ota_untar_write_callback, storage_ptr ) == CY_RSLT_SUCCESS)
    {
        storage_ptr->ota_is_tar_archive  = 1;
        return CY_UNTAR_SUCCESS;
    }
    return CY_UNTAR_ERROR;
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

    for(int i = 0; i < MCUBOOT_IMAGE_NUMBER; i++)
    {
#if CY_OTA_DIRECT_XIP
        if(cy_flash_area_open(CY_FLASH_UPGRADE_AREA(APP_INACTIVE_SLOT, 0), &fap) != 0)
#else
        if(cy_flash_area_open(CY_FLASH_AREA_IMAGE_SECONDARY(i), &fap) != 0)
#endif
        {
            cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "cy_flash_area_open failed\n");
            return CY_RSLT_OTA_ERROR_OPEN_STORAGE;
        }

        slot_erase_info[i].image_num = i;
        slot_erase_info[i].total_sectors = (fap->fa_size)/CY_FLASH_SECTOR_SIZE;
        slot_erase_info[i].erased_sectors = 0;
        slot_erase_info[i].erased_complete = false;
        slot_erase_info[i].erase_offset = 0;
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
 * @brief Determine if tar or non-tar and call correct write function
 *
 * @param[in]   storage_ptr     Pointer to the OTA Agent storage context @ref cy_ota_storage_context_t
 * @param[in]   chunk_info      Pointer to chunk information
 *
 * @return  CY_UNTAR_SUCCESS
 *          CY_UNTAR_ERROR
 */
cy_rslt_t cy_ota_storage_write(cy_ota_storage_context_t *storage_ptr, cy_ota_storage_write_info_t * const chunk_info)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;
    uint16_t copy_offset = 0;

    if((storage_ptr == NULL) || (chunk_info == NULL))
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "Storage context pointer for %s() is invalid\n", __func__);
        return CY_RSLT_OTA_ERROR_WRITE_STORAGE;
    }

    if(chunk_info->offset == 0UL)
    {
        file_header.is_tar_header_checked = false;
        file_header.buffer_size = 0;
    }

    if(!file_header.is_tar_header_checked)
    {
        /* we need to check some things when we receive the first 512 Bytes data */
        if((chunk_info->offset == 0UL) && (chunk_info->size >= TAR_BLOCK_SIZE))
        {
            /*
             * Check for incoming tarball (as opposed to a single file OTA)
             */
            if(cy_is_tar_header(chunk_info->buffer, chunk_info->size) == CY_UNTAR_SUCCESS)
            {
                cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_NOTICE, "%d:%s() TAR ARCHIVE\n", __LINE__, __func__);
                if(cy_ota_untar_init_context(storage_ptr, &ota_untar_context) != CY_RSLT_SUCCESS)
                {
                    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "%s() cy_ota_untar_init_context() FAILED! \n", __func__);
                    return CY_RSLT_OTA_ERROR_WRITE_STORAGE;
                }
            }
            else
            {
                cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_NOTICE, "%d:%s() Non TAR file\n", __LINE__, __func__);
            }
            file_header.is_tar_header_checked = true;
        }
        else
        {
            if(file_header.buffer == NULL)
            {
                file_header.buffer = (uint8_t *)malloc(TAR_BLOCK_SIZE);
                if(file_header.buffer == NULL)
                {
                    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "%s() malloc() for file_header Buffer FAILED! \n", __func__);
                    return CY_RSLT_OTA_ERROR_WRITE_STORAGE;
                }
                memset(file_header.buffer, 0x00, TAR_BLOCK_SIZE);
            }

            if((file_header.buffer_size + chunk_info->size) < TAR_BLOCK_SIZE)
            {
                memcpy((file_header.buffer + file_header.buffer_size), chunk_info->buffer, (chunk_info->size));
                file_header.buffer_size += chunk_info->size;

                return CY_RSLT_SUCCESS;
            }
            else
            {
                copy_offset = TAR_BLOCK_SIZE - file_header.buffer_size;
                memcpy((file_header.buffer + file_header.buffer_size), chunk_info->buffer, copy_offset);
                file_header.buffer_size = TAR_BLOCK_SIZE;

                chunk_info->offset = TAR_BLOCK_SIZE;
                chunk_info->size -= copy_offset;
            }

            if(cy_is_tar_header(file_header.buffer, file_header.buffer_size) == CY_UNTAR_SUCCESS)
            {
                cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "%d:%s() TAR ARCHIVE\n", __LINE__, __func__);
                if(cy_ota_untar_init_context(storage_ptr, &ota_untar_context) != CY_RSLT_SUCCESS)
                {
                    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "%s() cy_ota_untar_init_context() FAILED! \n", __func__);
                    return CY_RSLT_OTA_ERROR_WRITE_STORAGE;
                }
            }
            else
            {
                cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "%d:%s() Non TAR\n", __LINE__, __func__);
            }
            file_header.is_tar_header_checked = true;
        }
    }

    /* treat a tar file differently from a "normal" OTA */
    if(storage_ptr->ota_is_tar_archive != 0)
    {
        uint32_t consumed = 0;
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "%d:%s() TAR ARCHIVE\n", __LINE__, __func__);

        if(file_header.buffer_size)
        {
            cy_untar_result_t result;

            while (consumed < file_header.buffer_size)
            {
                result = cy_untar_parse(&ota_untar_context, (consumed), (file_header.buffer + consumed),
                                         (file_header.buffer_size - consumed), &consumed);
                if((result == CY_UNTAR_ERROR) || (result == CY_UNTAR_INVALID))
                {
                    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "%s() cy_untar_parse() FAIL consumed: %ld sz:%ld result:%ld)!\n", __func__, consumed, chunk_info->size, result);
                    return CY_RSLT_OTA_ERROR_WRITE_STORAGE;
                }
                /* Yield for a bit */
                cy_rtos_delay_milliseconds(1);
            }
            free(file_header.buffer);
            file_header.buffer = NULL;
            file_header.buffer_size = 0;

            consumed = 0;
        }

        while (consumed < chunk_info->size)
        {
            cy_untar_result_t result;
            result = cy_untar_parse(&ota_untar_context, (chunk_info->offset + consumed), &chunk_info->buffer[consumed + copy_offset],
                                    (chunk_info->size - consumed), &consumed);
            if((result == CY_UNTAR_ERROR) || (result == CY_UNTAR_INVALID))
            {
                cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "%s() cy_untar_parse() FAIL consumed: %ld sz:%ld result:%ld)!\n", __func__, consumed, chunk_info->size, result);
                return CY_RSLT_OTA_ERROR_WRITE_STORAGE;
            }

            /* Yield for a bit */
            cy_rtos_delay_milliseconds(1);
        }

        /* with the tarball we get a version - check if it is > current so we can bail early */
#ifdef CY_TEST_APP_VERSION_IN_TAR
        if(ota_untar_context.version[0] != 0)
        {
            /* example version string "<major>.<minor>.<build>" */
            uint16_t major = 0;
            uint16_t minor = 0;
            uint16_t build = 0;
            char *dot;
            major = atoi(ota_untar_context.version);
            dot = strstr(ota_untar_context.version, ".");
            if(dot != NULL)
            {
                dot++;
                minor = atoi(dot);
                dot = strstr(dot, ".");
                if(dot != NULL)
                {
                    dot++;
                    build = atoi(dot);

                    if((major < APP_VERSION_MAJOR) ||
                          ( (major == APP_VERSION_MAJOR) &&
                            (minor < APP_VERSION_MINOR)) ||
                          ( (major == APP_VERSION_MAJOR) &&
                            (minor == APP_VERSION_MINOR) &&
                            (build <= APP_VERSION_BUILD)))
                    {
                         cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "%s() OTA image version %d.%d.%d <= current %d.%d.%d-- bail!\r\n", __func__,
                                 major, minor, build,
                                 APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_VERSION_BUILD);

                         return CY_RSLT_OTA_ERROR_WRITE_STORAGE;
                    }
                }
            }
        }
#endif  /* CY_TEST_APP_VERSION_IN_TAR */
    }
    else
    {
        /* non-tarball OTA here, always image 0x00 */
        const struct flash_area *fap;

        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "%s() NON-TAR file\n", __func__);
#if CY_OTA_DIRECT_XIP
        if(cy_flash_area_open(CY_FLASH_UPGRADE_AREA(APP_INACTIVE_SLOT, image), &fap) != 0)
#else
        if(cy_flash_area_open(CY_FLASH_AREA_IMAGE_SECONDARY(0), &fap) != 0)
#endif
        {
            cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "%s() cy_flash_area_open()\n", __func__);
            return CY_RSLT_OTA_ERROR_WRITE_STORAGE;
        }

        if(file_header.buffer_size)
        {
            if(cy_flash_area_write(fap, 0, file_header.buffer, file_header.buffer_size) != 0)
            {
                result = CY_RSLT_OTA_ERROR_WRITE_STORAGE;
            }

            free(file_header.buffer);
            file_header.buffer = NULL;
            file_header.buffer_size = 0;

            if(result != CY_RSLT_SUCCESS)
            {
                cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "%s() WRITE FAILED\n", __func__);
                return CY_RSLT_OTA_ERROR_WRITE_STORAGE;
            }
        }

        if(cy_flash_area_write(fap, chunk_info->offset, (chunk_info->buffer + copy_offset), chunk_info->size) != 0)
        {
            result = CY_RSLT_OTA_ERROR_WRITE_STORAGE;
        }

        if(result != CY_RSLT_SUCCESS)
        {
            cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "%s() WRITE FAILED\n", __func__);
            return CY_RSLT_OTA_ERROR_WRITE_STORAGE;
        }

        cy_flash_area_close( fap);
    }

    return CY_RSLT_SUCCESS;
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
    if(cy_flash_area_boot_set_pending(storage_ptr->imgID, (storage_ptr->validate_after_reboot == 0)) != CY_RSLT_SUCCESS)
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
cy_rslt_t cy_ota_storage_set_boot_pending(uint16_t app_id, cy_ota_storage_context_t *storage_ptr)
{
    int rc = 0;
    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "%s()\n", __func__);
    if(storage_ptr == NULL)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "Storage context pointer is invalid\n");
        return CY_RSLT_OTA_ERROR_VERIFY;
    }

    rc = cy_flash_area_boot_set_pending(app_id, (storage_ptr->validate_after_reboot == 0));
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

    if(cy_flash_area_boot_get_pending_status(app_id, status) != 0)
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
    result = cy_flash_area_boot_set_confirmed(app_id);
    if(result != CY_RSLT_SUCCESS)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "cy_flash_area_boot_set_confirmed() Failed \n");
        return CY_RSLT_OTA_ERROR_GENERAL;
    }
#else
    /* Remove the Boot Magic from the Image in the Secondary slot to mark the Image in the primary as permanent. */
    if((cy_flash_area_boot_unset_pending(app_id)) != 0)
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

    if(cy_flash_area_boot_get_image_confirm_status(app_id, status) != 0)
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
