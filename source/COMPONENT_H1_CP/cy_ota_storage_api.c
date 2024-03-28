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

#include "cy_ota_storage_api.h"
#include "cy_ota_bootloader_abstraction_log.h"
#include "cy_ota_untar.h"

#include "cy_tx_thread.h"

/*
 * Define CY_TEST_APP_VERSION_IN_TAR to test the application version in the
 * TAR archive at start of OTA image download.
 *
 * NOTE: This requires that the user set the version number in the Makefile
 *          APP_VERSION_MAJOR
 *          APP_VERSION_MINOR
 *          APP_VERSION_BUILD
 */

/***********************************************************************
 *
 * defines & enums
 *
 **********************************************************************/

/***********************************************************************
 *
 * Macros
 *
 **********************************************************************/
#define CY_DS1_ADDRESS          0x680000

#ifndef CY_DS_SIZE
#define CY_DS_SIZE              0x3C0000
#endif

#define CY_DS_MDH_SIZE          48

/**
 * @brief Tarball support file types recognized in components.json file
 *
 * The file type in the tarball
 */
#define CY_FILE_TYPE_HEADER     "HEADER"            /**<  Image Header      */
#define CY_FILE_TYPE_DS         "DS"                /**<  Data Section      */
#define CY_FILE_TYPE_CERT       "CERTIFICATE"       /**<  Certificate       */

/***********************************************************************
 *
 * Structures
 *
 **********************************************************************/
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

/***********************************************************************
 *
 * Variables
 *
 **********************************************************************/
/**
 * @brief Context structure for parsing the tar file
 */
static cy_untar_context_t  ota_untar_context;

/**
 * @brief Context structure for update file header.
 */
static update_file_header_t file_header;

/**
 * @brief Local buffer for data flash write
 */
uint8_t *write_buffer = NULL;

/***********************************************************************
 *
 * functions
 *
 **********************************************************************/
static bool ota_allocate_write_buffer(uint64_t size)
{
    if(write_buffer != NULL)
    {
        free(write_buffer);
        write_buffer = NULL;
    }

    write_buffer = (uint8_t *)malloc(size);
    if(write_buffer == NULL)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "malloc() for Write Buffer FAILED! \n");
        return false;
    }
    else
    {
        return true;
    }
}

static void ota_free_write_buffer(void)
{
    if(write_buffer != NULL)
    {
        free(write_buffer);
        write_buffer = NULL;
    }
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
    BTSS_SYSTEM_NVRAM_OTA_ERR_t res = BTSS_SYSTEM_NVRAM_OTA_ERR_NONE;
    cy_ota_storage_context_t *storage_ptr = (cy_ota_storage_context_t *)cb_arg;

    if((ctxt == NULL) || (buffer == NULL) || (storage_ptr == NULL))
    {
        return CY_UNTAR_ERROR;
    }

    if((file_offset + chunk_size) > ctxt->files[file_index].size)
    {
        chunk_size = ctxt->files[file_index].size - file_offset;
    }

    if(ota_allocate_write_buffer(chunk_size) != true)
    {
        return CY_UNTAR_ERROR;
    }

    memcpy(write_buffer, buffer, chunk_size);

    if(strncmp(ctxt->files[file_index].type, CY_FILE_TYPE_HEADER, strlen(CY_FILE_TYPE_HEADER)) == 0)
    {
        /* Its Header Section(MDH) */
        /* Place holder for tar file based updates. */
    }
    else if(strncmp(ctxt->files[file_index].type, CY_FILE_TYPE_DS, strlen(CY_FILE_TYPE_DS)) == 0)
    {
        /* Its Data Section(DS) */
        /* Place holder for tar file based updates. */
    }
    else if(strncmp(ctxt->files[file_index].type, CY_FILE_TYPE_CERT, strlen(CY_FILE_TYPE_CERT)) == 0)
    {
        /* Its Certificate Section(CERT) */
        /* Place holder for tar file based updates. */
    }
    else
    {
        /* unknown file type */
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG2, "%d:%s Unknown File Type: >%s<\n", __LINE__, __func__, ctxt->files[file_index].type);
        return CY_UNTAR_ERROR;
    }

    ota_free_write_buffer();

    if(res != BTSS_SYSTEM_NVRAM_OTA_ERR_NONE)
    {
        return CY_UNTAR_ERROR;
    }
    else
    {
        return CY_UNTAR_SUCCESS;
    }
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
    cy_untar_result_t result = CY_UNTAR_ERROR;

    result = cy_untar_init(ctx_untar, ota_untar_write_callback, storage_ptr);
    if(result == CY_UNTAR_SUCCESS)
    {
        storage_ptr->ota_is_tar_archive  = 1;
    }
    return result;
}

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
    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "%s()\n", __func__);

    /* This init is already done BT subsystem. So returning CY_RSLT_SUCCESS */
    return CY_RSLT_SUCCESS;
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
    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "%s()\n", __func__);
    if(storage_ptr == NULL)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "Storage context pointer is invalid\n");
        return CY_RSLT_OTA_ERROR_OPEN_STORAGE;
    }

    /* clear out the stats */
    storage_ptr->total_image_size    = 0;
    storage_ptr->total_bytes_written = 0;
    storage_ptr->last_offset         = 0;
    storage_ptr->last_size           = 0;
    storage_ptr->storage_loc         = NULL;

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
    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "%s()\n", __func__);
    if(storage_ptr == NULL)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "Storage context pointer is invalid\n");
        return CY_RSLT_OTA_ERROR_READ_STORAGE;
    }

    /* Not Available for H1-CP Platforms. */
    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "READ NOT SUPPORTED FOR H1-CP...\r\n");
    return CY_RSLT_OTA_ERROR_UNSUPPORTED;
}

/**
 * @brief Determine if tar or non-tar and call correct write function
 *
 * @param[in]   storage_ptr     Pointer to the OTA Agent storage context 'cy_ota_storage_context_t'
 * @param[in]   chunk_info      Pointer to write data chunk information
 *
 * @return      CY_UNTAR_SUCCESS
 *              CY_UNTAR_ERROR
 */
cy_rslt_t cy_ota_storage_write(cy_ota_storage_context_t *storage_ptr, cy_ota_storage_write_info_t *chunk_info)
{
    BTSS_SYSTEM_NVRAM_OTA_ERR_t res = BTSS_SYSTEM_NVRAM_OTA_ERR_NONE;
    uint16_t copy_offset = 0;

    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "%s()\n", __func__);
    if((storage_ptr == NULL) || (chunk_info == NULL))
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "Storage context pointer is invalid\n");
        return CY_RSLT_OTA_ERROR_WRITE_STORAGE;
    }

    if(chunk_info->offset == 0UL)
    {
        file_header.is_tar_header_checked = false;
        file_header.buffer_size = 0;

        if(chunk_info->total_size > CY_DS_SIZE)
        {
            cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "Upgrade file size %ld exceeds available DS size %ld \n", chunk_info->total_size, CY_DS_SIZE);
            return CY_RSLT_OTA_ERROR_WRITE_STORAGE;
        }
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
                cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_NOTICE, "TAR ARCHIVE...!!!\n");
                if(cy_ota_untar_init_context(storage_ptr, &ota_untar_context) != CY_RSLT_SUCCESS)
                {
                    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "cy_ota_untar_init_context() FAILED! \n");
                    return CY_RSLT_OTA_ERROR_WRITE_STORAGE;
                }
            }
            else
            {
                cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_NOTICE, "Non TAR file...\n");
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
                    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "malloc() for file_header Buffer FAILED! \n");
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
                cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "TAR ARCHIVE ...!!!\n");
                if(cy_ota_untar_init_context(storage_ptr, &ota_untar_context) != CY_RSLT_SUCCESS)
                {
                    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "%s() cy_ota_untar_init_context() FAILED! \n", __func__);
                    return CY_RSLT_OTA_ERROR_WRITE_STORAGE;
                }
            }
            else
            {
                cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "Non TAR file...\n");
            }
            file_header.is_tar_header_checked = true;
        }
    }

    /* treat a tar file differently from a "normal" OTA */
    if(storage_ptr->ota_is_tar_archive != 0)
    {
        uint32_t consumed = 0;
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "TAR ARCHIVE file...!!!\n");

        if(file_header.buffer_size)
        {
            cy_untar_result_t result;

            while (consumed < file_header.buffer_size)
            {
                result = cy_untar_parse(&ota_untar_context, (consumed), (file_header.buffer + consumed),
                                         (file_header.buffer_size - consumed), &consumed);
                if((result == CY_UNTAR_ERROR) || (result == CY_UNTAR_INVALID))
                {
                    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "cy_untar_parse() FAIL consumed: %ld sz:%ld result:%ld)!\n", consumed, chunk_info->size, result);
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
                cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "cy_untar_parse() FAIL consumed: %ld sz:%ld result:%ld)!\n", consumed, chunk_info->size, result);
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
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "NON-TAR file ...\n");
        if(file_header.buffer_size)
        {
            /* First 48 Bytes is MDH, Pass MDH to Init.*/

            if(ota_allocate_write_buffer(CY_DS_MDH_SIZE) != true)
            {
                return CY_RSLT_OTA_ERROR_WRITE_STORAGE;
            }

            memset(write_buffer, 0x00, CY_DS_MDH_SIZE);
            memcpy(write_buffer, file_header.buffer, CY_DS_MDH_SIZE);

            cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "Header WRITE start for address : size : 0x%08x...!!!\n", CY_DS_MDH_SIZE);
            res = thread_ap_OTA_Initialize((BTSS_SYSTEM_NVRAM_OTA_HEADER_t *)write_buffer, CY_DS_MDH_SIZE);
            ota_free_write_buffer();
            if(res != BTSS_SYSTEM_NVRAM_OTA_ERR_NONE)
            {
                cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "thread_ap_OTA_Initialize() failed with result %d\n", (int)res);
            }
            else
            {
                cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "Header WRITE completed \n");
            }
            cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "DS WRITE start for address : 0x%08x size : 0x%08x...!!!\n",
                                                                                 (CY_DS1_ADDRESS), (file_header.buffer_size - CY_DS_MDH_SIZE));

            res = thread_ap_OTA_WriteAltImageMemory(CY_DS1_ADDRESS,
                                                    (file_header.buffer + CY_DS_MDH_SIZE),
                                                    (file_header.buffer_size - CY_DS_MDH_SIZE));
            free(file_header.buffer);
            file_header.buffer = NULL;
            file_header.buffer_size = 0;
            if(res != BTSS_SYSTEM_NVRAM_OTA_ERR_NONE)
            {
                cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "WRITE FAILED...!!!\n");
                return CY_RSLT_OTA_ERROR_WRITE_STORAGE;
            }
            else
            {
                cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "DS WRITE Completed...!!!\n");
            }
        }

        if(ota_allocate_write_buffer(chunk_info->size) != true)
        {
            return CY_RSLT_OTA_ERROR_WRITE_STORAGE;
        }
        memset(write_buffer, 0x00, chunk_info->size);

        /* First 48 Bytes is MDH, Pass MDH to Init.*/
        if(chunk_info->offset == 0UL)
        {
            memcpy(write_buffer, chunk_info->buffer, CY_DS_MDH_SIZE);
            cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "Header WRITE start for address : size : 0x%08x...!!!\n", CY_DS_MDH_SIZE);
            res = thread_ap_OTA_Initialize((BTSS_SYSTEM_NVRAM_OTA_HEADER_t *)write_buffer, CY_DS_MDH_SIZE);
            if(res != BTSS_SYSTEM_NVRAM_OTA_ERR_NONE)
            {
                ota_free_write_buffer();
                cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "thread_ap_OTA_Initialize() failed with result %d\n", (int)res);
                return CY_RSLT_OTA_ERROR_WRITE_STORAGE;
            }
            else
            {
                cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "Header WRITE completed \n");
            }

            /* Write remaing bytes of chunks to DS section */
            memset(write_buffer, 0x00, chunk_info->size);
            memcpy(write_buffer, (chunk_info->buffer + CY_DS_MDH_SIZE), ((chunk_info->size) - CY_DS_MDH_SIZE));

            cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "DS WRITE start for address : 0x%08x size : 0x%08x...!!!\n",
                                                                                 (CY_DS1_ADDRESS), ((chunk_info->size) - CY_DS_MDH_SIZE));
            res = thread_ap_OTA_WriteAltImageMemory(CY_DS1_ADDRESS, write_buffer, ((chunk_info->size) - CY_DS_MDH_SIZE));
            if(res != BTSS_SYSTEM_NVRAM_OTA_ERR_NONE)
            {
                ota_free_write_buffer();
                cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "WRITE FAILED...!!!\n");
                return CY_RSLT_OTA_ERROR_WRITE_STORAGE;
            }
            else
            {
                ota_free_write_buffer();
                cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "DS WRITE Completed...!!!\n");
            }
        }
        else
        {
            memcpy(write_buffer, (chunk_info->buffer + copy_offset), chunk_info->size);
            cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "DS WRITE start for address : 0x%08x size : 0x%08x...!!!\n",
                                                    (CY_DS1_ADDRESS + chunk_info->offset - CY_DS_MDH_SIZE), (chunk_info->size));
            res = thread_ap_OTA_WriteAltImageMemory((CY_DS1_ADDRESS + chunk_info->offset - CY_DS_MDH_SIZE), write_buffer, chunk_info->size);
            if(res != BTSS_SYSTEM_NVRAM_OTA_ERR_NONE)
            {
                ota_free_write_buffer();
                cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "WRITE FAILED with error %d...!!!\n", res);
                return CY_RSLT_OTA_ERROR_WRITE_STORAGE;
            }
            else
            {
                ota_free_write_buffer();
                cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "DS WRITE Completed...!!!\n");
            }
        }
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
    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "%s()\n", __func__);
    if(storage_ptr == NULL)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "Storage context pointer is invalid\n");
        return CY_RSLT_OTA_ERROR_CLOSE_STORAGE;
    }

    return CY_RSLT_SUCCESS;
}

/**
 * @brief Verify download signature
 *
 * @param[in]   storage_ptr     Pointer to the OTA Agent storage context @ref cy_ota_storage_context_t
 *
 * @return  CY_RSLT_SUCCESS
 *          CY_RSLT_OTA_ERROR_GENERAL
 */
cy_rslt_t cy_ota_storage_verify(cy_ota_storage_context_t *storage_ptr)
{
    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "%s()\n", __func__);
    if(storage_ptr == NULL)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "Storage context pointer is invalid\n");
        return CY_RSLT_OTA_ERROR_VERIFY;
    }

    /* Current implementation of ota-bootloader-abstraction doesnot verify Image.*/
    return CY_RSLT_SUCCESS;
}

/**
 * @brief Application has validated the new OTA Image
 *
 * This call needs to be after reboot and Bootloader has started the upgrade version of Application.
 *      to the Primary Slot.
 *
 * @param[in]   app_id          Application ID.
 *
 * @return      CY_RSLT_SUCCESS
 *              CY_RSLT_OTA_ERROR_GENERAL
 */
cy_rslt_t cy_ota_storage_image_validate(uint16_t app_id)
{
    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "%s()\n", __func__);
    (void)app_id;

    /* Not required for H1-CP platform. */
    return CY_RSLT_SUCCESS;
}

/**
 * @brief Application validates the downloaded new OTA Image and activate it.
 *
 * This call needs to be after new OTA image successfully downloaded to inactive DS.
 * If OTA image header, image certificates are valid then bootloader boots new OTA image.
 *
 * @param[in]   app_id          Application ID(For future use).
 *
 * @return      CY_RSLT_SUCCESS
 *              CY_RSLT_OTA_ERROR_GENERAL
 */
cy_rslt_t cy_ota_storage_switch_to_new_image(uint16_t app_id)
{
    /* Mark Image in Primary Slot as valid */
    BTSS_SYSTEM_NVRAM_OTA_ERR_t res = BTSS_SYSTEM_NVRAM_OTA_ERR_NONE;

    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_DEBUG, "%s()\n", __func__);
    (void)app_id;

    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "Enabling downloaded OTA image...\n");
    cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "Reset the Device to boot upgrade image...\n");
    res = thread_ap_OTA_SwitchToAltImage();
    if(res == BTSS_SYSTEM_NVRAM_OTA_ERR_NONE)
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "thread_ap_OTA_SwitchToAltImage completed successfully.\n", (unsigned int)res);
        return CY_RSLT_SUCCESS;
    }
    else
    {
        cy_ota_bootloader_abstraction_log_msg(CYLF_MIDDLEWARE, CY_LOG_ERR, "thread_ap_OTA_SwitchToAltImage failed with res : %d...\n", (int)res);
        return CY_RSLT_OTA_ERROR_GENERAL;
    }
}

cy_rslt_t cy_ota_storage_get_app_info(void* file_des, cy_ota_app_info_t *app_info)
{
    cy_rslt_t result = CY_RSLT_SUCCESS;

    return result;
}
