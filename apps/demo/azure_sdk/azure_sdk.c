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
#include "netx_applications/dns/nxd_dns.h"

extern uint8_t wifi_configured;

static void link_down( void *arg)
{
    wifi_disconnect_callback();
}

void application_start( void )
{
    wiced_init();
    mqtt_config_read();
    uart_init();
    if(!wifi_configured)
    {
        network_application_start();
        wiced_network_register_link_callback( NULL, NULL, link_down, NULL, WICED_STA_INTERFACE );
        wiced_network_up( WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL );
    }
    while(1)
    {
        wiced_rtos_delay_milliseconds(1000);
    }
}
