// Copyright (c) Microsoft Corporation. All rights reserved.
// SPDX-License-Identifier: MIT

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <mqtt_api.h>
#include "wiced.h"
#include "azure_server.h"
#include "storage.h"
#include "sntp.h"

extern uint8_t azure_flag;
wiced_interface_t interface;

void link_up_callback(void)
{
    wifi_status_change(2);
    sntp_start_auto_time_sync_nowait( 1000*60*60 );
    keep_alive();
    mqtt_connect_azure();
}
static void link_down( void *arg)
{
    wifi_status_change(1);
    wifi_disconnect_callback();
}
void application_start( void )
{
    wiced_init();
    dct_app_load();
    print_wifi_config_dct();
    uart_init();
    wifi_status_change(0);
    http_ota_init();
    wifi_watch_init();
    if(!azure_flag)
    {
        mqtt_watch_init();
        azure_start();
        wiced_network_register_link_callback( NULL, NULL, link_down, NULL, WICED_STA_INTERFACE );
        wiced_network_up( WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL );
    }
    while(1)
    {
        wiced_rtos_delay_milliseconds(1000);
    }
}
