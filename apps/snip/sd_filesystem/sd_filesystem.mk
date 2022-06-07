#
# Copyright 2020, Cypress Semiconductor Corporation or a subsidiary of 
 # Cypress Semiconductor Corporation. All Rights Reserved.
 # This software, including source code, documentation and related
 # materials ("Software"), is owned by Cypress Semiconductor Corporation
 # or one of its subsidiaries ("Cypress") and is protected by and subject to
 # worldwide patent protection (United States and foreign),
 # United States copyright laws and international treaty provisions.
 # Therefore, you may use this Software only as provided in the license
 # agreement accompanying the software package from which you
 # obtained this Software ("EULA").
 # If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 # non-transferable license to copy, modify, and compile the Software
 # source code solely for use in connection with Cypress's
 # integrated circuit products. Any reproduction, modification, translation,
 # compilation, or representation of this Software except as specified
 # above is prohibited without the express written permission of Cypress.
 #
 # Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 # EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 # WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 # reserves the right to make changes to the Software without notice. Cypress
 # does not assume any liability arising out of the application or use of the
 # Software or any product or circuit described in the Software. Cypress does
 # not authorize its products for use in any products where a malfunction or
 # failure of the Cypress product may reasonably be expected to result in
 # significant property damage, injury or death ("High Risk Product"). By
 # including Cypress's product in a High Risk Product, the manufacturer
 # of such system or application assumes all risk of such use and in doing
 # so agrees to indemnify Cypress against all liability.
#

NAME := App_sd_filesystem

$(NAME)_SOURCES := sd_filesystem.c

#==============================================================================
# SD Card support
#==============================================================================
# for SD/MMC card support
WICED_SDMMC_SUPPORT := yes

# USING_FILEX := yes

# default stack size was WICED_DEFAULT_APPLICATION_STACK_SIZE, 6K
# needs big stack size for deep directory processing
GLOBAL_DEFINES += APPLICATION_STACK_SIZE=1024*256

# For exFAT support, use filesystems/FileX/exFAT (for licensees only)
$(NAME)_COMPONENTS := filesystems/FileX/default

# Crypto driver for 43907/54907 platform doesnt support some of AES modes such as XTS and OFB
# Therefore disabling hw crypto since this app uses XTS
GLOBAL_DEFINES += WICED_CONFIG_DONOT_USE_HW_CRYPTO
#==============================================================================
# following is for SD Card Debug strings
#==============================================================================
ifeq ($(BUILD_TYPE),debug)
GLOBAL_DEFINES += BCMDBG
GLOBAL_DEFINES += WPRINT_ENABLE_WICED_DEBUG WPRINT_ENABLE_WICED_ERROR
endif

VALID_OSNS_COMBOS := ThreadX-NetX ThreadX-NetX_Duo
VALID_PLATFORMS := BCM943907AEVAL1F* CYW943907AEVAL1F CYW943907WAE4 BCM943907WCD2 CYW954907AEVAL1F