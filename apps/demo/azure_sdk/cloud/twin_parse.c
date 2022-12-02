#include "azure/az_core.h"
#include "azure/az_iot.h"
#include "iot_sample_common.h"
#include "wiced.h"
#include "mqtt_api.h"
#include "resources.h"
#include "uart_core.h"
#include "twin_parse.h"

syr_status device_status;

extern az_iot_hub_client hub_client;
extern wiced_mqtt_object_t mqtt_object;
extern wiced_event_flags_t Config_EventHandler;
extern wiced_event_flags_t Info_EventHandler;
extern wiced_event_flags_t C2D_EventHandler;

static az_span const twin_document_topic_request_id = AZ_SPAN_LITERAL_FROM_STR("get_twin");
static az_span const rse_name = AZ_SPAN_LITERAL_FROM_STR("rse");
static az_span const rsa_name = AZ_SPAN_LITERAL_FROM_STR("rsa");
static az_span const rsi_name = AZ_SPAN_LITERAL_FROM_STR("rsi");
static az_span const rsd_name = AZ_SPAN_LITERAL_FROM_STR("rsd");
static az_span const cnf_name = AZ_SPAN_LITERAL_FROM_STR("cnf");
static az_span const cnl_name = AZ_SPAN_LITERAL_FROM_STR("cnl");
static az_span const lng_name = AZ_SPAN_LITERAL_FROM_STR("lng");
static az_span const sse_name = AZ_SPAN_LITERAL_FROM_STR("sse");
static az_span const ssa_name = AZ_SPAN_LITERAL_FROM_STR("ssa");
static az_span const ssd_name = AZ_SPAN_LITERAL_FROM_STR("ssd");
static az_span const emr_name = AZ_SPAN_LITERAL_FROM_STR("emr");
static az_span const rcp_name = AZ_SPAN_LITERAL_FROM_STR("rcp");

static az_span const com_name = AZ_SPAN_LITERAL_FROM_STR("com");
static az_span const coa_name = AZ_SPAN_LITERAL_FROM_STR("coa");
static az_span const cod_name = AZ_SPAN_LITERAL_FROM_STR("cod");
static az_span const coe_name = AZ_SPAN_LITERAL_FROM_STR("coe");
static az_span const config_span = AZ_SPAN_LITERAL_FROM_STR("deviceConfig");
static az_span const info_span = AZ_SPAN_LITERAL_FROM_STR("deviceInfo");
static az_span const version_span = AZ_SPAN_LITERAL_FROM_STR("$version");


int parse_device_twin_message(
    char* topic,
    int topic_len,
    wiced_mqtt_topic_msg_t const* message,
    az_iot_hub_client_twin_response* out_twin_response)
{
  az_span const topic_span = az_span_create((uint8_t*)topic, topic_len);
  az_span const message_span = az_span_create((uint8_t*)message->data, message->data_len);

  // Parse message and retrieve twin_response info.
  az_result rc
      = az_iot_hub_client_twin_parse_received_topic(&hub_client, topic_span, out_twin_response);
  if (az_result_failed(rc))
  {
    IOT_SAMPLE_LOG_ERROR("Message from unknown topic: az_result return code 0x%08x.", rc);
    IOT_SAMPLE_LOG_AZ_SPAN("Topic:", topic_span);
    return 0;
  }
  IOT_SAMPLE_LOG_AZ_SPAN("Topic:", topic_span);
  IOT_SAMPLE_LOG_AZ_SPAN("Payload:", message_span);
  IOT_SAMPLE_LOG("Status: %d", out_twin_response->status);
  return 1;
}
void get_device_twin_document(void)
{
  int rc;

  IOT_SAMPLE_LOG("Client requesting device twin document from service.");

  // Get the Twin Document topic to publish the twin document request.
  char twin_document_topic_buffer[128];
  rc = az_iot_hub_client_twin_document_get_publish_topic(
      &hub_client,
      twin_document_topic_request_id,
      twin_document_topic_buffer,
      sizeof(twin_document_topic_buffer),
      NULL);
  if (az_result_failed(rc))
  {
    IOT_SAMPLE_LOG_ERROR(
        "Failed to get the Twin Document topic: az_result return code 0x%08x.", rc);
    exit(rc);
  }

  wiced_mqtt_publish(mqtt_object,twin_document_topic_buffer,NULL,0,0);
  IOT_SAMPLE_LOG("get_device_twin_document Send\r");
}
void parse_get_twin(az_span const message_span)
{
    uint32_t ret,events;
    uint32_t value;

    az_json_reader jr;
    az_json_reader_init(&jr, message_span, NULL);
    az_json_reader_next_token(&jr);
    if (jr.token.kind == AZ_JSON_TOKEN_BEGIN_OBJECT)
    {
        IOT_SAMPLE_LOG("parse_twin_message valid");
    }
    else
    {
        IOT_SAMPLE_LOG("parse_twin_message failed");
        return;
    }
    do
    {
        az_json_reader_next_token(&jr);
    }
    while(az_json_token_is_text_equal(&jr.token, config_span) == 0 && jr.token.kind != AZ_JSON_TOKEN_END_OBJECT);
    while (jr.token.kind != AZ_JSON_TOKEN_END_OBJECT)//find command
    {
        if (az_json_token_is_text_equal(&jr.token, rse_name))
        {
            az_json_reader_next_token(&jr);
            az_json_token_get_uint32(&jr,&value);
            IOT_SAMPLE_LOG_SUCCESS("parse rse ok,value is %d",value);
            if(device_status.config.rse != value)
            {
                wifi_uart_write_command_value(RSE_SET_CMD,value);
                ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_RSE_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
                if(events & EVENT_CONFIG_RSE_SET)
                {
                    IOT_SAMPLE_LOG_SUCCESS("SET RSE OK\r\n");
                }
            }
        }
        else if (az_json_token_is_text_equal(&jr.token, rsa_name))
        {
            az_json_reader_next_token(&jr);
            az_json_token_get_uint32(&jr,&value);
            IOT_SAMPLE_LOG_SUCCESS("parse rsa ok,value is %d",value);
            if(device_status.config.rsa != value)
            {
                wifi_uart_write_command_value(RSA_SET_CMD,value);
                ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_RSA_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
                if(events & EVENT_CONFIG_RSA_SET)
                {
                    IOT_SAMPLE_LOG_SUCCESS("SET RSA OK\r\n");
                }
            }
        }
        else if (az_json_token_is_text_equal(&jr.token, rsi_name))
        {
            az_json_reader_next_token(&jr);
            az_json_token_get_uint32(&jr,&value);
            IOT_SAMPLE_LOG_SUCCESS("parse rsi ok,value is %d",value);
            if(device_status.config.rsi != value)
            {
                wifi_uart_write_command_value(RSI_SET_CMD,value);
                ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_RSI_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
                if(events & EVENT_CONFIG_RSI_SET)
                {
                    IOT_SAMPLE_LOG_SUCCESS("SET RSI OK\r\n");
                }
            }
        }
        else if (az_json_token_is_text_equal(&jr.token, rsd_name))
        {
            az_json_reader_next_token(&jr);
            az_json_token_get_uint32(&jr,&value);
            IOT_SAMPLE_LOG_SUCCESS("parse rsd ok,value is %d",value);
            if(device_status.config.rsd != value)
            {
                wifi_uart_write_command_value(RSD_SET_CMD,value);
                ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_RSD_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
                if(events & EVENT_CONFIG_RSD_SET)
                {
                    IOT_SAMPLE_LOG_SUCCESS("SET RSD OK\r\n");
                }
            }
        }
        else if (az_json_token_is_text_equal(&jr.token, cnf_name))
        {
            az_json_reader_next_token(&jr);
            az_json_token_get_uint32(&jr,&value);
            IOT_SAMPLE_LOG_SUCCESS("parse cnf ok,value is %d",value);
            if(device_status.config.cnf != value)
            {
                wifi_uart_write_command_value(CNF_SET_CMD,value);
                ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_CNF_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
                if(events & EVENT_CONFIG_CNF_SET)
                {
                    IOT_SAMPLE_LOG_SUCCESS("SET CNF OK\r\n");
                }
            }
        }
        else if (az_json_token_is_text_equal(&jr.token, cnl_name))
        {
            az_json_reader_next_token(&jr);
            az_json_token_get_uint32(&jr,&value);
            IOT_SAMPLE_LOG_SUCCESS("parse cnl ok,value is %d",value);
            if(device_status.config.cnl != value)
            {
                wifi_uart_write_command_value(CNL_SET_CMD,value);
                ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_CNL_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
                if(events & EVENT_CONFIG_CNL_SET)
                {
                    IOT_SAMPLE_LOG_SUCCESS("SET CNL OK\r\n");
                }
            }
        }
        else if (az_json_token_is_text_equal(&jr.token, lng_name))
        {
            az_json_reader_next_token(&jr);
            az_json_token_get_uint32(&jr,&value);
            IOT_SAMPLE_LOG_SUCCESS("parse lng ok,value is %d",value);
            if(device_status.config.lng != value)
            {
                wifi_uart_write_command_value(LNG_SET_CMD,value);
                ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_LNG_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
                if(events & EVENT_CONFIG_LNG_SET)
                {
                    IOT_SAMPLE_LOG_SUCCESS("SET LNG OK\r\n");
                }
            }
        }
        else if (az_json_token_is_text_equal(&jr.token, sse_name))
        {
            az_json_reader_next_token(&jr);
            az_json_token_get_uint32(&jr,&value);
            IOT_SAMPLE_LOG_SUCCESS("parse sse ok,value is %d",value);
            if(device_status.config.sse != value)
            {
                wifi_uart_write_command_value(SSE_SET_CMD,value);
                ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_SSE_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
                if(events & EVENT_CONFIG_SSE_SET)
                {
                    IOT_SAMPLE_LOG_SUCCESS("SET SSE OK\r\n");
                }
            }
        }
        else if (az_json_token_is_text_equal(&jr.token, ssa_name))
        {
            az_json_reader_next_token(&jr);
            az_json_token_get_uint32(&jr,&value);
            IOT_SAMPLE_LOG_SUCCESS("parse ssa ok,value is %d",value);
            if(device_status.config.ssa != value)
            {
                wifi_uart_write_command_value(SSA_SET_CMD,value);
                ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_SSA_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
                if(events & EVENT_CONFIG_SSA_SET)
                {
                    IOT_SAMPLE_LOG_SUCCESS("SET SSA OK\r\n");
                }
            }
        }
        else if (az_json_token_is_text_equal(&jr.token, ssd_name))
        {
            az_json_reader_next_token(&jr);
            az_json_token_get_uint32(&jr,&value);
            IOT_SAMPLE_LOG_SUCCESS("parse ssd ok,value is %d",value);
            if(device_status.config.ssd != value)
            {
                wifi_uart_write_command_value(SSD_SET_CMD,value);
                ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_SSD_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
                if(events & EVENT_CONFIG_SSD_SET)
                {
                    IOT_SAMPLE_LOG_SUCCESS("SET SSD OK\r\n");
                }
            }
        }
        else if (az_json_token_is_text_equal(&jr.token, emr_name))
        {
            az_json_reader_next_token(&jr);
            az_json_token_get_uint32(&jr,&value);
            IOT_SAMPLE_LOG_SUCCESS("parse emr ok,value is %d",value);
            if(device_status.config.emr != value)
            {
                wifi_uart_write_command_value(EMR_SET_CMD,value);
                ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_EMR_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
                if(events & EVENT_CONFIG_EMR_SET)
                {
                    IOT_SAMPLE_LOG_SUCCESS("SET EMR OK\r\n");
                }
            }
        }
        else if (az_json_token_is_text_equal(&jr.token, rcp_name))
        {
            az_json_reader_next_token(&jr);
            az_json_token_get_uint32(&jr,&value);
            IOT_SAMPLE_LOG_SUCCESS("parse rcp ok,value is %d",value);
            if(device_status.config.rcp != value)
            {
                wifi_uart_write_command_value(RCP_SET_CMD,value);
                ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_RCP_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
                if(events & EVENT_CONFIG_RCP_SET)
                {
                    IOT_SAMPLE_LOG_SUCCESS("SET RCP OK\r\n");
                }
            }
        }
        az_json_reader_next_token(&jr);
    }
    az_json_reader_next_token(&jr);
    if(az_json_token_is_text_equal(&jr.token, info_span)==0)
    {
        twin_upload();
        return;
    }
    az_json_reader_next_token(&jr);
    while (jr.token.kind != AZ_JSON_TOKEN_END_OBJECT)//find command
    {
        if (az_json_token_is_text_equal(&jr.token, com_name))
        {
            az_json_reader_next_token(&jr);
            az_json_token_get_uint32(&jr,&value);
            IOT_SAMPLE_LOG_SUCCESS("parse com ok,value is %d",value);
            if(device_status.info.com != value)
            {
                wifi_uart_write_command_value(COM_SET_CMD,value);
                ret = wiced_rtos_wait_for_event_flags(&Info_EventHandler,EVENT_INFO_COM_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
                if(events & EVENT_INFO_COM_GET)
                {
                    IOT_SAMPLE_LOG_SUCCESS("SET COM OK\r\n");
                }
            }
        }
        else if (az_json_token_is_text_equal(&jr.token, coa_name))
        {
            az_json_reader_next_token(&jr);
            az_json_token_get_uint32(&jr,&value);
            IOT_SAMPLE_LOG_SUCCESS("parse coa ok,value is %d",value);
            if(device_status.info.coa != value)
            {
                wifi_uart_write_command_value(COA_SET_CMD,value);
                ret = wiced_rtos_wait_for_event_flags(&Info_EventHandler,EVENT_INFO_COA_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
                if(events & EVENT_INFO_COA_GET)
                {
                    IOT_SAMPLE_LOG_SUCCESS("SET COA OK\r\n");
                }
            }
        }
        else if (az_json_token_is_text_equal(&jr.token, cod_name))
        {
            az_json_reader_next_token(&jr);
            az_json_token_get_uint32(&jr,&value);
            IOT_SAMPLE_LOG_SUCCESS("parse cod ok,value is %d",value);
            if(device_status.info.cod != value)
            {
                wifi_uart_write_command_value(COD_SET_CMD,value);
                ret = wiced_rtos_wait_for_event_flags(&Info_EventHandler,EVENT_INFO_COD_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
                if(events & EVENT_INFO_COD_GET)
                {
                    IOT_SAMPLE_LOG_SUCCESS("SET COD OK\r\n");
                }
            }
        }
        else if (az_json_token_is_text_equal(&jr.token, coe_name))
        {
            az_json_reader_next_token(&jr);
            az_json_token_get_uint32(&jr,&value);
            IOT_SAMPLE_LOG_SUCCESS("parse coe ok,value is %d",value);
            if(device_status.info.coe != value)
            {
                wifi_uart_write_command_value(COE_SET_CMD,value);
                ret = wiced_rtos_wait_for_event_flags(&Info_EventHandler,EVENT_INFO_COE_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
                if(events & EVENT_INFO_COE_GET)
                {
                    IOT_SAMPLE_LOG_SUCCESS("SET COE OK\r\n");
                }
            }
        }
        az_json_reader_next_token(&jr);
    }
    twin_upload();
}
void handle_device_twin_message(
        wiced_mqtt_topic_msg_t const* message,
    az_iot_hub_client_twin_response const* twin_response)
{
  az_span const message_span = az_span_create((uint8_t*)message->data, message->data_len);

  switch (twin_response->response_type)
  {
    case AZ_IOT_HUB_CLIENT_TWIN_RESPONSE_TYPE_GET:
      IOT_SAMPLE_LOG("Message Type: GET");
      parse_get_twin(message_span);
      break;

    case AZ_IOT_HUB_CLIENT_TWIN_RESPONSE_TYPE_REPORTED_PROPERTIES:
      IOT_SAMPLE_LOG("Message Type: Reported Properties");
      break;

    case AZ_IOT_HUB_CLIENT_TWIN_RESPONSE_TYPE_DESIRED_PROPERTIES:
      IOT_SAMPLE_LOG("Message Type: Desired Properties");
      parse_get_twin(message_span);
      break;

    case AZ_IOT_HUB_CLIENT_TWIN_RESPONSE_TYPE_REQUEST_ERROR:
      IOT_SAMPLE_LOG_ERROR("Message Type: Request Error");
      break;
  }
}
