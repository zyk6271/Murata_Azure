#include "wiced.h"
#include "storage.h"
#include "wiced_framework.h"
#include "twin_parse.h"

uint8_t ap_flag = 0;
extern syr_status device_status;

char syr_version[]="1.0.4";

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
    wiced_dct_read_unlock( dct_wifi_config, WICED_FALSE );

    return WICED_SUCCESS;
}
wiced_result_t dct_app_load( void )
{
    user_app_t*       app_dct                  = NULL;

    wiced_dct_read_lock( (void**) &app_dct, WICED_FALSE, DCT_APP_SECTION, 0, sizeof( *app_dct ) );
    if(app_dct->init_flag==1)
    {
        WPRINT_APP_INFO( ( "telemetry_value : %d\r\n", (unsigned int)app_dct->telemetry_period ) );
        WPRINT_APP_INFO( ( "device_id : %s\r\n", app_dct->device_id ) );
        WPRINT_APP_INFO( ( "primaryKey : %s\r\n", app_dct->primaryKey ) );
        WPRINT_APP_INFO( ( "endpointAddress : %s\r\n", app_dct->endpointAddress ) );
        WPRINT_APP_INFO( ( "mac1 : %s\r\n", app_dct->mac1 ) );
        wiced_mac_t mac_read;
        int values[6];
        if(6 == sscanf(app_dct->mac1, "%x:%x:%x:%x:%x:%x%*c",
            &values[0], &values[1], &values[2],
            &values[3], &values[4], &values[5]))
        {
            for(uint8_t i = 0; i < 6; ++i)
            mac_read.octet[i] = (uint8_t) values[i];
        }
        wwd_wifi_set_mac_address(mac_read,WWD_AP_INTERFACE);
        wwd_wifi_set_mac_address(mac_read,WWD_STA_INTERFACE);
        wiced_dct_write( &mac_read, DCT_WIFI_CONFIG_SECTION, OFFSETOF(platform_dct_wifi_config_t,mac_address), sizeof(wiced_mac_t) );
        strncpy(device_status.info.srn,app_dct->device_id,strlen(app_dct->device_id));
    }
    strncpy(device_status.info.ver,syr_version,6);
    wiced_dct_read_unlock( app_dct, WICED_FALSE );
    return WICED_SUCCESS;
}
wiced_result_t dct_app_all_read( user_app_t** app_dct )
{
    user_app_t*       app_dct_origin                  = NULL;
    wiced_dct_read_lock( (void **)&app_dct_origin, WICED_TRUE, DCT_APP_SECTION, 0, sizeof( *app_dct_origin ) );
    memcpy(*app_dct,app_dct_origin,sizeof(user_app_t));
    wiced_dct_read_unlock( app_dct_origin, WICED_TRUE );
    return WICED_SUCCESS;
}
wiced_result_t dct_app_period_write( uint32_t value )
{
    user_app_t       app_dct;
    app_dct.telemetry_period = value;
    wiced_dct_write( &app_dct.telemetry_period, DCT_APP_SECTION, OFFSETOF(user_app_t,telemetry_period), sizeof(app_dct.telemetry_period) );
    return WICED_SUCCESS;
}
wiced_result_t dct_app_init_write(void)
{
    user_app_t       app_dct;
    app_dct.init_flag = 1;
    wiced_dct_write( &app_dct.init_flag, DCT_APP_SECTION, OFFSETOF(user_app_t,init_flag), sizeof(app_dct.init_flag) );
    return WICED_SUCCESS;
}
wiced_result_t dct_app_azc_write( user_app_t* app_dct )
{
    dct_app_init_write();
    wiced_dct_write( (const void*) &app_dct->device_id, DCT_APP_SECTION, OFFSETOF(user_app_t,device_id), sizeof(app_dct->mac1) + sizeof(app_dct->device_id)
                       + sizeof(app_dct->primaryKey) + sizeof(app_dct->endpointAddress) );
    return WICED_SUCCESS;
}

