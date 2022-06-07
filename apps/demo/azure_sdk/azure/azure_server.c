#include "cloud/iot_sample_common.h"
#include "azure/az_core.h"
#include "azure/az_iot.h"
#include "mqtt_api.h"
#include "resources.h"
#include "azure_server.h"

#define SAMPLE_TYPE PAHO_IOT_HUB
#define SAMPLE_NAME PAHO_IOT_HUB_TWIN_SAMPLE

iot_sample_environment_variables env_vars;
az_iot_hub_client hub_client;
wiced_thread_t azure_serv_t = NULL;

void azure_serv_callback( uint32_t arg )
{
    printf("azure_serv_thread created\n");
    //get_device_twin_document();
    config_get();
    info_get();
    telemetry_init();
    twin_upload();
    while ( 1 )
    {
        wiced_rtos_delay_milliseconds(10);
    }
}
void azure_env_init(void)
{
    int rc;

    iot_sample_read_environment_variables(SAMPLE_TYPE, SAMPLE_NAME, &env_vars);

    char mqtt_endpoint_buffer[128];
    iot_sample_create_mqtt_endpoint(
        SAMPLE_TYPE, &env_vars, mqtt_endpoint_buffer, sizeof(mqtt_endpoint_buffer));

    rc = az_iot_hub_client_init(&hub_client, env_vars.hub_hostname, env_vars.hub_device_id, NULL);
    if (az_result_failed(rc))
    {
      IOT_SAMPLE_LOG_ERROR("Failed to initialize hub client: az_result return code 0x%08x.", rc);
      exit(rc);
    }

}
void azure_start(void)
{
    azure_env_init();
    ntp_set_time_cycle();
    wiced_rtos_create_thread( &azure_serv_t, 2, "azure_serv", azure_serv_callback, 8192, 0 );
}
