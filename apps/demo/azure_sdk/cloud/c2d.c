#include "azure/az_core.h"
#include "azure/az_iot.h"
#include "iot_sample_common.h"
#include "wiced.h"
#include "mqtt_api.h"
#include "resources.h"
#include "ntp.h"
#include "telemetry.h"
#include "uart_core.h"
#include "twin_parse.h"

extern az_iot_hub_client hub_client;
extern wiced_mqtt_object_t mqtt_object;
extern wiced_event_flags_t C2D_EventHandler;

static az_span const type_name = AZ_SPAN_LITERAL_FROM_STR("type");
static az_span const rst_name = AZ_SPAN_LITERAL_FROM_STR("rstStart");
static az_span const def_name = AZ_SPAN_LITERAL_FROM_STR("defStart");
static az_span const ras_name = AZ_SPAN_LITERAL_FROM_STR("rasStart");
static az_span const typ_span = AZ_SPAN_LITERAL_FROM_STR("typ");
static az_span const event_span = AZ_SPAN_LITERAL_FROM_STR("event");
static az_span const timestamp_span = AZ_SPAN_LITERAL_FROM_STR("timestamp");
static az_span const triggered_span = AZ_SPAN_LITERAL_FROM_STR("triggeredBy");
static az_span const cloud_span = AZ_SPAN_LITERAL_FROM_STR("cloud");
static az_span const device_span = AZ_SPAN_LITERAL_FROM_STR("device");
static az_span const event_rst_span = AZ_SPAN_LITERAL_FROM_STR("rst.start");//0
static az_span const event_ras_span = AZ_SPAN_LITERAL_FROM_STR("ras.start");//1
static az_span const event_def_span = AZ_SPAN_LITERAL_FROM_STR("def.start");//2

void parse_c2d_message(
    char* topic,
    int topic_len,
    wiced_mqtt_topic_msg_t const* message,
    az_iot_hub_client_c2d_request* out_c2d_request)
{
   char string_temp[32];
   uint32_t result,events;
   uint32_t real_size;
   uint8_t type_found=0;
   uint8_t command_found=0;
  az_span const message_span = az_span_create((uint8_t*)message->data, message->data_len);

  az_json_reader jr;
  az_json_reader_init(&jr, message_span, NULL);
  az_json_reader_next_token(&jr);
  if (jr.token.kind == AZ_JSON_TOKEN_BEGIN_OBJECT)
  {
    IOT_SAMPLE_LOG("c2d valid");
  }
  else
      return;
  az_json_reader_next_token(&jr);
  while (type_found==0 && jr.token.kind != AZ_JSON_TOKEN_END_OBJECT)//find type
  {
    if (az_json_token_is_text_equal(&jr.token, type_name))
    {
      // Move to the value token.
        az_json_reader_next_token(&jr);
        az_json_token_get_string(&jr.token, string_temp,sizeof(string_temp),real_size);
        if(strcmp(string_temp,"command")==0)
        {
            type_found = 1;
        }
        else
            return;
    }
    else
    {
        az_json_reader_skip_children(&jr);
    }
    az_json_reader_next_token(&jr);
  }
  while (command_found==0 && jr.token.kind != AZ_JSON_TOKEN_END_OBJECT)//find command
  {
    if (az_json_token_is_text_equal(&jr.token, rst_name))
    {
        az_json_reader_next_token(&jr);
        c2d_reponse(0,1);
        command_found = 1;
        IOT_SAMPLE_LOG_SUCCESS("parse rst ok");
        wifi_uart_write_command_value(RST_SET_CMD,1);
        result = wiced_rtos_wait_for_event_flags(&C2D_EventHandler,EVENT_C2D_RST_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
        if(events & EVENT_C2D_RST_SET)
        {
            IOT_SAMPLE_LOG_SUCCESS("SET RST OK\r\n");
        }
    }
    else if (az_json_token_is_text_equal(&jr.token, def_name))
    {
        az_json_reader_next_token(&jr);
        command_found = 1;
        IOT_SAMPLE_LOG_SUCCESS("parse def ok");
        wifi_uart_write_command_value(DEF_SET_CMD,1);
        result = wiced_rtos_wait_for_event_flags(&C2D_EventHandler,EVENT_C2D_DEF_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
        if(events & EVENT_C2D_DEF_SET)
        {
            IOT_SAMPLE_LOG_SUCCESS("SET DEF OK\r\n");
            c2d_reponse(1,1);
            set_factory();
        }
    }
    else if (az_json_token_is_text_equal(&jr.token, ras_name))
    {
        az_json_reader_next_token(&jr);
        command_found = 1;
        IOT_SAMPLE_LOG_SUCCESS("parse ras ok");
        wifi_uart_write_command_value(RAS_SET_CMD,1);
        result = wiced_rtos_wait_for_event_flags(&C2D_EventHandler,EVENT_C2D_RAS_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
        if(events & EVENT_C2D_RAS_SET)
        {
            IOT_SAMPLE_LOG_SUCCESS("SET RAS OK\r\n");
            c2d_reponse(2,1);
        }
    }
    else
    {
        az_json_reader_skip_children(&jr);
    }
    az_json_reader_next_token(&jr);
  }
}
void c2d_reponse(uint8_t type,uint8_t source)
{
    char payload_buffer[1024] = {0};;
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
    az_json_writer_append_property_name(&jw, typ_span);
    az_json_writer_append_string(&jw,event_span);
    az_json_writer_append_property_name(&jw, timestamp_span);
    az_json_writer_append_int32(&jw,get_time());
    az_json_writer_append_property_name(&jw, event_span);
    switch(type)
    {
        case 0:az_json_writer_append_string(&jw,event_rst_span);break;
        case 1:az_json_writer_append_string(&jw,event_def_span);break;
        case 2:az_json_writer_append_string(&jw,event_ras_span);break;
    }
    az_json_writer_append_property_name(&jw, triggered_span);
    switch(source)
    {
        case 0:az_json_writer_append_string(&jw,device_span);break;
        case 1:az_json_writer_append_string(&jw,cloud_span);break;
    }
    az_json_writer_append_end_object(&jw);
    out_payload = az_json_writer_get_bytes_used_in_destination(&jw);
    wiced_mqtt_publish(mqtt_object,topic_buffer,az_span_ptr(out_payload),az_span_size(out_payload),0);
}
