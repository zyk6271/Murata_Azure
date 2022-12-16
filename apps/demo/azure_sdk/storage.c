#include "wiced.h"
#include "storage.h"
#include "wiced_framework.h"
#include "twin_parse.h"

#include "platform_config.h"
#include "platform_dct.h"
#include "wiced_defaults.h"
#include "wiced_result.h"
#include "wiced_apps_common.h"

uint8_t azure_flag = 0;
extern syr_status device_status;

char wifi_version[]={"1.1.1"};

wiced_result_t print_wifi_config_dct( void )
{
    platform_dct_wifi_config_t* dct_wifi_config = NULL;

    if ( wiced_dct_read_lock( (void**) &dct_wifi_config, WICED_FALSE, DCT_WIFI_CONFIG_SECTION, 0, sizeof( *dct_wifi_config ) ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }
    /* since we passed ptr_is_writable as WICED_FALSE, we are not allowed to write in to memory pointed by dct_security */
    WPRINT_APP_INFO( ( "----------------------------------------------------------------\n") );
    WPRINT_APP_INFO( ( "------------------------Version is %s-----------------------\n",wifi_version) );
    WPRINT_APP_INFO( ( "----------------------------------------------------------------\n") );

    /* Wi-Fi Config Section */
    WPRINT_APP_INFO( ( "Wi-Fi Config Section \r\n") );
    WPRINT_APP_INFO( ( "    device_configured               : %d \r\n", dct_wifi_config->device_configured ) );
    WPRINT_APP_INFO( ( "    stored_ap_list[0]  (SSID)       : %s \r\n", dct_wifi_config->stored_ap_list[0].details.SSID.value ) );
    WPRINT_APP_INFO( ( "    stored_ap_list[0]  (Passphrase) : %s \r\n", dct_wifi_config->stored_ap_list[0].security_key ) );
    WPRINT_APP_INFO( ( "    stored_ap_list[0]  (Passphrase_length) : %d \r\n", dct_wifi_config->stored_ap_list[0].security_key_length ) );
    WPRINT_APP_INFO( ( "    stored_ap_list[0]  (security)   : %d \r\n",   dct_wifi_config->stored_ap_list[0].details.security ) );
    WPRINT_APP_INFO( ( "    soft_ap_settings   (SSID)       : %s \r\n", dct_wifi_config->soft_ap_settings.SSID.value ) );
    WPRINT_APP_INFO( ( "    soft_ap_settings   (length)     : %d \r\n", dct_wifi_config->soft_ap_settings.SSID.length ) );
    WPRINT_APP_INFO( ( "    soft_ap_settings   (Passphrase) : %s \r\n", dct_wifi_config->soft_ap_settings.security_key ) );
    WPRINT_APP_INFO( ( "    DCT mac_address                 : ") );
    print_mac_address( (wiced_mac_t*) &dct_wifi_config->mac_address );
    WPRINT_APP_INFO( ("\r\n") );
    azure_flag = dct_wifi_config->device_configured;
    wiced_dct_read_unlock( dct_wifi_config, WICED_FALSE );

    return WICED_SUCCESS;
}
wiced_result_t dct_app_all_read( platform_dct_azure_config_t** app_dct )
{
    platform_dct_azure_config_t*       app_dct_origin                  = NULL;
    wiced_dct_read_lock( (void **)&app_dct_origin, WICED_TRUE, DCT_AZURE_SECTION, 0, sizeof( platform_dct_azure_config_t ) );
    memcpy(*app_dct,app_dct_origin,sizeof(platform_dct_azure_config_t));
    wiced_dct_read_unlock( app_dct_origin, WICED_TRUE );
    return WICED_SUCCESS;
}
wiced_result_t dct_app_azc_write( platform_dct_azure_config_t* app_dct )
{
    char ap_ssid[28];
    wiced_dct_write( (const void*) &app_dct->init_flag, DCT_AZURE_SECTION, OFFSETOF(platform_dct_azure_config_t,init_flag), sizeof(app_dct->init_flag));
    wiced_dct_write( (const void*) &app_dct->device_id, DCT_AZURE_SECTION, OFFSETOF(platform_dct_azure_config_t,device_id), sizeof(app_dct->device_id));
    wiced_dct_write( (const void*) &app_dct->endpointAddress, DCT_AZURE_SECTION, OFFSETOF(platform_dct_azure_config_t,endpointAddress), sizeof(app_dct->endpointAddress));
    wiced_dct_write( (const void*) &app_dct->primaryKey, DCT_AZURE_SECTION, OFFSETOF(platform_dct_azure_config_t,primaryKey), sizeof(app_dct->primaryKey));
    wiced_dct_write( (const void*) &app_dct->mac1, DCT_AZURE_SECTION, OFFSETOF(platform_dct_azure_config_t,mac1), sizeof(app_dct->mac1));
    strcpy(ap_ssid,"SYR");
    strcat(ap_ssid,app_dct->device_id);
    uint8_t size = strlen(ap_ssid);
    wiced_dct_write( ap_ssid, DCT_WIFI_CONFIG_SECTION, OFFSETOF(platform_dct_wifi_config_t,soft_ap_settings.SSID.value), 32 );
    wiced_dct_write(&size, DCT_WIFI_CONFIG_SECTION, OFFSETOF(platform_dct_wifi_config_t,soft_ap_settings.SSID.length), 1 );
    wiced_mac_t mac_read;
    int values[6];
    if(6 == sscanf(app_dct->mac1, "%x:%x:%x:%x:%x:%x%*c",
        &values[0], &values[1], &values[2],
        &values[3], &values[4], &values[5]))
    {
        for(uint8_t i = 0; i < 6; ++i)
        mac_read.octet[i] = (uint8_t) values[i];
    }
    wiced_dct_write( &mac_read, DCT_WIFI_CONFIG_SECTION, OFFSETOF(platform_dct_wifi_config_t,mac_address), sizeof(wiced_mac_t) );
    return WICED_SUCCESS;
}
