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

#ifndef SYSFLASH_H
#define SYSFLASH_H

#include <stdint.h>
#include "cy_syslib.h"

#ifndef MCUBOOT_IMAGE_NUMBER
#ifdef MCUBootApp
#warning Undefined MCUBOOT_IMAGE_NUMBER. Assuming 1 (single-image).
#endif /* MCUBootApp */
#define MCUBOOT_IMAGE_NUMBER 1
#endif /* MCUBOOT_IMAGE_NUMBER */

#if ((MCUBOOT_IMAGE_NUMBER < 1) || (MCUBOOT_IMAGE_NUMBER > 4))
#error Unsupported MCUBOOT_IMAGE_NUMBER. Set it to between 1 and 4.
#endif /* ((MCUBOOT_IMAGE_NUMBER < 1) || (MCUBOOT_IMAGE_NUMBER > 4)) */

#define FLASH_AREA_BOOTLOADER        (0u)

#define FLASH_AREA_IMG_1_PRIMARY     (1u)
#define FLASH_AREA_IMG_1_SECONDARY   (2u)

#define FLASH_AREA_IMAGE_SCRATCH     (3u)

#if MCUBOOT_IMAGE_NUMBER >= 2
#define FLASH_AREA_IMG_2_PRIMARY     (4u)
#define FLASH_AREA_IMG_2_SECONDARY   (5u)
#endif /* MCUBOOT_IMAGE_NUMBER >= 2 */

#define FLASH_AREA_IMAGE_SWAP_STATUS (7u)

#if MCUBOOT_IMAGE_NUMBER >= 3
#define FLASH_AREA_IMG_3_PRIMARY     (8u)
#define FLASH_AREA_IMG_3_SECONDARY   (9u)
#endif /* MCUBOOT_IMAGE_NUMBER >= 3 */

#if MCUBOOT_IMAGE_NUMBER == 4
#define FLASH_AREA_IMG_4_PRIMARY     (10u)
#define FLASH_AREA_IMG_4_SECONDARY   (11u)
#endif /* MCUBOOT_IMAGE_NUMBER == 4 */

#define FLASH_AREA_ERROR             255u  /* Invalid flash area */

#endif /* SYSFLASH_H */
