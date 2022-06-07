/* Copyright (c) 2017 Arrow Electronics, Inc.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Apache License 2.0
 * which accompanies this distribution, and is available at
 * http://apache.org/licenses/LICENSE-2.0
 * Contributors: Arrow Electronics, Inc.
 */
#include "wiced.h"
#include "ntp.h"
#include "client.h"
#include "time.h"

platform_rtc_time_t rtc_time;

void stamp_to_rtc(int stampTime)
{
    time_t tick = (time_t)stampTime;
    struct tm tm;
    char s[100];
    tm = *localtime(&tick);
    strftime(s, sizeof(s), "%Y-%m-%d %H:%M:%S", &tm);
    rtc_time.year = atoi(s)-1900;
    rtc_time.month = atoi(s+5)-1;
    rtc_time.date = atoi(s+8);
    rtc_time.hr = atoi(s+11);
    rtc_time.min = atoi(s+14);
    rtc_time.sec = atoi(s+17);
    rtc_time.weekday = 0;
}
int rtc_to_stamp(void)
{
    struct tm stm;
    platform_rtc_time_t rtc;
    memset(&stm,0,sizeof(stm));
    platform_rtc_get_time(&rtc);
    stm.tm_year=rtc.year;
    stm.tm_mon=rtc.month;
    stm.tm_mday=rtc.date;
    stm.tm_hour=rtc.hr;
    stm.tm_min=rtc.min;
    stm.tm_sec=rtc.sec;
    return mktime(&stm);
}
int set_time(int timer) {
    stamp_to_rtc(timer);
    platform_rtc_set_time(&rtc_time);
    return WICED_SUCCESS;
}
int get_time(void) {
    return rtc_to_stamp();
}
int ntp_set_time_cycle(void) {
    return ntp_set_time(NTP_DEFAULT_SERVER, NTP_DEFAULT_PORT, NTP_DEFAULT_TIMEOUT);
}
