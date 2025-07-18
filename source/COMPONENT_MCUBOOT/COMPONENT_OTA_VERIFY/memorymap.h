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
 *
 * MCUBoot specfic memorymap macro's.
 *
 */

#pragma once
#include "flash_map_backend.h"

#include "bootutil/bootutil.h"
extern struct flash_area flash_areas[];
extern struct flash_area * const boot_area_descs[];

#ifndef FLASH_AREA_BOOTLOADER
#define FLASH_AREA_BOOTLOADER          ( 0u)
#endif

#ifndef FLASH_AREA_IMG_1_PRIMARY
#define FLASH_AREA_IMG_1_PRIMARY       ( 1u)
#endif

#ifndef FLASH_AREA_IMG_1_SECONDARY
#define FLASH_AREA_IMG_1_SECONDARY     ( 2u)
#endif

#ifndef FLASH_AREA_IMAGE_SCRATCH
#define FLASH_AREA_IMAGE_SCRATCH       ( 3u)
#endif

#ifndef FLASH_AREA_IMAGE_SWAP_STATUS
#define FLASH_AREA_IMAGE_SWAP_STATUS   ( 7u)
#endif

typedef enum
{
	IMAGE_BOOT_MODE_FLASH = 0U,
	IMAGE_BOOT_MODE_RAM = 1U,
} image_boot_mode_t;

#if 0
typedef struct image_boot_config_s {
	image_boot_mode_t mode;
	uint32_t address;
	uint32_t size;
} image_boot_config_t;

extern image_boot_config_t image_boot_config[BOOT_IMAGE_NUMBER];
#endif
