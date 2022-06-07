/*
 * Copyright 2020, Cypress Semiconductor Corporation or a subsidiary of 
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
 * so agrees to indemnify Cypress against all liability.
 */

/**
 * @file Wiced log routines.
 *
 * A logging subsystem that allows run time control for the logging level.
 * Log messages are passed back to the application for output.
 */

#pragma once

#include <stdarg.h>
#include "wiced_result.h"
#include "wiced_time.h"

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************
 *                      Macros
 ******************************************************/

#define WICED_LOG_MSG_TEST(...) wiced_log_msg(WLF_TEST, WICED_LOG_DEBUG4, __VA_ARGS__)

/******************************************************
 *                    Constants
 ******************************************************/

/******************************************************
 *                   Enumerations
 ******************************************************/

/**
 * Logging levels.
 */
typedef enum
{
    WICED_LOG_OFF = 0,
    WICED_LOG_ERR,
    WICED_LOG_WARNING,
    WICED_LOG_NOTICE,
    WICED_LOG_INFO,
    WICED_LOG_DEBUG0,
    WICED_LOG_DEBUG1,
    WICED_LOG_DEBUG2,
    WICED_LOG_DEBUG3,
    WICED_LOG_DEBUG4,

    WICED_LOG_PRINTF, /* Identifies log messages generated by wiced_log_printf calls */

    WICED_LOG_MAX
} WICED_LOG_LEVEL_T;

typedef enum
{
    WLF_DEF = 0,                    /* General log message not associated with any specific facility */
    WLF_AUDIO,
    WLF_TEST,
    WLF_DUKTAPE,
    WLF_AVS_CLIENT,
    WLF_OTA2,
    WLF_TSFILTER,
    WLF_DRIVER,

    WLF_MAX                         /* Must be last */
} WICED_LOG_FACILITY_T;

/******************************************************
 *                 Type Definitions
 ******************************************************/

typedef int (*log_output)(WICED_LOG_LEVEL_T level, char *logmsg);
typedef wiced_result_t (*platform_get_time)(wiced_time_t* time);

/******************************************************
 *                    Structures
 ******************************************************/
/*****************************************************************************/
/**
 *
 *  @defgroup utilities     WICED Utilities
 *  @ingroup  utilities
 *
 *  WICED Utility libraries and helper functions for commonly used tasks.
 *
 *
 *  @addtogroup log     Logging
 *  @ingroup utilities
 *
 * This library implements the WICED Logging API
 *
 *  @{
 */
/*****************************************************************************/
/******************************************************
 *               Function Declarations
 ******************************************************/
/**
 * Initialize the logging subsystem.
 *
 * @note If platform_output is NULL, log messages will be discarded.
 *       If platform_time is NULL, wiced_time_get_time() is used for time stamps.
 *
 * @param[in] level           : The initial logging level to use for all facilities.
 * @param[in] platform_output : Pointer to the platform output routine for log messages.
 * @param[in] platform_time   : Optional pointer to a platform time routine for log message time stamps.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_log_init(WICED_LOG_LEVEL_T level, log_output platform_output, platform_get_time platform_time);

/**
 * Shutdown the logging subsystem.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_log_shutdown(void);

/**
 * Set the platform output routine for log messages.
 *
 * @note If platform_output is NULL, log messages will be discarded.
 *
 * @param[in] platform_output : Pointer to the platform output routine for log messages.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_log_set_platform_output(log_output platform_output);

/**
 * Set the platform routine for getting time stamps for log messages.
 *
 * @note If platform_time is NULL, wiced_time_get_time() is used for time stamps.
 *
 * @param[in] platform_time   : Pointer to a platform time routine for log message time stamps.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_log_set_platform_time(platform_get_time platform_time);

/**
 * Set the logging level for a facility.
 *
 * @param[in] facility  : The facility for which to set the log level.
 * @param[in] level     : The new log level to use.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_log_set_facility_level(WICED_LOG_FACILITY_T facility, WICED_LOG_LEVEL_T level);

/**
 * Set the logging level for all facilities.
 *
 * @param[in] level  : The new log level to use.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_log_set_all_levels(WICED_LOG_LEVEL_T level);

/**
 * Get the logging level for a facility.
 *
 * @param[in] facility  : The facility for which to return the log level.
 *
 * @return The current log level.
 */
WICED_LOG_LEVEL_T wiced_log_get_facility_level(WICED_LOG_FACILITY_T facility);

/**
 * Write a log message.
 *
 * @note The format arguments are the same as for printf.
 *
 * @param[in] facility  : The facility for the log message.
 * @param[in] level     : Log level of the message.
 * @param[in] fmt       : Format control string followed by any optional arguments.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_log_msg(WICED_LOG_FACILITY_T facility, WICED_LOG_LEVEL_T level, const char *fmt, ...);

/**
 * Write a log message bypassing the log level check.
 *
 * @note The format arguments are the same as for printf.
 *
 * @param[in] fmt    : Format control string followed by any optional arguments.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_log_printf(const char *fmt, ...);

/**
 * Write a log message bypassing the log level check.
 *
 * @note The format arguments are the same as for vprintf.
 *
 * @param[in] fmt   : Format control string.
 * @param[in] varg  : va_list of arguments.
 *
 * @return @ref wiced_result_t
 */
wiced_result_t wiced_log_vprintf(const char *fmt, va_list varg);

/** @} */

#ifdef __cplusplus
} /* extern "C" */
#endif
