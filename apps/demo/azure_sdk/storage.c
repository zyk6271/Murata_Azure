#include "wiced.h"
#include "storage.h"
#include "wiced_framework.h"
DEFINE_APP_DCT(user_app_t)
{
    .init_flag   = 1,
    .telemetry_period   = 10,
    .device_id   = "test",
    .primaryKey   = "test",
    .endpointAddress   = "test",
    .mac1  = "test",
};
wiced_result_t dct_app_all_print( void )
{
    user_app_t*       app_dct                  = NULL;

    wiced_dct_read_lock( (void**) &app_dct, WICED_FALSE, DCT_APP_SECTION, 0, sizeof( *app_dct ) );
    WPRINT_APP_INFO( ( "telemetry_value : %d\r\n", (unsigned int)app_dct->telemetry_period ) );
    WPRINT_APP_INFO( ( "device_id : %s\r\n", app_dct->device_id ) );
    WPRINT_APP_INFO( ( "primaryKey : %s\r\n", app_dct->primaryKey ) );
    WPRINT_APP_INFO( ( "endpointAddress : %s\r\n", app_dct->endpointAddress ) );
    WPRINT_APP_INFO( ( "mac1 : %s\r\n", app_dct->mac1 ) );
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
wiced_result_t dct_app_azc_write( user_app_t* app_dct )
{
    wiced_dct_write( (const void*) &app_dct->device_id, DCT_APP_SECTION, OFFSETOF(user_app_t,device_id), sizeof(app_dct->mac1) + sizeof(app_dct->device_id)
                       + sizeof(app_dct->primaryKey) + sizeof(app_dct->endpointAddress) );
    return WICED_SUCCESS;
}
wiced_result_t dct_app_period_write( uint32_t value )
{
    user_app_t       app_dct;
    app_dct.telemetry_period = value;
    wiced_dct_write( &app_dct.telemetry_period, DCT_APP_SECTION, OFFSETOF(user_app_t,telemetry_period), sizeof(app_dct.telemetry_period) );
    return WICED_SUCCESS;
}
