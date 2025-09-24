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

#ifndef __FLASH_MAP_BACKEND_H__
#define __FLASH_MAP_BACKEND_H__

#include "cy_ota_sysflash.h"

#define CY_MCUBOOT_ERR_FLASH      1
#define CY_MCUBOOT_ERR_FILE       2
#define CY_MCUBOOT_ERR_BADIMAGE   3
#define CY_MCUBOOT_ERR_BADVECT    4
#define CY_MCUBOOT_ERR_BADSTATUS  5
#define CY_MCUBOOT_ERR_NOMEM      6
#define CY_MCUBOOT_ERR_BADARGS    7
#define CY_MCUBOOT_ERR_BADVERSION 8

/*
 * Swap type values for MCUBoot
 */

/** Attempt to boot the contents of the primary slot. */
#define CY_MCUBOOT_SWAP_TYPE_NONE     1

/*
 * Swap to the secondary slot.
 * Absent a confirm command, revert back on next boot.
 */
#define CY_MCUBOOT_SWAP_TYPE_TEST     2

/*
 * Swap to the secondary slot and permanently switch to booting its contents.
 */
#define CY_MCUBOOT_SWAP_TYPE_PERM     3

/* Swap back to alternate slot.  A confirm changes this state to NONE. */
#define CY_MCUBOOT_SWAP_TYPE_REVERT   4

/* Swap failed because image to be run is not valid */
#define CY_MCUBOOT_SWAP_TYPE_FAIL     5

/* Swapping encountered an unrecoverable error */
#define CY_MCUBOOT_SWAP_TYPE_PANIC    0xff


#define CY_MCUBOOT_MAGIC_GOOD     1
#define CY_MCUBOOT_MAGIC_BAD      2
#define CY_MCUBOOT_MAGIC_UNSET    3
#define CY_MCUBOOT_MAGIC_ANY      4  /* NOTE: control only, not dependent on sector */
#define CY_MCUBOOT_MAGIC_NOTGOOD  5  /* NOTE: control only, not dependent on sector */

/* For setting the image_ok flag */
#define CY_MCUBOOT_FLAG_SET       1
#define CY_MCUBOOT_FLAG_BAD       2
#define CY_MCUBOOT_FLAG_UNSET     3
#define CY_MCUBOOT_FLAG_ANY       4  /* NOTE: control only, not dependent on sector */

#define CY_FLASH_DEVICE_INDEX_MASK                 (0x7F)
#define CY_FLASH_DEVICE_GET_EXT_INDEX(n)           ((n) & CY_FLASH_DEVICE_INDEX_MASK)
#define CY_FLASH_DEVICE_EXTERNAL_FLAG              (0x80)
#define CY_FLASH_DEVICE_INTERNAL_FLASH             (0x7F)
#define CY_FLASH_DEVICE_EXTERNAL_FLASH(index)      (CY_FLASH_DEVICE_EXTERNAL_FLAG | index)

/* Code and Work flash regions for XMC */
#if defined (XMC7100) || defined (XMC7200)
/* Internal Code flash */
#define INTERNAL_FLASH_CODE_LARGE               CY_FLASH_DEVICE_INTERNAL_FLASH
#define INTERNAL_FLASH_CODE_SMALL               CY_FLASH_DEVICE_INTERNAL_FLASH
/* Internal Work flash */
#define INTERNAL_FLASH_WORK_LARGE               CY_FLASH_DEVICE_INTERNAL_FLASH
#define INTERNAL_FLASH_WORK_SMALL               CY_FLASH_DEVICE_INTERNAL_FLASH
#endif

#ifndef CY_BOOT_EXTERNAL_DEVICE_INDEX
/* assume first(one) SMIF0 device is used */
#define CY_BOOT_EXTERNAL_DEVICE_INDEX            (0)
#endif

/*
 * Extract the swap type and image number from image trailers's swap_info
 * filed.
 */
#define BOOT_GET_SWAP_TYPE(swap_info)    ((swap_info) & 0x0FU)
#define BOOT_GET_IMAGE_NUM(swap_info)    ((swap_info) >> 4U)

/* multi-image ? */
#if (MCUBOOT_IMAGE_NUMBER == 1)

#define CY_FLASH_AREA_IMAGE_PRIMARY(x)    (((x) == 0) ?          \
                                         FLASH_AREA_IMG_1_PRIMARY : \
                                         FLASH_AREA_IMG_1_PRIMARY)

#define CY_FLASH_AREA_IMAGE_SECONDARY(x)  (((x) == 0) ?          \
                                         FLASH_AREA_IMG_1_SECONDARY : \
                                         FLASH_AREA_IMG_1_SECONDARY)

#define CY_FLASH_UPGRADE_AREA(x,y)    (((x) == 0) ?          \
                                         FLASH_AREA_IMG_1_PRIMARY : \
                                         FLASH_AREA_IMG_1_SECONDARY)

#elif (MCUBOOT_IMAGE_NUMBER == 2)

#define CY_FLASH_AREA_IMAGE_PRIMARY(x)    (((x) == 0) ?          \
                                         FLASH_AREA_IMG_1_PRIMARY : \
                                        ((x) == 1) ?          \
                                         FLASH_AREA_IMG_2_PRIMARY : \
                                         255)
#define CY_FLASH_AREA_IMAGE_SECONDARY(x)  (((x) == 0) ?          \
                                         FLASH_AREA_IMG_1_SECONDARY : \
                                        ((x) == 1) ?          \
                                         FLASH_AREA_IMG_2_SECONDARY : \
                                         255)

#else
#warning "Image slot and flash area mapping is not defined"
#endif


#if((defined (CYW20829B0LKML) || defined (CYW20829B1010) || \
     defined (CYW89829B01MKSBG) || defined (CYW89829B1232)) && (MCUBOOT_IMAGE_NUMBER > 1))
#error Multi-image configuration is NOT supported on this platform!
#endif /* ((defined(CYW20829) || defined(CYW89829)) && MCUBOOT_IMAGE_NUMBER > 1) */

/**
 *
 * Provides abstraction of flash regions for type of use.
 * I.e. dude where's my image?
 *
 * System will contain a map which contains flash areas. Every
 * region will contain flash identifier, offset within flash and length.
 *
 * 1. This system map could be in a file within filesystem (Initializer
 * must know/figure out where the filesystem is at).
 * 2. Map could be at fixed location for project (compiled to code)
 * 3. Map could be at specific place in flash (put in place at mfg time).
 *
 * Note that the map you use must be valid for BSP it's for,
 * match the linker scripts when platform executes from flash,
 * and match the target offset specified in download script.
 */
#include <inttypes.h>

/**
 * @brief Structure describing an area on a flash device.
 *
 * Multiple flash devices may be available in the system, each of
 * which may have its own areas. For this reason, flash areas track
 * which flash device they are part of.
 */
#ifndef CY_OTA_IMAGE_VERIFICATION
struct flash_area
{
    uint8_t fa_id;         /** This flash area's ID; unique in the system. */
    uint8_t fa_device_id;  /** ID of the flash device this area is a part of.  */
    uint16_t pad16;        /** Pad for Word length(32 Bytes). */
    uint32_t fa_off;       /** This area's offset, relative to the beginning of its flash device's storage. */
    uint32_t fa_size;      /** This area's size, in bytes. */
};
#else
#include "flash_map_backend.h"
#endif

/*< Opens the area for use. id is one of the `fa_id`s */
int8_t cy_flash_area_open(uint8_t id, const struct flash_area **fa);

/*< Closes the area for use. id is one of the `fa_id`s */
void cy_flash_area_close(const struct flash_area *fa);

/*< Reads `len` bytes of flash memory at `off` to the buffer at `dst` */
int8_t cy_flash_area_read(const struct flash_area *fa, uint32_t off, void *dst, uint32_t len);

/*< Writes `len` bytes of flash memory at `off` from the buffer at `src` */
int8_t cy_flash_area_write(const struct flash_area *fa, uint32_t off, const void *src, uint32_t len);

/*< Erases `len` bytes of flash memory at `off` */
int8_t cy_flash_area_erase(const struct flash_area *fa, uint32_t off, uint32_t len);

/* writes MAGIC, OK and swap_type */
int8_t cy_flash_area_boot_set_pending(uint8_t image, uint8_t permanent);

/* Reads swap_type from secondary slot image header */
int8_t cy_flash_area_boot_get_pending_status(uint8_t *boot_pending_status);

/* writes MAGIC and OK */
int8_t cy_flash_area_boot_set_confirmed(uint8_t image);

/* Reads image ok flag from primary slot image header */
int8_t cy_flash_area_boot_get_image_confirm_status(uint8_t *image_ok_status);

/* Removes the Boot Magic of the image in the secondary slot. */
int8_t cy_flash_area_boot_unset_pending(uint8_t image);

#endif /* __FLASH_MAP_BACKEND_H__ */
