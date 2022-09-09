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
#include "dct_read_write_dct.h"
#include "storage.h"
#include "sntp.h"

extern uint8_t ap_flag;
wiced_interface_t interface;

const wiced_ip_setting_t ip_settings =
{
    INITIALISER_IPV4_ADDRESS( .ip_address, MAKE_IPV4_ADDRESS(192,168, 4 ,1) ),
    INITIALISER_IPV4_ADDRESS( .netmask,    MAKE_IPV4_ADDRESS(255,255,255,0) ),
    INITIALISER_IPV4_ADDRESS( .gateway,    MAKE_IPV4_ADDRESS(192,168, 4 ,1) ),
};
static void link_up( void *arg)
{
    UNUSED_PARAMETER(arg);
    WPRINT_APP_INFO( ("Network connection is up\n") );
}
static void link_down( void *arg)
{
    UNUSED_PARAMETER(arg);
    WPRINT_APP_INFO( ("Network connection is down\n") );

    wiced_network_down(WICED_STA_INTERFACE);
    wifi_restart_release();
}
void application_start( void )
{
    wiced_init();
    uart_init();
    wifi_status_change(0);
    dct_app_load();
    print_wifi_config_dct();
    if(ap_flag)
    {
        wiced_network_up(WICED_AP_INTERFACE, WICED_USE_INTERNAL_DHCP_SERVER, &ip_settings);
        http_start();
    }
    else
    {
        wiced_network_register_link_callback( link_up, NULL, link_down, NULL, WICED_STA_INTERFACE );
        while(wiced_network_up( WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL ) != WICED_SUCCESS)
        {
            WPRINT_APP_INFO( ("wiced_network_up retry\n") );
            wiced_rtos_delay_milliseconds(1000);
        }
        wifi_watch_init();
        mqtt_watch_init();
        sntp_start_auto_time_sync_nowait( 1000*60*60 );
        azure_start();
        mqtt_init();
        keep_alive();
    }
    while(1)
    {
        wiced_rtos_delay_milliseconds(1000);
    }
}
