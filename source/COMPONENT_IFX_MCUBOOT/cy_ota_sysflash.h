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

#ifndef CY_OTA_SYSFLASH_H
#define CY_OTA_SYSFLASH_H

#include <stdint.h>
#include "cy_syslib.h"

#ifndef CY_IFX_MCUBOOT_IMAGE_NUMBER
#ifdef MCUBootApp
#warning Undefined MCUBOOT_IMAGE_NUMBER. Assuming 1 (single-image).
#endif /* MCUBootApp */
#define CY_IFX_MCUBOOT_IMAGE_NUMBER 1
#endif /* MCUBOOT_IMAGE_NUMBER */

#if ((CY_IFX_MCUBOOT_IMAGE_NUMBER < 1) || (CY_IFX_MCUBOOT_IMAGE_NUMBER > 8))
#error Unsupported CY_IFX_MCUBOOT_IMAGE_NUMBER. Set it to between 1 and 8.
#endif /* ((CY_IFX_MCUBOOT_IMAGE_NUMBER < 1) || (CY_IFX_MCUBOOT_IMAGE_NUMBER > 8)) */

#ifndef FLASH_AREA_BOOTLOADER
#define FLASH_AREA_BOOTLOADER          ( 0u)
#endif

#ifndef FLASH_AREA_IMG_1_PRIMARY
#define FLASH_AREA_IMG_1_PRIMARY       ( 1u)
#endif

#ifndef FLASH_AREA_IMG_1_SECONDARY
#define FLASH_AREA_IMG_1_SECONDARY     ( 2u)
#endif

#if CY_IFX_MCUBOOT_IMAGE_NUMBER >= 2
#define FLASH_AREA_IMG_2_PRIMARY     (3u)
#define FLASH_AREA_IMG_2_SECONDARY   (4u)
#endif /* CY_IFX_MCUBOOT_IMAGE_NUMBER >= 2 */

#if CY_IFX_MCUBOOT_IMAGE_NUMBER >= 3
#define FLASH_AREA_IMG_3_PRIMARY     (5u)
#define FLASH_AREA_IMG_3_SECONDARY   (6u)
#endif /* CY_IFX_MCUBOOT_IMAGE_NUMBER >= 3 */

#if CY_IFX_MCUBOOT_IMAGE_NUMBER >= 4
#define FLASH_AREA_IMG_4_PRIMARY     (7u)
#define FLASH_AREA_IMG_4_SECONDARY   (8u)
#endif /* CY_IFX_MCUBOOT_IMAGE_NUMBER == 4 */

#if CY_IFX_MCUBOOT_IMAGE_NUMBER >= 5
#define FLASH_AREA_IMG_5_PRIMARY     (9u)
#define FLASH_AREA_IMG_5_SECONDARY   (10u)
#endif /* CY_IFX_MCUBOOT_IMAGE_NUMBER == 5 */

#if CY_IFX_MCUBOOT_IMAGE_NUMBER >= 6
#define FLASH_AREA_IMG_6_PRIMARY     (11u)
#define FLASH_AREA_IMG_6_SECONDARY   (12u)
#endif /* CY_IFX_MCUBOOT_IMAGE_NUMBER == 6 */

#if CY_IFX_MCUBOOT_IMAGE_NUMBER >= 7
#define FLASH_AREA_IMG_7_PRIMARY     (13u)
#define FLASH_AREA_IMG_7_SECONDARY   (14u)
#endif /* CY_IFX_MCUBOOT_IMAGE_NUMBER == 4 */

#if CY_IFX_MCUBOOT_IMAGE_NUMBER >= 8
#define FLASH_AREA_IMG_8_PRIMARY     (15u)
#define FLASH_AREA_IMG_8_SECONDARY   (16u)
#endif /* CY_IFX_MCUBOOT_IMAGE_NUMBER == 8 */

#ifndef FLASH_AREA_IMAGE_SCRATCH
#define FLASH_AREA_IMAGE_SCRATCH       (((CY_IFX_MCUBOOT_IMAGE_NUMBER*2)+1))
#endif


#ifndef FLASH_AREA_IMAGE_SWAP_STATUS
#define FLASH_AREA_IMAGE_SWAP_STATUS   (((CY_IFX_MCUBOOT_IMAGE_NUMBER+1)*2))
#endif

#ifndef FLASH_AREA_ERROR
#define FLASH_AREA_ERROR             255u  /* Invalid flash area */
#endif

#endif /* CY_OTA_SYSFLASH_H */
