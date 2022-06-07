/* Copyright (c) 2017 Arrow Electronics, Inc.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Apache License 2.0
 * which accompanies this distribution, and is available at
 * http://apache.org/licenses/LICENSE-2.0
 * Contributors: Arrow Electronics, Inc.
 */

#ifndef ACN_SDK_C_NTP_CONFIG_H_
#define ACN_SDK_C_NTP_CONFIG_H_

/* NTP server settings */
#define NTP_DEFAULT_SERVER  "ntp.aliyun.com"
#define NTP_DEFAULT_PORT    123
#define NTP_DEFAULT_TIMEOUT 4000

int ntp_set_time_common(const char *server, uint16_t port, int timeout, int try_times);
int set_time(int timer);
int get_time(void);

#endif // ACN_SDK_C_NTP_CONFIG_H_
