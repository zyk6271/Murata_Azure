/* * Copyright 2020, Cypress Semiconductor Corporation or a subsidiary of 
 * Cypress Semiconductor Corporation. All Rights Reserved.
 * 
 * This software, associated documentation and materials ("Software"),
 * is owned by Cypress Semiconductor Corporation
 * or one of its subsidiaries ("Cypress") and is protected by and subject to
 * worldwide patent protection (United States and foreign),
 * United States copyright laws and international treaty provisions.
 * Therefore, you may use this Software only as provided in the license
 * agreement accompanying the software package from which you
 * obtained this Software ("EULA").
 * If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 * non-transferable license to copy, modify, and compile the Software
 * source code solely for use in connection with Cypress's
 * integrated circuit products. Any reproduction, modification, translation,
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
 * so agrees to indemnify Cypress against all liability. */MEMORY{/*    BTLDR_OR_VECTOR_FLASH      (rx)  : ORIGIN = 0x1A000000, LENGTH = 512*/    DCT1_FLASH(rx) : ORIGIN = 0x1A004000, LENGTH = 16K    DCT2_FLASH(rx) : ORIGIN = 0x1A008000, LENGTH = 16K    APP_CODE_A(rx) : ORIGIN = 0x1A00C000, LENGTH = 474880    APP_CODE_B(rx) : ORIGIN = 0x1B000000, LENGTH = 512K    CODE_READ_PROTECTION_B(rx) : ORIGIN = 0x1B0002FC, LENGTH = 4    SRAM_FAST(rwx) : ORIGIN = 0x10000000, LENGTH = 32K    SRAM_DATA(rwx) : ORIGIN = 0x10080000, LENGTH = 40K    SRAM_STACK(rwx) : ORIGIN = 0x20000000, LENGTH = 64K}PHDRS{    rodata PT_LOAD ;}SECTIONS{    .rodata :    {        dct_location = .;        KEEP(*(.rodata.initial_dct))        KEEP(*(.rodata._app_dct))        KEEP(*(.rodata.wiced_apps_lut))        KEEP(*(.rodata.wiced_apps_lut_fill))        dct_end = .;    } >DCT1_FLASH  AT> DCT1_FLASH :rodata}PROVIDE( dct_full_size_loc = LENGTH( DCT1_FLASH ));PROVIDE( dct_used_size_loc = (dct_end - ORIGIN( DCT1_FLASH )));