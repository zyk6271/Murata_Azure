/* Copyright (c) 2017 Arrow Electronics, Inc.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Apache License 2.0
 * which accompanies this distribution, and is available at
 * http://apache.org/licenses/LICENSE-2.0
 * Contributors: Arrow Electronics, Inc.
 */
#include "wiced.h"
#include "ntp.h"
#include "wiced_time.h"

wiced_utc_time_t get_time(void) {
    wiced_utc_time_t utc;
    wiced_time_get_utc_time(&utc);
    return utc;
}
