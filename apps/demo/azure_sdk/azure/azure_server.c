#include "cloud/iot_sample_common.h"
#include "azure/az_core.h"
#include "azure/az_iot.h"
#include "mqtt_api.h"
#include "resources.h"
#include "azure_server.h"
#include "base64.h"
#include "telemetry.h"

#define SAMPLE_TYPE PAHO_IOT_HUB
#define SAMPLE_NAME PAHO_IOT_HUB_TWIN_SAMPLE

iot_sample_environment_variables env_vars;
az_iot_hub_client hub_client;
wiced_thread_t azure_serv_t;

wiced_semaphore_t azure_refresh_sem;

void azure_refresh(void)
{
    wiced_rtos_set_semaphore(&azure_refresh_sem);
}
void azure_serv_callback( uint32_t arg )
{
    wiced_rtos_init_semaphore( &azure_refresh_sem );
    azure_env_init();
    mqtt_init();
    while ( 1 )
    {
        wiced_rtos_get_semaphore( &azure_refresh_sem, WICED_WAIT_FOREVER );
        config_get();
        info_get();
        wiced_rtos_delay_milliseconds(1000);
        twin_upload();
        telemetry_request();
        wiced_rtos_delay_milliseconds(2000);
        wifi_status_change(4);
    }
}
void azure_env_init(void)
{
    int rc;

    iot_sample_read_environment_variables(SAMPLE_TYPE, SAMPLE_NAME, &env_vars);

    rc = az_iot_hub_client_init(&hub_client, env_vars.hub_hostname, env_vars.hub_device_id, NULL);
    if (az_result_failed(rc))
    {
      IOT_SAMPLE_LOG_ERROR("Failed to initialize hub client: az_result return code 0x%08x.", rc);
      exit(rc);
    }

}
void network_application_start(void)
{
    http_ota_init();
    wifi_watch_init();
    mqtt_watch_init();
    wiced_rtos_create_thread( &azure_serv_t, 7, "azure_serv", azure_serv_callback, 8192, 0 );
}
