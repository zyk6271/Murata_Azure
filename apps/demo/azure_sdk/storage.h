#ifndef STORAGE_H
#define STORAGE_H

wiced_result_t print_wifi_config_dct( void );
wiced_result_t dct_app_load( void );
wiced_result_t dct_app_all_read( platform_dct_azure_config_t** app_t );
wiced_result_t dct_read_first_ap_ssid_name(uint8_t* ssid);
wiced_result_t dct_app_azc_write( platform_dct_azure_config_t* app_t );
wiced_result_t dct_app_period_write( uint32_t value );

#endif
