#ifndef STORAGE_H
#define STORAGE_H

typedef struct
{
    uint8_t  init_flag ;
    uint32_t  telemetry_period ;
    char     device_id[20];
    char     primaryKey[100];
    char     endpointAddress[100];
    char     mac1[30];
} user_app_t;
wiced_result_t dct_app_all_print( void );
wiced_result_t dct_app_all_read( user_app_t** app_t );
wiced_result_t dct_app_azc_write( user_app_t* app_t );
wiced_result_t dct_app_period_write( uint32_t value );

#endif