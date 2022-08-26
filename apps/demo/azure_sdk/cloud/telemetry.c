#include "azure/az_core.h"
#include "azure/az_iot.h"
#include "iot_sample_common.h"
#include "wiced.h"
#include "mqtt_api.h"
#include "resources.h"
#include "telemetry.h"
#include "uart_core.h"
#include "twin_parse.h"
#include "dct_read_write_dct.h"
#include "storage.h"

wiced_timer_t telemetry_timer;
uint32_t telemetry_value;

extern az_iot_hub_client hub_client;
extern wiced_mqtt_object_t mqtt_object;
extern syr_status device_status;

extern EventGroupHandle_t TEM_EventHandler;

static az_span const type_name = AZ_SPAN_LITERAL_FROM_STR("type");
static az_span const data_name = AZ_SPAN_LITERAL_FROM_STR("data");
static az_span const telemetry_span = AZ_SPAN_LITERAL_FROM_STR("telemetry");
static az_span const timestamp_span = AZ_SPAN_LITERAL_FROM_STR("timestamp");
static az_span const net_name = AZ_SPAN_LITERAL_FROM_STR("net");
static az_span const bat_name = AZ_SPAN_LITERAL_FROM_STR("bat");
static az_span const ala_name = AZ_SPAN_LITERAL_FROM_STR("ala");
static az_span const alr_name = AZ_SPAN_LITERAL_FROM_STR("alr");

void telemetry_upload(void)
{
    EventBits_t EventValue;
    char payload_buffer[2048];
    az_span payload = AZ_SPAN_FROM_BUFFER(payload_buffer);
    az_span out_payload;
    az_json_writer jw;

    char topic_buffer[128];
    az_iot_hub_client_telemetry_get_publish_topic(
        &hub_client,
        NULL,
        topic_buffer,
        sizeof(topic_buffer),
        NULL);

    az_json_writer_init(&jw, payload, NULL);
    az_json_writer_append_begin_object(&jw);
    az_json_writer_append_property_name(&jw, type_name);
    az_json_writer_append_string(&jw,telemetry_span);
    az_json_writer_append_property_name(&jw, timestamp_span);
    az_json_writer_append_int32(&jw,get_time());
    az_json_writer_append_property_name(&jw, data_name);
    az_json_writer_append_begin_object(&jw);

    wifi_uart_write_command_value(NET_GET_CMD,0);
    EventValue = xEventGroupWaitBits(TEM_EventHandler,EVENT_TEM_NET_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_TEM_NET_GET)
    {
        IOT_SAMPLE_LOG_SUCCESS("GET NET %d\r\n",device_status.tem.net);
        az_json_writer_append_property_name(&jw, net_name);
        az_json_writer_append_int32(&jw,device_status.tem.net);
    }
    wifi_uart_write_command_value(BAT_GET_CMD,0);
    EventValue = xEventGroupWaitBits(TEM_EventHandler,EVENT_TEM_BAT_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_TEM_BAT_GET)
    {
        IOT_SAMPLE_LOG_SUCCESS("GET BAT %d\r\n",device_status.tem.bat);
        az_json_writer_append_property_name(&jw, bat_name);
        az_json_writer_append_int32(&jw,device_status.tem.bat);
    }
    wifi_uart_write_command_value(ALA_GET_CMD,0);
    EventValue = xEventGroupWaitBits(TEM_EventHandler,EVENT_TEM_ALA_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_TEM_ALA_GET)
    {
        IOT_SAMPLE_LOG_SUCCESS("GET ALA %d\r\n",device_status.tem.ala);
        az_json_writer_append_property_name(&jw, ala_name);
        az_json_writer_append_int32(&jw,device_status.tem.ala);
    }
    wifi_uart_write_command_value(ALR_GET_CMD,0);
    EventValue = xEventGroupWaitBits(TEM_EventHandler,EVENT_TEM_ALR_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_TEM_ALR_GET)
    {
        IOT_SAMPLE_LOG_SUCCESS("GET ALR %d\r\n",device_status.tem.alr);
        az_json_writer_append_property_name(&jw, alr_name);
        az_json_writer_append_int32(&jw,device_status.tem.alr);
    }
    az_json_writer_append_end_object(&jw);
    az_json_writer_append_end_object(&jw);
    out_payload = az_json_writer_get_bytes_used_in_destination(&jw);
    wiced_mqtt_publish(mqtt_object,topic_buffer,az_span_ptr(out_payload),az_span_size(out_payload),0);
    printf("telemetry_upload,payload is %s\r\n",az_span_ptr(out_payload));
}
void telemetry_init(void)
{
    user_app_t *app_t = calloc(sizeof(user_app_t), sizeof(char));
    dct_app_all_read(&app_t);
    telemetry_value = app_t->telemetry_period;
    if(telemetry_value == 0 || telemetry_value >= 1440)
    {
        telemetry_value = 10;
        dct_app_period_write(telemetry_value);
        printf("telemetry default to 10\r\n");
    }
    wiced_rtos_init_timer(&telemetry_timer,telemetry_value * 60 * 1000,telemetry_upload,0);
    wiced_rtos_start_timer(&telemetry_timer);
    free(app_t);
}

void telemetry_period_set(uint32_t value)
{
    if(value==0)return;
    wiced_rtos_deinit_timer(&telemetry_timer);
    wiced_rtos_init_timer(&telemetry_timer,telemetry_value * 60 * 1000,telemetry_upload,0);
    wiced_rtos_start_timer(&telemetry_timer);
    dct_app_period_write(value);
    printf("telemetry_period_set,period is %d\r\n",telemetry_value);
}
