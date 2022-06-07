#include "azure/az_core.h"
#include "azure/az_iot.h"
#include "iot_sample_common.h"
#include "wiced.h"
#include "mqtt_api.h"
#include "resources.h"
#include "twin_upload.h"
#include "uart_core.h"
#include "twin_parse.h"

uint8_t mac_value[18];
uint8_t srn_value[10];
uint8_t ver_value[10];
wiced_mac_t mac_info;

extern syr_status device_status;
extern az_iot_hub_client hub_client;
extern wiced_mqtt_object_t mqtt_object;

extern EventGroupHandle_t Config_EventHandler;
extern EventGroupHandle_t Info_EventHandler;

extern uint32_t telemetry_value;

static az_span const twin_patch_topic_request_id = AZ_SPAN_LITERAL_FROM_STR("reported_prop");
static az_span const deviceInfo_name = AZ_SPAN_LITERAL_FROM_STR("deviceInfo");
static az_span const deviceConfig_name = AZ_SPAN_LITERAL_FROM_STR("deviceConfig");
static az_span const ras_name = AZ_SPAN_LITERAL_FROM_STR("ras");
static az_span const mac_name = AZ_SPAN_LITERAL_FROM_STR("mac");
static az_span const srn_name = AZ_SPAN_LITERAL_FROM_STR("srn");
static az_span const sup_name = AZ_SPAN_LITERAL_FROM_STR("sup");
static az_span const ver_name = AZ_SPAN_LITERAL_FROM_STR("ver");
static az_span const wnk_name = AZ_SPAN_LITERAL_FROM_STR("wnk");
static az_span const wgw_name = AZ_SPAN_LITERAL_FROM_STR("wgw");
static az_span const wip_name = AZ_SPAN_LITERAL_FROM_STR("wip");
static az_span const com_name = AZ_SPAN_LITERAL_FROM_STR("com");
static az_span const coa_name = AZ_SPAN_LITERAL_FROM_STR("coa");
static az_span const cod_name = AZ_SPAN_LITERAL_FROM_STR("cod");
static az_span const coe_name = AZ_SPAN_LITERAL_FROM_STR("coe");
static az_span const cnd_name = AZ_SPAN_LITERAL_FROM_STR("cnd");
static az_span const rse_name = AZ_SPAN_LITERAL_FROM_STR("rse");
static az_span const rsa_name = AZ_SPAN_LITERAL_FROM_STR("rsa");
static az_span const rsi_name = AZ_SPAN_LITERAL_FROM_STR("rsi");
static az_span const rsd_name = AZ_SPAN_LITERAL_FROM_STR("rsd");
static az_span const cnf_name = AZ_SPAN_LITERAL_FROM_STR("cnf");
static az_span const cnl_name = AZ_SPAN_LITERAL_FROM_STR("cnl");
static az_span const sse_name = AZ_SPAN_LITERAL_FROM_STR("sse");
static az_span const ssa_name = AZ_SPAN_LITERAL_FROM_STR("ssa");
static az_span const ssd_name = AZ_SPAN_LITERAL_FROM_STR("ssd");
static az_span const lng_name = AZ_SPAN_LITERAL_FROM_STR("lng");
static az_span const tpr_name = AZ_SPAN_LITERAL_FROM_STR("tpr");

void info_single_upload(uint8_t command,uint32_t value)//主动上传
{
    char payload_buffer[2048];
    az_span payload = AZ_SPAN_FROM_BUFFER(payload_buffer);
    az_span out_payload;
    az_json_writer jw;

    char twin_patch_topic_buffer[128];
    az_iot_hub_client_twin_patch_get_publish_topic(
        &hub_client,
        twin_patch_topic_request_id,
        twin_patch_topic_buffer,
        sizeof(twin_patch_topic_buffer),
        NULL);

    az_json_writer_init(&jw, payload, NULL);
    az_json_writer_append_begin_object(&jw);

    az_json_writer_append_property_name(&jw, deviceInfo_name);
    az_json_writer_append_begin_object(&jw);
    switch(command)
    {
    case RAS_PUT_CMD:
        az_json_writer_append_property_name(&jw, ras_name);
        break;
    case SUP_PUT_CMD:
        az_json_writer_append_property_name(&jw, sup_name);
        break;
    case COM_PUT_CMD:
        az_json_writer_append_property_name(&jw, com_name);
        break;
    case COA_PUT_CMD:
        az_json_writer_append_property_name(&jw, coa_name);
        break;
    case COD_PUT_CMD:
        az_json_writer_append_property_name(&jw, cod_name);
        break;
    case COE_PUT_CMD:
        az_json_writer_append_property_name(&jw, coe_name);
        break;
    case CND_PUT_CMD:
        az_json_writer_append_property_name(&jw, cnd_name);
        break;
    default:
        return;
    }
    az_json_writer_append_int32(&jw, value);
    az_json_writer_append_end_object(&jw);

    az_json_writer_append_end_object(&jw);
    out_payload = az_json_writer_get_bytes_used_in_destination(&jw);
    wiced_mqtt_publish(mqtt_object,twin_patch_topic_buffer,az_span_ptr(out_payload),az_span_size(out_payload),0);
}
void config_single_upload(uint8_t command,uint32_t value)//主动上传
{
    char payload_buffer[2048];
    az_span payload = AZ_SPAN_FROM_BUFFER(payload_buffer);
    az_span out_payload;
    az_json_writer jw;

    char twin_patch_topic_buffer[128];
    az_iot_hub_client_twin_patch_get_publish_topic(
        &hub_client,
        twin_patch_topic_request_id,
        twin_patch_topic_buffer,
        sizeof(twin_patch_topic_buffer),
        NULL);

    az_json_writer_init(&jw, payload, NULL);
    az_json_writer_append_begin_object(&jw);

    az_json_writer_append_property_name(&jw, deviceConfig_name);
    az_json_writer_append_begin_object(&jw);
    switch(command)
    {
    case RSE_PUT_CMD:
        az_json_writer_append_property_name(&jw, rse_name);
        break;
    case RSA_PUT_CMD:
        az_json_writer_append_property_name(&jw, rsa_name);
        break;
    case RSI_PUT_CMD:
        az_json_writer_append_property_name(&jw, rsi_name);
        break;
    case RSD_PUT_CMD:
        az_json_writer_append_property_name(&jw, rsd_name);
        break;
    case CNF_PUT_CMD:
        az_json_writer_append_property_name(&jw, cnf_name);
        break;
    case CNL_PUT_CMD:
        az_json_writer_append_property_name(&jw, cnl_name);
        break;
    case SSE_PUT_CMD:
        az_json_writer_append_property_name(&jw, sse_name);
        break;
    case SSA_PUT_CMD:
        az_json_writer_append_property_name(&jw, ssa_name);
        break;
    case SSD_PUT_CMD:
        az_json_writer_append_property_name(&jw, ssd_name);
        break;
    case LNG_PUT_CMD:
        az_json_writer_append_property_name(&jw, lng_name);
        break;
    default:
        return;
    }
    az_json_writer_append_int32(&jw, value);
    az_json_writer_append_end_object(&jw);

    az_json_writer_append_end_object(&jw);

    out_payload = az_json_writer_get_bytes_used_in_destination(&jw);
    wiced_mqtt_publish(mqtt_object,twin_patch_topic_buffer,az_span_ptr(out_payload),az_span_size(out_payload),0);
    printf("config_single_upload,topic is %s\r\n",twin_patch_topic_buffer);
}

void config_get(void)
{
    EventBits_t EventValue;
    wifi_uart_write_command_value(RSE_GET_CMD,0);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_RSE_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_RSE_GET)
    {
        IOT_SAMPLE_LOG_SUCCESS("GET RSE %d\r\n",device_status.config.rse);
    }
    wifi_uart_write_command_value(RSA_GET_CMD,0);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_RSA_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_RSA_GET)
    {
        IOT_SAMPLE_LOG_SUCCESS("GET RSA %d\r\n",device_status.config.rsa);
    }
    wifi_uart_write_command_value(RSI_GET_CMD,0);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_RSI_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_RSI_GET)
    {
        IOT_SAMPLE_LOG_SUCCESS("GET RSI %d\r\n",device_status.config.rsi);
    }
    wifi_uart_write_command_value(RSD_GET_CMD,0);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_RSD_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_RSD_GET)
    {
        IOT_SAMPLE_LOG_SUCCESS("GET RSD %d\r\n",device_status.config.rsd);
    }
    wifi_uart_write_command_value(CNF_GET_CMD,0);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_CNF_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_CNF_GET)
    {
        IOT_SAMPLE_LOG_SUCCESS("GET CNF %d\r\n",device_status.config.cnf);
    }
    wifi_uart_write_command_value(CNL_GET_CMD,0);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_CNL_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_CNL_GET)
    {
        IOT_SAMPLE_LOG_SUCCESS("GET CNL %d\r\n",device_status.config.cnl);
    }

    wifi_uart_write_command_value(SSE_GET_CMD,0);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_SSE_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_SSE_GET)
    {
        IOT_SAMPLE_LOG_SUCCESS("GET SSE %d\r\n",device_status.config.sse);
    }
    wifi_uart_write_command_value(SSA_GET_CMD,0);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_SSA_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_SSA_GET)
    {
        IOT_SAMPLE_LOG_SUCCESS("GET SSA %d\r\n",device_status.config.ssa);
    }
    wifi_uart_write_command_value(SSD_GET_CMD,0);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_SSD_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_SSD_GET)
    {
        IOT_SAMPLE_LOG_SUCCESS("GET SSD %d\r\n",device_status.config.ssd);
    }
    wifi_uart_write_command_value(LNG_GET_CMD,0);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_LNG_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_LNG_GET)
    {
        IOT_SAMPLE_LOG_SUCCESS("GET LNG %d\r\n",device_status.config.lng);
    }
}
void info_get(void)
{
    EventBits_t EventValue;
    wifi_uart_write_command_value(COM_GET_CMD,0);
    EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_COM_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_INFO_COM_GET)
    {
        IOT_SAMPLE_LOG_SUCCESS("GET COM %d\r\n",device_status.info.com);
    }
    wifi_uart_write_command_value(COA_GET_CMD,0);
    EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_COA_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_INFO_COA_GET)
    {
        IOT_SAMPLE_LOG_SUCCESS("GET COA %d\r\n",device_status.info.coa);
    }
    wifi_uart_write_command_value(COD_GET_CMD,0);
    EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_COD_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_INFO_COD_GET)
    {
        IOT_SAMPLE_LOG_SUCCESS("GET COD %d\r\n",device_status.info.cod);
    }
    wifi_uart_write_command_value(COE_GET_CMD,0);
    EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_COE_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_INFO_COE_GET)
    {
        IOT_SAMPLE_LOG_SUCCESS("GET COE %d\r\n",device_status.info.coe);
    }
    wifi_uart_write_command_value(SUP_GET_CMD,0);
    EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_SUP_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_INFO_SUP_GET)
    {
        IOT_SAMPLE_LOG_SUCCESS("GET SUP %d\r\n",device_status.info.sup);
    }
    wifi_uart_write_command_value(VER_GET_CMD,0);
    EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_VER_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_INFO_VER_GET)
    {
        IOT_SAMPLE_LOG_SUCCESS("GET VER %d\r\n",device_status.info.ver);
    }
    wifi_uart_write_command_value(SRN_GET_CMD,0);
    EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_SRN_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_INFO_SRN_GET)
    {
        IOT_SAMPLE_LOG_SUCCESS("GET SRN %d\r\n",device_status.info.srn);
    }
}
void twin_upload(void)
{
    char payload_buffer[2048];
    memset(payload_buffer,0,2048);
    az_span payload = AZ_SPAN_FROM_BUFFER(payload_buffer);
    az_span out_payload;
    az_json_writer jw;

    char twin_patch_topic_buffer[128];
    az_iot_hub_client_twin_patch_get_publish_topic(
        &hub_client,
        twin_patch_topic_request_id,
        twin_patch_topic_buffer,
        sizeof(twin_patch_topic_buffer),
        NULL);

    az_json_writer_init(&jw, payload, NULL);
    az_json_writer_append_begin_object(&jw);

    az_json_writer_append_property_name(&jw, deviceConfig_name);
    az_json_writer_append_begin_object(&jw);
    az_json_writer_append_property_name(&jw, rse_name);
    az_json_writer_append_int32(&jw, device_status.config.rse);
    az_json_writer_append_property_name(&jw, rsa_name);
    az_json_writer_append_int32(&jw, device_status.config.rsa);
    az_json_writer_append_property_name(&jw, rsi_name);
    az_json_writer_append_int32(&jw, device_status.config.rsi);
    az_json_writer_append_property_name(&jw, rsd_name);
    az_json_writer_append_int32(&jw, device_status.config.rsd);
    az_json_writer_append_property_name(&jw, cnf_name);
    az_json_writer_append_int32(&jw, device_status.config.cnf);
    az_json_writer_append_property_name(&jw, cnl_name);
    az_json_writer_append_int32(&jw, device_status.config.cnl);
    az_json_writer_append_property_name(&jw, sse_name);
    az_json_writer_append_int32(&jw, device_status.config.sse);
    az_json_writer_append_property_name(&jw, ssa_name);
    az_json_writer_append_int32(&jw, device_status.config.ssa);
    az_json_writer_append_property_name(&jw, ssd_name);
    az_json_writer_append_int32(&jw, device_status.config.ssd);
    az_json_writer_append_property_name(&jw, lng_name);
    az_json_writer_append_int32(&jw, device_status.config.lng);
    az_json_writer_append_property_name(&jw, tpr_name);
    az_json_writer_append_int32(&jw, telemetry_value);
    az_json_writer_append_end_object(&jw);

    az_json_writer_append_property_name(&jw, deviceInfo_name);
    az_json_writer_append_begin_object(&jw);
    az_json_writer_append_property_name(&jw, sup_name);
    az_json_writer_append_int32(&jw, device_status.info.sup);
    az_json_writer_append_property_name(&jw, com_name);
    az_json_writer_append_int32(&jw, device_status.info.com);
    az_json_writer_append_property_name(&jw, coa_name);
    az_json_writer_append_int32(&jw, device_status.info.coa);
    az_json_writer_append_property_name(&jw, cod_name);
    az_json_writer_append_int32(&jw, device_status.info.cod);
    az_json_writer_append_property_name(&jw, coe_name);
    az_json_writer_append_int32(&jw, device_status.info.coe);
    az_json_writer_append_property_name(&jw, srn_name);
    sprintf(srn_value,"%09d",device_status.info.srn);
    az_json_writer_append_string(&jw, az_span_create_from_str(srn_value));
    az_json_writer_append_property_name(&jw, ver_name);
    sprintf(ver_value,"%01d.%01d.%01d",device_status.info.ver/100%10,device_status.info.ver/10%10,device_status.info.ver%10);
    az_json_writer_append_string(&jw, az_span_create_from_str(ver_value));
    az_json_writer_append_property_name(&jw, mac_name);
    if ( wwd_wifi_get_mac_address( &mac_info, WWD_STA_INTERFACE ) == WWD_SUCCESS )
    {
        sprintf(mac_value,"%02X:%02X:%02X:%02X:%02X:%02X",mac_info.octet[0],mac_info.octet[1],mac_info.octet[2],mac_info.octet[3],mac_info.octet[4],mac_info.octet[5]);
    }
    az_json_writer_append_string(&jw,az_span_create_from_str(mac_value));
    az_json_writer_append_property_name(&jw, wip_name);
    az_json_writer_append_string(&jw, az_span_create_from_str(ip4addr_ntoa(netif_ip4_addr(&IP_HANDLE(WICED_STA_INTERFACE)))));
    az_json_writer_append_property_name(&jw, wnk_name);
    az_json_writer_append_string(&jw, az_span_create_from_str(ip4addr_ntoa(netif_ip4_netmask(&IP_HANDLE(WICED_STA_INTERFACE)))));
    az_json_writer_append_property_name(&jw, wgw_name);
    az_json_writer_append_string(&jw, az_span_create_from_str(ip4addr_ntoa(netif_ip4_gw(&IP_HANDLE(WICED_STA_INTERFACE)))));
    az_json_writer_append_end_object(&jw);

    az_json_writer_append_end_object(&jw);
    out_payload = az_json_writer_get_bytes_used_in_destination(&jw);
    wiced_mqtt_publish(mqtt_object,twin_patch_topic_buffer,az_span_ptr(out_payload),az_span_size(out_payload),0);
    printf("info_upload,size is %d,payload is %s\r\n",az_span_size(out_payload),az_span_ptr(out_payload));
}
