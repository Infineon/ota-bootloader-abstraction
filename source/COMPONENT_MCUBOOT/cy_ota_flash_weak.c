/*
 * Copyright 2023, Cypress Semiconductor Corporation (an Infineon company) or
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
 * Weak functions for ota flash APIs
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "cy_ota_flash.h"

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
#if defined(__ICCARM__)
#define OTA_WEAK_FUNCTION        __WEAK
#define TLS_ATTR_PACKED(struct)  __packed struct
#elif defined(__GNUC__) || defined(__clang__) || defined(__CC_ARM)
#define OTA_WEAK_FUNCTION        __attribute__((weak))
#define TLS_ATTR_PACKED(struct)  struct __attribute__((packed))
#else
#define OTA_WEAK_FUNCTION        __attribute__((weak))
#define TLS_ATTR_PACKED(struct)  struct __attribute__((packed))
#endif  /* defined(__ICCARM__) */

#define UNUSED_ARG(arg)                     (void)(arg)

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
 * @brief Initializes flash, QSPI flash, or any other external memory type
 *
 * NOTE: This function must be implemented in the user's code.
 *
 * @return  CY_RSLT_SUCCESS on success
 *          CY_RSLT_TYPE_ERROR on failure
 */
OTA_WEAK_FUNCTION cy_rslt_t cy_ota_mem_init(void)
{
    /*
     * This function does nothing, weak implementation.
     * The purpose of this code is to disable compiler warnings for Non-optimized
     * builds which do not remove unused functions and require them for the
     * completeness of the linking step.
     */
    return CY_RSLT_SUCCESS;
}

/**
 * @brief Read from flash, QSPI flash, or any other external memory type
 *
 * NOTE: This function must be implemented in the user's code.
 *
 * @param[in]   mem_type   Memory type @ref cy_ota_mem_type_t
 * @param[in]   addr       Starting address to read from.
 * @param[out]  data       Pointer to the buffer to store the data read from the memory.
 * @param[in]   len        Number of data bytes to read.
 *
 * @return  CY_RSLT_SUCCESS on success
 *          CY_RSLT_TYPE_ERROR on failure
 */
OTA_WEAK_FUNCTION cy_rslt_t cy_ota_mem_read(cy_ota_mem_type_t mem_type, uint32_t addr, void *data, size_t len)
{
    /*
     * This function does nothing, weak implementation.
     * The purpose of this code is to disable compiler warnings for Non-optimized
     * builds which do not remove unused functions and require them for the
     * completeness of the linking step.
     */
    UNUSED_ARG(mem_type);
    UNUSED_ARG(addr);
    UNUSED_ARG(data);
    UNUSED_ARG(len);
    return CY_RSLT_SUCCESS;
}

/**
 * @brief Write to flash, QSPI flash, or any other external memory type
 *
 * NOTE: This function must be implemented in the user's code.
 *
 * @param[in]   mem_type   Memory type @ref cy_ota_mem_type_t
 * @param[in]   addr       Starting address to write to.
 * @param[in]   data       Pointer to the buffer containing the data to be written.
 * @param[in]   len        Number of bytes to write.
 *
 * @return  CY_RSLT_SUCCESS on success
 *          CY_RSLT_TYPE_ERROR on failure
 */
OTA_WEAK_FUNCTION cy_rslt_t cy_ota_mem_write(cy_ota_mem_type_t mem_type, uint32_t addr, void *data, size_t len)
{
    /*
     * This function does nothing, weak implementation.
     * The purpose of this code is to disable compiler warnings for Non-optimized
     * builds which do not remove unused functions and require them for the
     * completeness of the linking step.
     */
    UNUSED_ARG(mem_type);
    UNUSED_ARG(addr);
    UNUSED_ARG(data);
    UNUSED_ARG(len);
    return CY_RSLT_SUCCESS;
}

/**
 * @brief Erase flash, QSPI flash, or any other external memory type
 *
 * NOTE: This function must be implemented in the user's code.
 *
 * @param[in]   mem_type   Memory type @ref cy_ota_mem_type_t
 * @param[in]   addr       Starting address to begin erasing.
 * @param[in]   len        Number of bytes to erase.
 *
 * @return  CY_RSLT_SUCCESS
 *          CY_RSLT_TYPE_ERROR
 */
OTA_WEAK_FUNCTION cy_rslt_t cy_ota_mem_erase(cy_ota_mem_type_t mem_type, uint32_t addr, size_t len)
{
    /*
     * This function does nothing, weak implementation.
     * The purpose of this code is to disable compiler warnings for Non-optimized
     * builds which do not remove unused functions and require them for the
     * completeness of the linking step.
     */
    UNUSED_ARG(mem_type);
    UNUSED_ARG(addr);
    UNUSED_ARG(len);
    return CY_RSLT_SUCCESS;
}

/**
 * @brief To get page size for programming flash, QSPI flash, or any other external memory type
 *
 * NOTE: This function must be implemented in the user's code.
 *
 * @param[in]   mem_type   Memory type @ref cy_ota_mem_type_t
 * @param[in]   addr       Address that belongs to the sector for which programming page size needs to be returned.
 *
 * @return    Page size in bytes.
 */
OTA_WEAK_FUNCTION size_t cy_ota_mem_get_prog_size(cy_ota_mem_type_t mem_type, uint32_t addr)
{
    /*
     * This function does nothing, weak implementation.
     * The purpose of this code is to disable compiler warnings for Non-optimized
     * builds which do not remove unused functions and require them for the
     * completeness of the linking step.
     */
    UNUSED_ARG(mem_type);
    UNUSED_ARG(addr);
    return CY_RSLT_SUCCESS;
}

/**
 * @brief To get sector size of flash, QSPI flash, or any other external memory type
 *
 * NOTE: This function must be implemented in the user's code.
 *
 * @param[in]   mem_type   Memory type @ref cy_ota_mem_type_t
 * @param[in]   addr       Address that belongs to the sector for which sector erase size needs to be returned.
 *
 * @return    Sector size in bytes.
 */
OTA_WEAK_FUNCTION size_t cy_ota_mem_get_erase_size(cy_ota_mem_type_t mem_type, uint32_t addr)
{
    /*
     * This function does nothing, weak implementation.
     * The purpose of this code is to disable compiler warnings for Non-optimized
     * builds which do not remove unused functions and require them for the
     * completeness of the linking step.
     */
    UNUSED_ARG(mem_type);
    UNUSED_ARG(addr);
    return CY_RSLT_SUCCESS;
}
