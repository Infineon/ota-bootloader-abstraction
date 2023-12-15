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

#ifndef CY_OTA_FLASH_H_
#define CY_OTA_FLASH_H_

#include "cy_result_mw.h"

/* OTA API */
#include "cy_pdl.h"
#include "cyhal.h"

/**
 * \addtogroup group_ota_bootsupport
 * \{
 */

/***********************************************************************
 *
 * Defines & Enums
 *
 **********************************************************************/
 /**
  * \addtogroup group_ota_bootsupport_macros
  * \{
  */
/** The function or operation is not supported on the target or the memory */
#define CY_RSLT_SERIAL_FLASH_ERR_UNSUPPORTED (cy_rslt_t)(CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_BOARD_LIB_SERIAL_FLASH, 1))
/** The Serial Flash not initialized */
#define CY_RSLT_SERIAL_FLASH_ERR_NOT_INITED (cy_rslt_t)(CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_BOARD_LIB_SERIAL_FLASH, 2))
/** Parameters passed to a function are invalid */
#define CY_RSLT_SERIAL_FLASH_ERR_BAD_PARAM   (cy_rslt_t)(CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_BOARD_LIB_SERIAL_FLASH, 3))
/** A previously initiated read operation is not yet complete */
#define CY_RSLT_SERIAL_FLASH_ERR_READ_BUSY   (cy_rslt_t)(CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_BOARD_LIB_SERIAL_FLASH, 4))
/** A DMA error occurred during read transfer */
#define CY_RSLT_SERIAL_FLASH_ERR_DMA         (cy_rslt_t)(CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_BOARD_LIB_SERIAL_FLASH, 5))
/** Read abort failed. QSPI block is busy. */
#define CY_RSLT_SERIAL_FLASH_ERR_QSPI_BUSY   (cy_rslt_t)(CY_RSLT_CREATE(CY_RSLT_TYPE_ERROR, CY_RSLT_MODULE_BOARD_LIB_SERIAL_FLASH, 6))

/** \} group_ota_macros */


/**
  * \addtogroup group_ota_bootsupport_typedefs
  * \{
  */

/**
 * @brief Enumeration of OTA storage types.
 */
typedef enum
{
    CY_OTA_MEM_TYPE_INTERNAL_FLASH = 0,      /**<  For internal flash type. */
    CY_OTA_MEM_TYPE_EXTERNAL_FLASH,          /**<  For external flash type. */
    CY_OTA_MEM_TYPE_RRAM,                    /**<  For RRAM type.           */
    CY_OTA_MEM_TYPE_NONE                     /**<  Default value.           */
} cy_ota_mem_type_t;

/** \} group_ota_typedefs */

/***********************************************************************
 *
 * Functions
 *
 **********************************************************************/
/**
 * \addtogroup group_ota_bootsupport_functions
 * \{
 * OTA Bootloader support library flash operation APIs. Storage Interface APIs call these flash operation APIs
 * to store downloaded UPGRADE image on Internal or External Flash based on target platforms.
 */

/**
 * @brief Initializes flash, QSPI flash, or any other external memory type
 *
 * NOTE: This function must be implemented in the user's code.
 *
 * @return  CY_RSLT_SUCCESS on success
 *          CY_RSLT_TYPE_ERROR on failure
 */
cy_rslt_t cy_ota_mem_init(void);

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
cy_rslt_t cy_ota_mem_read(cy_ota_mem_type_t mem_type, uint32_t addr, void *data, size_t len);

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
cy_rslt_t cy_ota_mem_write(cy_ota_mem_type_t mem_type, uint32_t addr, void *data, size_t len);

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
cy_rslt_t cy_ota_mem_erase(cy_ota_mem_type_t mem_type, uint32_t addr, size_t len);

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
size_t cy_ota_mem_get_prog_size(cy_ota_mem_type_t mem_type, uint32_t addr);

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
size_t cy_ota_mem_get_erase_size(cy_ota_mem_type_t mem_type, uint32_t addr);

/** \} group_ota_bootsupport_functions */

/** \} group_ota_bootsupport */

#endif /* CY_OTA_FLASH_H_ */
