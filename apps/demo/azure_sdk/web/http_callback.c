#include "wiced.h"
#include "http_server.h"
#include "http_api.h"
#include "resources.h"
#include "sntp.h"
#include "cJSON.h"
#include "http_callback.h"
#include "uart_core.h"
#include "twin_parse.h"
#include "uart_core.h"

wiced_ssid_t save_ssid;
uint8_t save_security_id;
wiced_wep_key_t save_password;
wiced_http_response_stream_t *now_stream;

extern syr_status device_status;
extern EventGroupHandle_t Config_EventHandler;
extern EventGroupHandle_t Info_EventHandler;
extern EventGroupHandle_t TEM_EventHandler;
extern EventGroupHandle_t C2D_EventHandler;

int32_t wifi_callback( const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body )
{
    now_stream = stream;
    return 0;
}
int32_t http_rst_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    EventBits_t EventValue;
    now_stream = stream;

    char* value_buf = flite_value(url_path,&size);
    value = atoi(value_buf);
    printf("http_rst_callback value %d,size %d\r\n",value,size);
    wifi_uart_write_command_value(RST_SET_CMD,1);
    EventValue = xEventGroupWaitBits(C2D_EventHandler,EVENT_C2D_RST_SET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_C2D_RST_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setrst",6,value_buf,size);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setrst",6,value_buf,size);
    }
    free(value_buf);
    return 0;
}
int32_t http_def_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    EventBits_t EventValue;
    now_stream = stream;

    char* value_buf = flite_value(url_path,&size);
    value = atoi(value_buf);
    printf("http_def_set_callback value %d,size %d\r\n",value,size);
    wifi_uart_write_command_value(DEF_SET_CMD,1);
    EventValue = xEventGroupWaitBits(C2D_EventHandler,EVENT_C2D_DEF_SET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_C2D_DEF_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setdef",6,value_buf,size);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setdef",6,value_buf,size);
    }
}
int32_t http_ras_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, waiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    EventBits_t EventValue;
    now_stream = stream;

    char* value_buf = flite_value(url_path,&size);
    value = atoi(value_buf);
    printf("http_def_set_callback value %d,size %d\r\n",value,size);
    wifi_uart_write_command_value(RAS_SET_CMD,1);
    EventValue = xEventGroupWaitBits(C2D_EventHandler,EVENT_C2D_RAS_SET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_C2D_RAS_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setras",6,value_buf,size);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setras",6,value_buf,size);
    }
}
int32_t http_cnd_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    printf("http_cnd_callback GET\r\n");
    wifi_uart_write_frame(CND_GET_CMD,MCU_TX_VER,0);
    EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_CND_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_INFO_CND_GET)
    {
        http_get_flush_value("getCND",6,device_status.info.cnd);
    }
    else
    {
        http_get_flush_value("getCND",6,0);
    }
}
int32_t http_net_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    printf("http_net_callback GET\r\n");
    wifi_uart_write_frame(NET_GET_CMD,MCU_TX_VER,0);
    EventValue = xEventGroupWaitBits(TEM_EventHandler,EVENT_TEM_NET_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_TEM_NET_GET)
    {
        http_get_flush_value("getNET",6,device_status.tem.net);
    }
    else
    {
        http_get_flush_value("getNET",6,0);
    }
}
int32_t http_bat_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    printf("http_bat_callback GET\r\n");
    wifi_uart_write_frame(BAT_GET_CMD,MCU_TX_VER,0);
    EventValue = xEventGroupWaitBits(TEM_EventHandler,EVENT_TEM_BAT_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_TEM_BAT_GET)
    {
        http_flush_value("getBAT",6,device_status.tem.bat);
    }
    else
    {
        http_flush_value("getBAT",6,0);
    }
}
int32_t http_ala_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    printf("http_ala_callback GET\r\n");
    wifi_uart_write_frame(ALA_GET_CMD,MCU_TX_VER,0);
    EventValue = xEventGroupWaitBits(TEM_EventHandler,EVENT_TEM_ALA_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_TEM_ALA_GET)
    {
        http_get_flush_value("getALA",6,device_status.tem.ala);
    }
    else
    {
        http_get_flush_value("getALA",6,0);
    }
}
int32_t http_alr_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    printf("http_alr_callback GET\r\n");
    wifi_uart_write_frame(ALR_GET_CMD,MCU_TX_VER,0);
    EventValue = xEventGroupWaitBits(TEM_EventHandler,EVENT_TEM_ALR_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_TEM_ALR_GET)
    {
        http_get_flush_value("getALR",6,device_status.tem.alr);
    }
    else
    {
        http_get_flush_value("getALR",6,0);
    }
}
int32_t http_sup_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    printf("http_sup_callback GET\r\n");
    wifi_uart_write_frame(SUP_GET_CMD,MCU_TX_VER,0);
    EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_SUP_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_INFO_SUP_GET)
    {
        http_get_flush_value("getSUP",6,device_status.info.sup);
    }
    else
    {
        http_get_flush_value("getSUP",6,0);
    }
}
int32_t http_rse_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    printf("http_rse_callback GET\r\n");
    wifi_uart_write_frame(RSE_GET_CMD,MCU_TX_VER,0);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_RSE_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_RSE_GET)
    {
        http_get_flush_value("getRSE",6,device_status.config.rse);
    }
    else
    {
        http_get_flush_value("getRSE",6,0);
    }
}
int32_t http_rse_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    EventBits_t EventValue;
    now_stream = stream;
    char* value_buf = flite_value(url_path,&size);
    value = atoi(value_buf);
    printf("http_rse_set_callback value %d,size %d\r\n",value,size);
    wifi_uart_write_command_value(RSE_SET_CMD,value);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_RSE_SET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_RSE_SET)
    {
         http_set_flush_value(WICED_SUCCESS,"setrse",6,device_status.config.rse);
    }
    else
    {
        http_set_flush_value(WICED_TIMEOUT,"setrse",6,0);
    }
}
int32_t http_rsa_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    printf("http_rsa_callback GET\r\n");
    wifi_uart_write_frame(RSA_GET_CMD,MCU_TX_VER,0);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_RSA_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_RSA_GET)
    {
        http_get_flush_value("getRSA",6,device_status.config.rsa);
    }
    else
    {
        http_get_flush_value("getRSA",6,0);
    }
}
int32_t http_rsa_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    EventBits_t EventValue;
    now_stream = stream;
    char* value_buf = flite_value(url_path,&size);
    value = atoi(value_buf);
    printf("http_rsa_set_callback value %d,size %d\r\n",value,size);
    wifi_uart_write_command_value(RSA_SET_CMD,value);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_RSA_SET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_RSA_SET)
    {
        http_set_flush_value(WICED_SUCCESS,"setrsa",device_status.config.rsa);
    }
    else
    {
        http_set_flush_value(WICED_TIMEOUT,"setrsa",0);
    }
}
int32_t http_rsi_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    printf("http_rsi_callback GET\r\n");
    wifi_uart_write_frame(RSI_GET_CMD,MCU_TX_VER,value);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_RSI_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_RSI_GET)
    {
        http_get_flush_value("getRSI",6,device_status.config.rsi);
    }
    else
    {
        http_get_flush_value("getRSI",6,0);
    }
}
int32_t http_rsi_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    EventBits_t EventValue;
    now_stream = stream;
    char* value_buf = flite_value(url_path,&size);
    value = atoi(value_buf);
    printf("http_rsi_set_callback value %d,size %d\r\n",value,size);
    wifi_uart_write_command_value(RSI_SET_CMD,target_value);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_RSI_SET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_RSI_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setrsi",device_status.config.rsi);
    }
    else
    {
        http_set_flush_value(ERRDATA_CODE,"setrsi",0);
    }
}
int32_t http_rsd_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    printf("http_rsd_callback GET\r\n");
    wifi_uart_write_frame(RSD_GET_CMD,MCU_TX_VER,value);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_RSD_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_RSD_GET)
    {
        http_get_flush_value("getRSD",6,device_status.config.rsd);
    }
    else
    {
        http_get_flush_value("getRSD",6,0);
    }
}
int32_t http_rsd_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    EventBits_t EventValue;
    now_stream = stream;
    char* value_buf = flite_value(url_path,&size);
    value = atoi(value_buf);
    printf("http_rsd_set_callback value %d,size %d\r\n",value,size);
    wifi_uart_write_command_value(RSD_SET_CMD,target_value);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_RSD_SET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_RSD_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setrsd",device_status.config.rsd);
    }
    else
    {
        http_set_flush_value(ERRDATA_CODE,"setrsd",0);
    }
}
int32_t http_cnf_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    printf("http_cnf_callback GET\r\n");
    wifi_uart_write_frame(CNF_GET_CMD,MCU_TX_VER,value);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_CNF_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_CNF_GET)
    {
        http_get_flush_value("getcnf",6,device_status.config.cnf);
    }
    else
    {
        http_get_flush_value("getcnf",6,0);
    }
}
int32_t http_cnf_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    EventBits_t EventValue;
    now_stream = stream;
    char* value_buf = flite_value(url_path,&size);
    value = atoi(value_buf);
    printf("http_cnf_set_callback value %d,size %d\r\n",value,size);
    wifi_uart_write_command_value(CNF_SET_CMD,target_value);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_CNF_SET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_CNF_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setCNF",device_status.config.cnf);
    }
    else
    {
        http_set_flush_value(ERRDATA_CODE,"setCNF",0);
    }
}
int32_t http_cnl_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    printf("http_cnl_callback GET\r\n");
    wifi_uart_write_frame(CNL_GET_CMD,MCU_TX_VER,value);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_CNL_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_CNL_GET)
    {
        http_get_flush_value("getCNL",6,device_status.config.cnl);
    }
    else
    {
        http_get_flush_value("getCNL",6,0);
    }
}
int32_t http_cnl_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    EventBits_t EventValue;
    now_stream = stream;
    char* value_buf = flite_value(url_path,&size);
    value = atoi(value_buf);
    printf("http_cnl_set_callback value %d,size %d\r\n",value,size);
    wifi_uart_write_command_value(CNL_SET_CMD,target_value);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_CNL_SET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_CNL_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setCNL",device_status.config.cnl);
    }
    else
    {
        http_set_flush_value(ERRDATA_CODE,"setCNL",0);
    }
}
int32_t http_sse_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    printf("http_sse_callback GET\r\n");
    wifi_uart_write_frame(SSE_GET_CMD,MCU_TX_VER,value);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_SSE_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_SSE_GET)
    {
        http_get_flush_value("getSSE",6,device_status.config.sse);
    }
    else
    {
        http_get_flush_value("getSSE",6,0);
    }
}
int32_t http_sse_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    EventBits_t EventValue;
    now_stream = stream;
    char* value_buf = flite_value(url_path,&size);
    value = atoi(value_buf);
    printf("http_sse_set_callback value %d,size %d\r\n",value,size);
    wifi_uart_write_command_value(SSE_SET_CMD,target_value);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_SSE_SET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_SSE_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setSSE",device_status.config.sse);
    }
    else
    {
        http_set_flush_value(ERRDATA_CODE,"setSSE",0);
    }
}
int32_t http_ssa_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    printf("http_ssa_callback GET\r\n");
    wifi_uart_write_frame(SSA_GET_CMD,MCU_TX_VER,value);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_SSA_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_SSA_GET)
    {
        http_get_flush_value("getSSA",6,device_status.config.ssa);
    }
    else
    {
        http_get_flush_value("getSSA",6,0);
    }
}
int32_t http_ssa_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    EventBits_t EventValue;
    now_stream = stream;
    char* value_buf = flite_value(url_path,&size);
    value = atoi(value_buf);
    printf("http_ssa_set_callback value %d,size %d\r\n",value,size);
    wifi_uart_write_command_value(SSA_SET_CMD,target_value);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_SSA_SET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_SSA_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setssa",device_status.config.ssa);
    }
    else
    {
        http_set_flush_value(ERRDATA_CODE,"setssa",0);
    }
}
int32_t http_ssd_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    printf("http_ssd_callback GET\r\n");
    wifi_uart_write_frame(SSD_GET_CMD,MCU_TX_VER,value);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_SSD_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_SSD_GET)
    {
        http_get_flush_value("getSSD",6,device_status.config.ssd);
    }
    else
    {
        http_get_flush_value("getSSD",6,0);
    }
}
int32_t http_ssd_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    EventBits_t EventValue;
    now_stream = stream;
    char* value_buf = flite_value(url_path,&size);
    printf("http_ssd_set_callback value %d,size %d\r\n",value,size);
    wifi_uart_write_command_value(SSD_SET_CMD,target_value);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_SSD_SET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_SSD_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setssd",device_status.config.ssd);
    }
    else
    {
        http_set_flush_value(ERRDATA_CODE,"setssd",0);
    }
}
int32_t http_lng_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    printf("http_lng_callback GET\r\n");
    wifi_uart_write_frame(LNG_GET_CMD,MCU_TX_VER,value);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_LNG_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_LNG_GET)
    {
        http_get_flush_value("getLNG",6,device_status.config.lng);
    }
    else
    {
        http_get_flush_value("getLNG",6,0);
    }
}
int32_t http_lng_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    EventBits_t EventValue;
    now_stream = stream;
    char* value_buf = flite_value(url_path,&size);
    printf("http_lng_set_callback value %d,size %d\r\n",value,size);
    wifi_uart_write_command_value(LNG_SET_CMD,target_value);
    EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_LNG_SET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_CONFIG_LNG_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setlng",device_status.config.lng);
    }
    else
    {
        http_set_flush_value(ERRDATA_CODE,"setlngh",0);
    }
}
int32_t http_ver_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    printf("http_ver_callback GET\r\n");
    wifi_uart_write_frame(VER_GET_CMD,MCU_TX_VER,value);
    EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_VER_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_INFO_VER_GET)
    {
        http_get_flush_value("getVER",6,device_status.info.ver);
    }
    else
    {
        http_get_flush_value("getVER",6,0);
    }
}
int32_t http_srn_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    printf("http_srn_callback GET\r\n");
    wifi_uart_write_frame(SRN_GET_CMD,MCU_TX_VER,value);
    EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_SRN_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_INFO_SRN_GET)
    {
        http_get_flush_value("getSRN",6,device_status.info.srn);
    }
    else
    {
        http_get_flush_value("getSRN",6,0);
    }
}
int32_t http_com_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    printf("http_com_callback GET\r\n");
    wifi_uart_write_frame(COM_GET_CMD,MCU_TX_VER,value);
    EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_COM_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_INFO_COM_GET)
    {
        http_get_flush_value("getCOM",6,device_status.info.com);
    }
    else
    {
        http_get_flush_value("getCOM",6,0);
    }
}
int32_t http_com_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    EventBits_t EventValue;
    now_stream = stream;
    char* value_buf = flite_value(url_path,&size);
    value = atoi(value_buf);
    printf("http_com_set_callback value %d,size %d\r\n",value,size);
    wifi_uart_write_command_value(COM_SET_CMD,target_value);
    EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_COM_SET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_INFO_COM_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setcom",device_status.info.com);
    }
    else
    {
        http_set_flush_value(ERRDATA_CODE,"setCOM",0);
    }
}
int32_t http_coa_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    printf("http_coa_callback GET\r\n");
    wifi_uart_write_frame(COA_GET_CMD,MCU_TX_VER,value);
    EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_COA_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_INFO_COA_GET)
    {
        http_get_flush_value("getCOA",6,device_status.info.coa);
    }
    else
    {
        http_get_flush_value("getCOA",6,0);
    }
}
int32_t http_coa_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    EventBits_t EventValue;
    now_stream = stream;
    char* value_buf = flite_value(url_path,&size);
    value = atoi(value_buf);
    printf("http_coa_set_callback value %d,size %d\r\n",value,size);
    wifi_uart_write_command_value(COA_SET_CMD,target_value);
    EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_COA_SET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_INFO_COA_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setcoa",device_status.info.coa);
    }
    else
    {
        http_set_flush_value(ERRDATA_CODE,"setcoa",0);
    }
}
int32_t http_cod_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    printf("http_cod_callback GET\r\n");
    wifi_uart_write_frame(COD_GET_CMD,MCU_TX_VER,value);
    EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_COD_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_INFO_COD_GET)
    {
        http_flush_value("getCOD",6,device_status.info.cod);
    }
    else
    {
        http_flush_value("getCOD",6,0);
    }
}
int32_t http_cod_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    EventBits_t EventValue;
    now_stream = stream;
    char* value_buf = flite_value(url_path,&size);
    value = atoi(value_buf);
    printf("http_cod_set_callback value %d,size %d\r\n",value,size);
    wifi_uart_write_command_value(COD_SET_CMD,target_value);
    EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_COD_SET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_INFO_COD_SET)
    {
        http_flush_value(SUCCESS_CODE,"setcod",device_status.info.cod);
    }
    else
    {
        http_flush_value(ERRDATA_CODE,"setcod",0);
    }
}
int32_t http_coe_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    printf("http_coe_callback GET\r\n");
    wifi_uart_write_frame(COE_GET_CMD,MCU_TX_VER,value);
    EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_COE_GET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_INFO_COE_GET)
    {
        http_flush_value("getCOE",6,device_status.info.coe);
    }
    else
    {
        http_flush_value("getCOE",6,0);
    }
}
int32_t http_coe_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    EventBits_t EventValue;
    now_stream = stream;
    char* value_buf = flite_value(url_path,&size);
    value = atoi(value_buf);
    printf("http_coe_set_callback value %d,size %d\r\n",value,size);
    wifi_uart_write_command_value(COE_SET_CMD,target_value);
    EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_COE_SET,pdTRUE,pdTRUE,100);
    if(EventValue & EVENT_INFO_COE_SET)
    {
        http_flush_value(SUCCESS_CODE,"setcoe",device_status.info.coe);
    }
    else
    {
        http_flush_value(ERRDATA_CODE,"setcoe",0);
    }
}
int32_t http_wfc_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    char *target_value = malloc(128);
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_wfc_callback GET\r\n");
        if(save_ssid.length)
        {
            http_flush_string(SUCCESS_CODE,"getWFC",save_ssid.value);
        }
        else
        {
            http_flush_string(ERRDATA_CODE,"getWFC",0);
        }
        break;
    case WICED_HTTP_POST_REQUEST:
        if(verification_string("value",http_message_body->data,http_message_body->message_data_length,target_value)==0)break;
        printf("http_wfc_callback POST,value %s,len is %d\r\n",target_value, strlen(target_value));
        if(strlen(target_value)<=32)
        {
            memcpy(&save_ssid.value,target_value, strlen(target_value));
            save_ssid.length = strlen(target_value);
            http_flush_string(SUCCESS_CODE,"setWFC",save_ssid.value);
        }
        else
        {
            http_flush_value(ERRDATA_CODE,"setWFC",0);
        }
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errWFC",0);
        break;
    }
    free(target_value);
    return 0;
}
int32_t http_wfc_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    char *target_value = malloc(128);
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_wfc_callback GET\r\n");
        if(save_ssid.length)
        {
            http_flush_string(SUCCESS_CODE,"getWFC",save_ssid.value);
        }
        else
        {
            http_flush_string(ERRDATA_CODE,"getWFC",0);
        }
        break;
    case WICED_HTTP_POST_REQUEST:
        if(verification_string("value",http_message_body->data,http_message_body->message_data_length,target_value)==0)break;
        printf("http_wfc_callback POST,value %s,len is %d\r\n",target_value, strlen(target_value));
        if(strlen(target_value)<=32)
        {
            memcpy(&save_ssid.value,target_value, strlen(target_value));
            save_ssid.length = strlen(target_value);
            http_flush_string(SUCCESS_CODE,"setWFC",save_ssid.value);
        }
        else
        {
            http_flush_value(ERRDATA_CODE,"setWFC",0);
        }
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errWFC",0);
        break;
    }
    free(target_value);
    return 0;
}
int32_t http_wfs_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t target_value = 0;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        http_flush_value(SUCCESS_CODE,"getWFS",save_security_id);
        printf("http_wfs_callback GET\r\n");
        break;
    case WICED_HTTP_POST_REQUEST:
        target_value = verification_value("value",http_message_body->data,http_message_body->message_data_length);
        save_security_id = target_value;
        http_flush_value(SUCCESS_CODE,"setWFS",save_security_id);
        printf("http_wfs_callback POST\r\n");
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errWFS",0);
        break;
    }
    return 0;
}
int32_t http_wfs_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t target_value = 0;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        http_flush_value(SUCCESS_CODE,"getWFS",save_security_id);
        printf("http_wfs_callback GET\r\n");
        break;
    case WICED_HTTP_POST_REQUEST:
        target_value = verification_value("value",http_message_body->data,http_message_body->message_data_length);
        save_security_id = target_value;
        http_flush_value(SUCCESS_CODE,"setWFS",save_security_id);
        printf("http_wfs_callback POST\r\n");
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errWFS",0);
        break;
    }
    return 0;
}
int32_t http_wfk_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    char *target_value = malloc(128);
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_wfk_callback GET\r\n");
        if(save_password.length)
        {
            http_flush_string(SUCCESS_CODE,"getWFK",save_password.data);
        }
        else
        {
            http_flush_value(ERRDATA_CODE,"getWFK",0);
        }
        break;
    case WICED_HTTP_POST_REQUEST:
        if(verification_string("value",http_message_body->data,http_message_body->message_data_length,target_value)==0)break;
        printf("http_wfk_callback POST\r\n");
        if(strlen(target_value)<=32)
        {
            memcpy(&save_password.data,target_value,strlen(target_value));
            save_password.length = strlen(target_value);
            http_flush_string(SUCCESS_CODE,"setWFK",save_password.data);
        }
        else
        {
            http_flush_value(ERRDATA_CODE,"setWFK",0);
        }
        break;
    default:
        http_flush_string(ERRREQS_CODE,"errWFK",0);
        break;
    }
    free(target_value);
    return 0;
}
int32_t http_wfk_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    char *target_value = malloc(128);
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_wfk_callback GET\r\n");
        if(save_password.length)
        {
            http_flush_string(SUCCESS_CODE,"getWFK",save_password.data);
        }
        else
        {
            http_flush_value(ERRDATA_CODE,"getWFK",0);
        }
        break;
    case WICED_HTTP_POST_REQUEST:
        if(verification_string("value",http_message_body->data,http_message_body->message_data_length,target_value)==0)break;
        printf("http_wfk_callback POST\r\n");
        if(strlen(target_value)<=32)
        {
            memcpy(&save_password.data,target_value,strlen(target_value));
            save_password.length = strlen(target_value);
            http_flush_string(SUCCESS_CODE,"setWFK",save_password.data);
        }
        else
        {
            http_flush_value(ERRDATA_CODE,"setWFK",0);
        }
        break;
    default:
        http_flush_string(ERRREQS_CODE,"errWFK",0);
        break;
    }
    free(target_value);
    return 0;
}
int32_t http_wsr_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint8_t ret;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_wsr_callback GET\r\n");
        ret = set_config(save_ssid,save_security_id,save_password);
        http_flush_value(SUCCESS_CODE,"getWSR",ret);
        rst_work();
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errWSR",0);
        break;
    }
    return 0;
}
int32_t http_azc_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint8_t ret;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("azc GET\r\n");
        azc_flush();
        break;
    case WICED_HTTP_POST_REQUEST:
        printf("azc post %s\r\n",http_message_body->data);
        azc_parse(http_message_body->data,http_message_body->message_data_length);
        printf("http_azc_callback POST\r\n");
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errWSR",0);
        break;
    }
    return 0;
}
