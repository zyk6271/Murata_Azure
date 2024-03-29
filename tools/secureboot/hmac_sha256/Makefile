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
# Makefile for gcc
CC = mingw32-gcc.exe
RM = del

# Uncomment below to compile in Linux
# CC = gcc
# RM = rm -f

CFLAGS = -g

SRCBASE = ../../../
TOOLCHAIN_NAME = GCC

VPATH = $(SRCBASE)WICED/security/BESL/mbedtls_open $(SRCBASE)/WICED/security/BESL/mbedtls_open/library

INC += -I$(SRCBASE)WICED/security/BESL/mbedtls_open/include/mbedtls
INC += -I$(SRCBASE)WICED/security/BESL/mbedtls_open/include
INC += -I$(SRCBASE)WICED/security/BESL/crypto_internal
INC += -I$(SRCBASE)WICED/security/BESL/include

# The following includes are from WICED/security/BESL/Makefile
INC += -I$(SRCBASE)WICED/WWD/include
INC += -I$(SRCBASE)WICED/platform/include
INC += -I$(SRCBASE)WICED
INC += -I$(SRCBASE)WICED/platform/$(TOOLCHAIN_NAME)
INC += -I$(SRCBASE)include
INC += -I$(SRCBASE)libraries/utilities/ring_buffer
INC += -I$(SRCBASE)libraries/utilities/base64
INC += -I$(SRCBASE)libraries/utilities/TLV
INC += -I$(SRCBASE)WICED/security/BESL/crypto_internal/micro-ecc
INC += -I$(SRCBASE)libraries/utilities/linked_list
INC += -I$(SRCBASE)tools/prebuild_stubs

TARGET = hmac_sha256

OBJS = hmac_sha256.o sha256.o

$(TARGET) : $(OBJS)
	$(CC) -o $(TARGET) $(OBJS)

.c.o :
	$(CC) $(INC) $(CFLAGS) -c $< -o $@

clean :
	$(RM) $(TARGET) *.o *.exe core
