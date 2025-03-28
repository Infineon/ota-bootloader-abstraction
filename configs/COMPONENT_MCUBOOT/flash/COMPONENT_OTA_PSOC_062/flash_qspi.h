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
 *  Header file for PSoC6 external flash driver adoption layer.
 */

#ifndef _FLASH_QSPI_H
#define _FLASH_QSPI_H

#ifdef OTA_USE_EXTERNAL_FLASH

#include <stdint.h>
#include "cy_pdl.h"

cy_en_smif_status_t qspi_init_sfdp(uint32_t smif_id);
cy_en_smif_status_t qspi_init(cy_stc_smif_block_config_t *blk_config);
cy_en_smif_status_t qspi_init_hardware(void);
uint32_t qspi_get_prog_size(void);
uint32_t qspi_get_erase_size(void);
uint32_t qspi_get_mem_size(void);

SMIF_Type *qspi_get_device(void);
cy_stc_smif_context_t *qspi_get_context(void);
cy_stc_smif_mem_config_t *qspi_get_memory_config(uint8_t index);

void qspi_deinit(uint32_t smif_id);

void qspi_set_mode(cy_en_smif_mode_t mode);
cy_en_smif_mode_t qspi_get_mode(void);

#endif /* OTA_USE_EXTERNAL_FLASH */

#endif /* _FLASH_QSPI_H */
