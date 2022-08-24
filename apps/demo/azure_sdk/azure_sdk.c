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

uint8_t ap_flag = 0;
wiced_interface_t interface;
extern uint32_t telemetry_value;
const wiced_ip_setting_t ip_settings =
{
    INITIALISER_IPV4_ADDRESS( .ip_address, MAKE_IPV4_ADDRESS(192,168, 4 ,1) ),
    INITIALISER_IPV4_ADDRESS( .netmask,    MAKE_IPV4_ADDRESS(255,255,255,0) ),
    INITIALISER_IPV4_ADDRESS( .gateway,    MAKE_IPV4_ADDRESS(192,168, 4 ,1) ),
};


wiced_result_t print_wifi_config_dct( void )
{
    platform_dct_wifi_config_t* dct_wifi_config = NULL;

    if ( wiced_dct_read_lock( (void**) &dct_wifi_config, WICED_FALSE, DCT_WIFI_CONFIG_SECTION, 0, sizeof( *dct_wifi_config ) ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }
    /* since we passed ptr_is_writable as WICED_FALSE, we are not allowed to write in to memory pointed by dct_security */

    WPRINT_APP_INFO( ( "\r\n----------------------------------------------------------------\r\n\r\n") );

    /* Wi-Fi Config Section */
    WPRINT_APP_INFO( ( "Wi-Fi Config Section \r\n") );
    WPRINT_APP_INFO( ( "    device_configured               : %d \r\n", dct_wifi_config->device_configured ) );
    WPRINT_APP_INFO( ( "    stored_ap_list[0]  (SSID)       : %s \r\n", dct_wifi_config->stored_ap_list[0].details.SSID.value ) );
    WPRINT_APP_INFO( ( "    stored_ap_list[0]  (Passphrase) : %s \r\n", dct_wifi_config->stored_ap_list[0].security_key ) );
    WPRINT_APP_INFO( ( "    stored_ap_list[0]  (security)   : %d \r\n",   dct_wifi_config->stored_ap_list[0].details.security ) );
    WPRINT_APP_INFO( ( "    soft_ap_settings   (SSID)       : %s \r\n", dct_wifi_config->soft_ap_settings.SSID.value ) );
    WPRINT_APP_INFO( ( "    soft_ap_settings   (Passphrase) : %s \r\n", dct_wifi_config->soft_ap_settings.security_key ) );
    WPRINT_APP_INFO( ( "    DCT mac_address                 : ") );
    print_mac_address( (wiced_mac_t*) &dct_wifi_config->mac_address );
    WPRINT_APP_INFO( ("\r\n") );
    ap_flag = dct_wifi_config->device_configured;
    /* Here ptr_is_writable should be same as what we passed during wiced_dct_read_lock() */
    wiced_dct_read_unlock( dct_wifi_config, WICED_FALSE );

    return WICED_SUCCESS;
}
static wiced_result_t print_app_dct( void )
{
    user_app_t* dct_app = NULL;

    if ( wiced_dct_read_lock( (void**) &dct_app, WICED_FALSE, DCT_APP_SECTION, 0, sizeof( *dct_app ) ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }

    WPRINT_APP_INFO( ( "    telemetry_value              : %u \r\n", (unsigned int)((user_app_t*)dct_app)->telemetry_period ) );

    wiced_dct_read_unlock( dct_app, WICED_FALSE );

    telemetry_value = (unsigned int)((user_app_t*)dct_app)->telemetry_period ;

    return WICED_SUCCESS;
}
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
    print_wifi_config_dct();
    dct_app_all_print();
    //print_app_dct();
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
        keep_alive();
        mqtt_init();
        azure_start();
    }
    while(1)
    {
        wiced_rtos_delay_milliseconds(1000);
    }
}
