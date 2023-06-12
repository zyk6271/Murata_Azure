#include "azure/az_core.h"
#include "azure/az_iot.h"
#include "iot_sample_common.h"
#include "wiced.h"
#include "mqtt_api.h"
#include "resources.h"
#include "telemetry.h"
#include "uart_core.h"
#include "twin_parse.h"
#include "storage.h"

extern az_iot_hub_client hub_client;
extern wiced_mqtt_object_t mqtt_object;
static az_span const type_name = AZ_SPAN_LITERAL_FROM_STR("type");
static az_span const rssi_name = AZ_SPAN_LITERAL_FROM_STR("wfr");
static az_span const data_name = AZ_SPAN_LITERAL_FROM_STR("data");
static az_span const alarm_span = AZ_SPAN_LITERAL_FROM_STR("alarm");
static az_span const event_span = AZ_SPAN_LITERAL_FROM_STR("event");
static az_span const telemetry_span = AZ_SPAN_LITERAL_FROM_STR("telemetry");
static az_span const timestamp_span = AZ_SPAN_LITERAL_FROM_STR("timestamp");
static az_span const deviceid_span = AZ_SPAN_LITERAL_FROM_STR("deviceId");

void telemetry_request(void)
{
    unsigned short send_len = 0;
    send_len = set_wifi_uart_byte(send_len,1);
    wifi_uart_write_frame(TELEMETRY_CONTROL_CMD, MCU_TX_VER, send_len);
}
void telemetry_reponse(void)
{
    unsigned short send_len = 0;
    send_len = set_wifi_uart_byte(send_len,2);
    wifi_uart_write_frame(TELEMETRY_CONTROL_CMD, MCU_TX_VER, send_len);
}
void telemetry_upload(unsigned char* data_buf,unsigned short data_len)
{
    uint8_t device_buffer[32];
    char payload_buffer[512];
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

    dct_read_device_id(device_buffer);
    az_span device_data = az_span_create(data_buf,data_len-1);

    az_json_writer_init(&jw, payload, NULL);
    az_json_writer_append_begin_object(&jw);
    az_json_writer_append_property_name(&jw, type_name);
    az_json_writer_append_string(&jw,telemetry_span);
    az_json_writer_append_property_name(&jw, timestamp_span);
    az_json_writer_append_int32(&jw,get_time());

    az_json_writer_append_property_name(&jw, deviceid_span);
    az_json_writer_append_string(&jw,az_span_create_from_str(device_buffer));

    az_json_writer_append_property_name(&jw, data_name);
    az_json_writer_append_json_text(&jw,device_data);
    az_json_writer_append_property_name(&jw, rssi_name);
    az_json_writer_append_int32(&jw,sta_rssi_get());

    az_json_writer_append_end_object(&jw);
    out_payload = az_json_writer_get_bytes_used_in_destination(&jw);
    out_payload._internal.ptr[out_payload._internal.size] = '}';
    out_payload._internal.size += 1;
    if(wifi_status_get() == 4)
    {
        wiced_mqtt_publish(mqtt_object,topic_buffer,az_span_ptr(out_payload),az_span_size(out_payload),0);
        printf("telemetry_upload,payload is %s\r\n",az_span_ptr(out_payload));
    }
    telemetry_reponse();
}

void alarm_upload(unsigned char* data_buf,unsigned short data_len)
{
    uint8_t device_buffer[32];
    char payload_buffer[512] = {0};
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

    az_span device_data = az_span_create(data_buf,data_len-1);

    az_json_writer_init(&jw, payload, NULL);
    az_json_writer_append_begin_object(&jw);
    az_json_writer_append_property_name(&jw, type_name);
    az_json_writer_append_string(&jw,alarm_span);
    az_json_writer_append_property_name(&jw, timestamp_span);
    az_json_writer_append_int32(&jw,get_time());
    az_json_writer_append_property_name(&jw, rssi_name);
    az_json_writer_append_int32(&jw,sta_rssi_get());
    dct_read_device_id(device_buffer);
    az_json_writer_append_property_name(&jw, deviceid_span);
    az_json_writer_append_string(&jw,az_span_create_from_str(device_buffer));
    az_json_writer_append_property_name(&jw, data_name);
    az_json_writer_append_json_text(&jw,device_data);

    az_json_writer_append_end_object(&jw);
    out_payload = az_json_writer_get_bytes_used_in_destination(&jw);
    out_payload._internal.ptr[out_payload._internal.size] = '}';
    out_payload._internal.size += 1;
    printf("alarm_upload,payload is %s\r\n",az_span_ptr(out_payload));
    if(wifi_status_get() == 4)
    {
        wiced_mqtt_publish(mqtt_object,topic_buffer,az_span_ptr(out_payload),az_span_size(out_payload),0);
    }
}

void event_upload(unsigned char* data_buf,unsigned short data_len)
{
    uint8_t device_buffer[32];
    char payload_buffer[256] = {0};
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

    az_span device_data = az_span_create(data_buf,data_len-1);

    az_json_writer_init(&jw, payload, NULL);
    az_json_writer_append_begin_object(&jw);
    az_json_writer_append_property_name(&jw, type_name);
    az_json_writer_append_string(&jw,event_span);
    az_json_writer_append_property_name(&jw, timestamp_span);
    az_json_writer_append_int32(&jw,get_time());
    az_json_writer_append_property_name(&jw, rssi_name);
    az_json_writer_append_int32(&jw,sta_rssi_get());

    dct_read_device_id(device_buffer);
    az_json_writer_append_property_name(&jw, deviceid_span);
    az_json_writer_append_string(&jw,az_span_create_from_str(device_buffer));
    az_json_writer_append_property_name(&jw, data_name);
    az_json_writer_append_json_text(&jw,device_data);

    az_json_writer_append_end_object(&jw);
    out_payload = az_json_writer_get_bytes_used_in_destination(&jw);
    out_payload._internal.ptr[out_payload._internal.size] = '}';
    out_payload._internal.size += 1;
    printf("event_upload,payload is %s\r\n",az_span_ptr(out_payload));
    if(wifi_status_get() == 4)
    {
        wiced_mqtt_publish(mqtt_object,topic_buffer,az_span_ptr(out_payload),az_span_size(out_payload),0);
    }
}
