#include "azure/az_core.h"
#include "azure/az_iot.h"
#include "iot_sample_common.h"
#include "wiced.h"
#include "mqtt_api.h"
#include "resources.h"
#include "twin_upload.h"
#include "uart_core.h"
#include "twin_parse.h"

wiced_mac_t mac_info;
static uint8_t mac_buf[18];
static uint8_t ipv4_buf[18];
static uint8_t mask_buf[18];
static uint8_t gw_ip_buf[18];
static uint8_t ssid_name_buf[32];

extern syr_status device_status;
extern az_iot_hub_client hub_client;
extern wiced_mqtt_object_t mqtt_object;
extern char* device_id;

extern wiced_event_flags_t Config_EventHandler;
extern wiced_event_flags_t Info_EventHandler;

#define LOG_D printf

static az_span const twin_patch_topic_request_id = AZ_SPAN_LITERAL_FROM_STR("reported_prop");
static az_span const deviceInfo_name = AZ_SPAN_LITERAL_FROM_STR("deviceInfo");
static az_span const deviceConfig_name = AZ_SPAN_LITERAL_FROM_STR("deviceConfig");
static az_span const ras_name = AZ_SPAN_LITERAL_FROM_STR("ras");
static az_span const mac_name = AZ_SPAN_LITERAL_FROM_STR("mac1");
static az_span const mcu_srn_name = AZ_SPAN_LITERAL_FROM_STR("srn");
static az_span const wifi_srn_name = AZ_SPAN_LITERAL_FROM_STR("srn");
static az_span const sup_name = AZ_SPAN_LITERAL_FROM_STR("sup");
static az_span const mcu_ver_name = AZ_SPAN_LITERAL_FROM_STR("ver");
static az_span const wifi_ver_name = AZ_SPAN_LITERAL_FROM_STR("ver2");
static az_span const alm_name = AZ_SPAN_LITERAL_FROM_STR("alm");
static az_span const alw_name = AZ_SPAN_LITERAL_FROM_STR("alw");
static az_span const aln_name = AZ_SPAN_LITERAL_FROM_STR("aln");
static az_span const wnk_name = AZ_SPAN_LITERAL_FROM_STR("wnk");
static az_span const wgw_name = AZ_SPAN_LITERAL_FROM_STR("wgw");
static az_span const wip_name = AZ_SPAN_LITERAL_FROM_STR("wip");
static az_span const wfc_name = AZ_SPAN_LITERAL_FROM_STR("wfc");
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
static az_span const rcp_name = AZ_SPAN_LITERAL_FROM_STR("rcp");
static az_span const emr_name = AZ_SPAN_LITERAL_FROM_STR("emr");
static az_span const wti_name = AZ_SPAN_LITERAL_FROM_STR("wti");
static az_span const apt_name = AZ_SPAN_LITERAL_FROM_STR("apt");
static az_span const rtc_name = AZ_SPAN_LITERAL_FROM_STR("rtc");
static az_span const hwv_name = AZ_SPAN_LITERAL_FROM_STR("hwv");
static az_span const wad_name = AZ_SPAN_LITERAL_FROM_STR("wad");

void config_get(void)
{
    uint32_t result,events;
    wifi_uart_write_command_value(RSE_GET_CMD,0);
    result = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_RSE_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_RSE_GET)
    {
        LOG_D("GET RSE %d\r\n",device_status.config.rse);
    }
    wifi_uart_write_command_value(RSA_GET_CMD,0);
    result = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_RSA_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_RSA_GET)
    {
        LOG_D("GET RSA %d\r\n",device_status.config.rsa);
    }
    wifi_uart_write_command_value(RSI_GET_CMD,0);
    result = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_RSI_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_RSI_GET)
    {
        LOG_D("GET RSI %d\r\n",device_status.config.rsi);
    }
    wifi_uart_write_command_value(RSD_GET_CMD,0);
    result = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_RSD_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_RSD_GET)
    {
        LOG_D("GET RSD %d\r\n",device_status.config.rsd);
    }
    wifi_uart_write_command_value(CNF_GET_CMD,0);
    result = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_CNF_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_CNF_GET)
    {
        LOG_D("GET CNF %d\r\n",device_status.config.cnf);
    }
    wifi_uart_write_command_value(CNL_GET_CMD,0);
    result = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_CNL_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_CNL_GET)
    {
        LOG_D("GET CNL %d\r\n",device_status.config.cnl);
    }

    wifi_uart_write_command_value(SSE_GET_CMD,0);
    result = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_SSE_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_SSE_GET)
    {
        LOG_D("GET SSE %d\r\n",device_status.config.sse);
    }
    wifi_uart_write_command_value(SSA_GET_CMD,0);
    result = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_SSA_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_SSA_GET)
    {
        LOG_D("GET SSA %d\r\n",device_status.config.ssa);
    }
    wifi_uart_write_command_value(SSD_GET_CMD,0);
    result = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_SSD_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_SSD_GET)
    {
        LOG_D("GET SSD %d\r\n",device_status.config.ssd);
    }
    wifi_uart_write_command_value(LNG_GET_CMD,0);
    result = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_LNG_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_LNG_GET)
    {
        LOG_D("GET LNG %d\r\n",device_status.config.lng);
    }
    wifi_uart_write_command_value(RCP_GET_CMD,0);
    result = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_RCP_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_RCP_GET)
    {
        LOG_D("GET RCP %d\r\n",device_status.config.rcp);
    }
    wifi_uart_write_command_value(WTI_GET_CMD,0);
    result = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_WTI_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_WTI_GET)
    {
        LOG_D("GET WTI %d\r\n",device_status.config.wti);
    }
    wifi_uart_write_command_value(EMR_GET_CMD,0);
    result = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_EMR_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_EMR_GET)
    {
        LOG_D("GET EMR %d\r\n",device_status.config.emr);
    }
    wifi_uart_write_command_value(APT_GET_CMD,0);
    result = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_APT_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_APT_GET)
    {
        LOG_D("GET APT %d\r\n",device_status.config.apt);
    }
    wifi_uart_write_command_value(WAD_GET_CMD,0);
    result = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_WAD_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_WAD_GET)
    {
        LOG_D("GET WAD %d\r\n",device_status.config.wad);
    }
}
void info_get(void)
{
    uint32_t result,events;
    wifi_uart_write_command_value(SUP_GET_CMD,0);
    result = wiced_rtos_wait_for_event_flags(&Info_EventHandler,EVENT_INFO_SUP_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_INFO_SUP_GET)
    {
        LOG_D("GET SUP %d\r\n",device_status.info.sup);
    }
    wifi_uart_write_command_value(ALM_GET_CMD,0);
    result = wiced_rtos_wait_for_event_flags(&Info_EventHandler,EVENT_INFO_ALM_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_INFO_ALM_GET)
    {
        LOG_D("GET ALM %s\r\n",device_status.info.alm_array);
    }
    wifi_uart_write_command_value(ALW_GET_CMD,0);
    result = wiced_rtos_wait_for_event_flags(&Info_EventHandler,EVENT_INFO_ALW_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_INFO_ALW_GET)
    {
        LOG_D("GET ALW %s\r\n",device_status.info.alw_array);
    }
    wifi_uart_write_command_value(ALN_GET_CMD,0);
    result = wiced_rtos_wait_for_event_flags(&Info_EventHandler,EVENT_INFO_ALN_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_INFO_ALN_GET)
    {
        LOG_D("GET ALN %s\r\n",device_status.info.aln_array);
    }
}
void twin_single_upload(char* json,uint32_t length)
{
    char payload_buffer[512] = {0};
    memset(payload_buffer,0,512);
    az_span payload = AZ_SPAN_FROM_BUFFER(payload_buffer);
    az_span out_payload;
    az_json_writer jw;
    ULONG ip_address, network_mask,gateway_ip;

    char twin_patch_topic_buffer[128];
    az_iot_hub_client_twin_patch_get_publish_topic(
        &hub_client,
        twin_patch_topic_request_id,
        twin_patch_topic_buffer,
        sizeof(twin_patch_topic_buffer),
        NULL);

    wiced_mqtt_publish(mqtt_object,twin_patch_topic_buffer,json,length,0);
    LOG_D("twin_single_upload,size is %d,payload is %s\r\n",length,json);
}
void twin_upload(void)
{
    extern char wifi_version[];
    char payload_buffer[2048] = {0};
    memset(payload_buffer,0,2048);
    az_span payload = AZ_SPAN_FROM_BUFFER(payload_buffer);
    az_span out_payload;
    az_json_writer jw;
    ULONG ip_address, network_mask,gateway_ip;

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
    az_json_writer_append_property_name(&jw, rcp_name);
    az_json_writer_append_int32(&jw, device_status.config.rcp);
    az_json_writer_append_property_name(&jw, emr_name);
    az_json_writer_append_int32(&jw, device_status.config.emr);
    az_json_writer_append_property_name(&jw, wti_name);
    az_json_writer_append_int32(&jw, device_status.config.wti);
    az_json_writer_append_property_name(&jw, apt_name);
    az_json_writer_append_int32(&jw, device_status.config.apt);
    az_json_writer_append_property_name(&jw, rtc_name);
    az_json_writer_append_int32(&jw,get_time());
    az_json_writer_append_property_name(&jw, wad_name);
    az_json_writer_append_int32(&jw,device_status.config.wad);
    az_json_writer_append_end_object(&jw);

    az_json_writer_append_property_name(&jw, deviceInfo_name);
    az_json_writer_append_begin_object(&jw);
    az_json_writer_append_property_name(&jw, sup_name);
    az_json_writer_append_int32(&jw, device_status.info.sup);
    az_json_writer_append_property_name(&jw, wifi_srn_name);
    az_json_writer_append_string(&jw, az_span_create_from_str(device_id));
    az_json_writer_append_property_name(&jw, mcu_ver_name);
    az_json_writer_append_string(&jw, az_span_create_from_str(device_status.info.ver));
    az_json_writer_append_property_name(&jw, wifi_ver_name);
    az_json_writer_append_string(&jw, az_span_create_from_str(wifi_version));
    az_json_writer_append_property_name(&jw, mac_name);
    if ( wwd_wifi_get_mac_address( &mac_info, WWD_STA_INTERFACE ) == WWD_SUCCESS )
    {
        sprintf(mac_buf,"%02X:%02X:%02X:%02X:%02X:%02X",mac_info.octet[0],mac_info.octet[1],mac_info.octet[2],mac_info.octet[3],mac_info.octet[4],mac_info.octet[5]);
    }
    az_json_writer_append_string(&jw,az_span_create_from_str(mac_buf));
    nx_ip_address_get( &IP_HANDLE(WICED_STA_INTERFACE), &ip_address, &network_mask );
    nx_ip_gateway_address_get(&IP_HANDLE(WICED_STA_INTERFACE),&gateway_ip);
    sprintf(ipv4_buf,"%d.%d.%d.%d",(unsigned char)( ( ip_address >> 24 ) & 0xff ),(unsigned char)( ( ip_address >> 16 ) & 0xff ),(unsigned char)( ( ip_address >> 8 ) & 0xff ), (unsigned char)( ( ip_address >> 0 ) & 0xff ));
    sprintf(mask_buf,"%d.%d.%d.%d",(unsigned char) ( ( network_mask >> 24 ) & 0xff ), (unsigned char) ( ( network_mask >> 16 ) & 0xff ), (unsigned char) ( ( network_mask >> 8 ) & 0xff ), (unsigned char) ( ( network_mask >> 0 ) & 0xff ));
    sprintf(gw_ip_buf,"%d.%d.%d.%d",(unsigned char) ( ( gateway_ip >> 24 ) & 0xff ), (unsigned char) ( ( gateway_ip >> 16 ) & 0xff ), (unsigned char) ( ( gateway_ip >> 8 ) & 0xff ), (unsigned char) ( ( gateway_ip >> 0 ) & 0xff ) );
    az_json_writer_append_property_name(&jw, wip_name);
    az_json_writer_append_string(&jw, az_span_create_from_str(ipv4_buf));
    az_json_writer_append_property_name(&jw, wnk_name);
    az_json_writer_append_string(&jw, az_span_create_from_str(mask_buf));
    az_json_writer_append_property_name(&jw, wgw_name);
    az_json_writer_append_string(&jw, az_span_create_from_str(gw_ip_buf));
    az_json_writer_append_property_name(&jw, alm_name);
    az_json_writer_append_string(&jw, az_span_create_from_str(device_status.info.alm_array));
    az_json_writer_append_property_name(&jw, alw_name);
    az_json_writer_append_string(&jw, az_span_create_from_str(device_status.info.alw_array));
    az_json_writer_append_property_name(&jw, aln_name);
    az_json_writer_append_string(&jw, az_span_create_from_str(device_status.info.aln_array));
    az_json_writer_append_property_name(&jw, hwv_name);
    az_json_writer_append_string(&jw, az_span_create_from_str("35"));
    dct_read_first_ap_ssid_name(ssid_name_buf);
    az_json_writer_append_property_name(&jw, wfc_name);
    az_json_writer_append_string(&jw, az_span_create_from_str(ssid_name_buf));
    az_json_writer_append_end_object(&jw);

    az_json_writer_append_end_object(&jw);
    out_payload = az_json_writer_get_bytes_used_in_destination(&jw);
    wiced_mqtt_publish(mqtt_object,twin_patch_topic_buffer,az_span_ptr(out_payload),az_span_size(out_payload),0);
    LOG_D("twin_upload,size is %d,payload is %s\r\n",az_span_size(out_payload),az_span_ptr(out_payload));

}
