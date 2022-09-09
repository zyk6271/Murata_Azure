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
wiced_thread_t azure_serv_t = NULL;

wiced_semaphore_t azure_refresh_sem;

void azure_refresh(void)
{
    wiced_rtos_set_semaphore(&azure_refresh_sem);
}
void azure_serv_callback( uint32_t arg )
{
    telemetry_init();
    while ( 1 )
    {
        wiced_rtos_get_semaphore( &azure_refresh_sem, WICED_WAIT_FOREVER );
        get_device_twin_document();
        telemetry_upload();
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
void azure_start(void)
{
    wiced_rtos_init_semaphore( &azure_refresh_sem );
    mqtt_config_read();
    azure_env_init();
    wiced_rtos_create_thread( &azure_serv_t, 2, "azure_serv", azure_serv_callback, 8192, 0 );
}
