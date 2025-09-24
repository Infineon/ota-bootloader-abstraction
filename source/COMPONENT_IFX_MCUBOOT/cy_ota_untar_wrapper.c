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
 *  Cypress OTA Agent network abstraction un-tar archive
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "cy_ota_api.h"
#include "cy_ota_internal.h"
#include "cy_ota_storage_api.h"
#include "cy_ota_bootloader_abstraction_log.h"

#include "cyabs_rtos.h"

#include "cy_ota_untar.h"
#include "cy_flash_map_backend.h"

/* define CY_TEST_APP_VERSION_IN_TAR to test the application version in the
 * TAR archive at start of OTA image download.
 *
 * NOTE: This requires that the user set the version number in the Makefile
 *          APP_VERSION_MAJOR
 *          APP_VERSION_MINOR
 *          APP_VERSION_BUILD
 */

/***********************************************************************
 *
 * OTA wrappers and callbacks for cy_ota_untar.c
 *
 **********************************************************************/

/***********************************************************************
 *
 * defines & enums
 *
 **********************************************************************/

/**
 * @brief Tarball support file types recognized in components.json file
 *
 * The file type in the tarball
 */
#define CY_FILE_TYPE_SPE        "SPE"       /**< Secure Programming Environment (TFM) code type                                */
#define CY_FILE_TYPE_NSPE       "NSPE"      /**< Non-Secure Programming Environment (application) code type                    */

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
 * Data & Variables
 *
 **********************************************************************/

/**
 * @brief Context structure for parsing the tar file
 */
static cy_untar_context_t  ota_untar_context;

/**
 * @brief Structure for handling TAR Header for MTU Sizes less than 512
 */
typedef struct update_file_header
{
    /* Temporary Buffer to handle TAR Header */
    uint8_t *buffer;

    /* Size of data in temporary Buffer */
    uint32_t buffer_size;

    /* Indicates if the TAR header check is completed */
    bool is_tar_header_checked;

} update_file_header_t;

static update_file_header_t file_header;

/***********************************************************************
 *
 * Forward declarations
 *
 **********************************************************************/

/***********************************************************************
 *
 * Functions
 *
 **********************************************************************/

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
    uint16_t image = 0;
    const struct flash_area *fap;
    cy_ota_storage_context_t *storage_ptr = (cy_ota_storage_context_t *)cb_arg;

    (void)image;
    if((ctxt == NULL) || (buffer == NULL) || (storage_ptr == NULL))
    {
        return CY_UNTAR_ERROR;
    }

#if !CY_OTA_DIRECT_XIP
    image = ctxt->files[ctxt->current_file].img_id;  /* The application code */
#endif

    if((file_offset + chunk_size) > ctxt->files[file_index].size)
    {
        chunk_size = ctxt->files[file_index].size - file_offset;
    }

    if(ctxt->files[ctxt->current_file].is_valid_img != 2)
    {
#if CY_OTA_DIRECT_XIP
        if(cy_flash_area_open(CY_FLASH_UPGRADE_AREA(APP_INACTIVE_SLOT, image), &fap) != 0)
#else
        if(cy_flash_area_open(CY_FLASH_AREA_IMAGE_SECONDARY(image-1), &fap) != 0)
#endif
        {
            cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "%s() cy_flash_area_open(%d) failed\n", __func__, image);
            return CY_UNTAR_ERROR;
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

#if 0 /* keep if needed later */
/**
 * @brief - set pending for the files contained in the TAR archive we just validated.
 *
 * return   CY_UNTAR_SUCCESS
 */
cy_untar_result_t cy_ota_untar_set_pending(void)
{
    uint16_t i;
    uint16_t image = 0;
    for (i = 0; i < ota_untar_context.num_files_in_json; i++ )
    {
        if(strncmp(ota_untar_context.files[i].type, CY_FILE_TYPE_SPE, strlen(CY_FILE_TYPE_SPE)) == 0)
        {
            image = 1;  /* The TFM code, cm0 */
        }
        else if(strncmp(ota_untar_context.files[i].type, CY_FILE_TYPE_NSPE, strlen(CY_FILE_TYPE_NSPE)) == 0)
        {
            image = 0;  /* The application code, cm4 */
        }
        else
        {
            /* unknown file type */
            cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "%d:%s BAD FILE TYPE %d: >%s<\n", __LINE__, __func__, i, ota_untar_context.files[i].type);
            continue;
        }
        boot_set_pending(image, 0);
    }

    return CY_UNTAR_SUCCESS;

}
#endif

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
