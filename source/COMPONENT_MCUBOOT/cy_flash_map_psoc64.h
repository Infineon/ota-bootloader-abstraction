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
 * Flashmap header for PSOC_064_2M.
 */

#ifndef CY_FLASH_MAP_PSOC_64_H
#define CY_FLASH_MAP_PSOC_64_H

static struct flash_area flash_areas[] = {
    {
        .fa_id        = FLASH_AREA_BOOTLOADER,
        .fa_device_id = FLASH_AREA_BOOTLOADER_DEV_ID,
        .fa_off       = FLASH_AREA_BOOTLOADER_START,
        .fa_size      = FLASH_AREA_BOOTLOADER_SIZE,
    },
    {
        .fa_id        = FLASH_AREA_IMG_1_PRIMARY,
        .fa_device_id = FLASH_AREA_IMG_1_PRIMARY_DEV_ID,
        .fa_off       = FLASH_AREA_IMG_1_PRIMARY_START,
        .fa_size      = FLASH_AREA_IMG_1_PRIMARY_SIZE,
    },
    {
        .fa_id        = FLASH_AREA_IMG_1_SECONDARY,
        .fa_device_id = FLASH_AREA_IMG_1_SECONDARY_DEV_ID,
        .fa_off       = FLASH_AREA_IMG_1_SECONDARY_START,
        .fa_size      = FLASH_AREA_IMG_1_SECONDARY_SIZE,
    }
#ifdef FLASH_AREA_IMAGE_SCRATCH_DEV_ID
    ,{
        .fa_id        = FLASH_AREA_IMAGE_SCRATCH,
        .fa_device_id = FLASH_AREA_IMAGE_SCRATCH_DEV_ID,
        .fa_off       = FLASH_AREA_IMAGE_SCRATCH_START,
        .fa_size      = FLASH_AREA_IMAGE_SCRATCH_SIZE,
    }
#endif
#ifdef FLASH_AREA_IMAGE_SWAP_STATUS_DEV_ID
    ,{
        .fa_id        = FLASH_AREA_IMAGE_SWAP_STATUS,
        .fa_device_id = FLASH_AREA_IMAGE_SWAP_STATUS_DEV_ID,
        .fa_off       = FLASH_AREA_IMAGE_SWAP_STATUS_START,
        .fa_size      = FLASH_AREA_IMAGE_SWAP_STATUS_SIZE,
    }
#endif
};

/* These are indexes into the above table and do not directly correspond with the fa_id */
struct flash_area *boot_area_descs[] = {
    &flash_areas[0U],
    &flash_areas[1U],
    &flash_areas[2U],
#ifdef FLASH_AREA_IMAGE_SCRATCH_START
    &flash_areas[3U],
#endif
#ifdef FLASH_AREA_IMAGE_SWAP_STATUS_START
    &flash_areas[4U],
#endif
    NULL
};

#endif /* CY_FLASH_MAP_PSOC_64_H */
