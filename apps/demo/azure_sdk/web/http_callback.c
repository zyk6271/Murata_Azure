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
wiced_wpa_key_t save_password;
wiced_http_response_stream_t *now_stream;

extern syr_status device_status;
extern wiced_event_flags_t Config_EventHandler;
extern wiced_event_flags_t Info_EventHandler;
extern wiced_event_flags_t TEM_EventHandler;
extern wiced_event_flags_t C2D_EventHandler;

#define LOG_D   printf

int32_t http_rst_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    uint32_t ret,events;
    now_stream = stream;
    char* value_buf = flite_value(url_path,(void *)&size);
    value = atoi(value_buf);
    LOG_D("http_rst_callback value %ld,size %ld\r\n",value,size);
    wifi_uart_write_command_value(RST_SET_CMD,1);
    ret = wiced_rtos_wait_for_event_flags(&C2D_EventHandler,EVENT_C2D_RST_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_C2D_RST_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setrst",6,value);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setrst",6,value);
    }
    free(value_buf);
    return ret;
}
int32_t http_def_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    uint32_t ret,events;
    now_stream = stream;
    char* value_buf = flite_value(url_path,(void *)&size);
    value = atoi(value_buf);
    LOG_D("http_def_set_callback value %ld,size %ld\r\n",value,size);
    wifi_uart_write_command_value(DEF_SET_CMD,1);
    ret = wiced_rtos_wait_for_event_flags(&C2D_EventHandler,EVENT_C2D_DEF_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_C2D_DEF_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setdef",6,value);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setdef",6,value);
    }
    return ret;
}
int32_t http_ras_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    uint32_t ret,events;
    now_stream = stream;
    char* value_buf = flite_value(url_path,(void *)&size);
    value = atoi(value_buf);
    LOG_D("http_def_set_callback value %ld,size %ld\r\n",value,size);
    wifi_uart_write_command_value(RAS_SET_CMD,1);
    ret = wiced_rtos_wait_for_event_flags(&C2D_EventHandler,EVENT_C2D_RAS_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_C2D_RAS_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setras",6,value);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setras",6,value);
    }
    return ret;
}
int32_t http_cnd_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_cnd_callback GET\r\n");
    wifi_uart_write_command_value(CND_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&Info_EventHandler,EVENT_INFO_CND_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_INFO_CND_GET)
    {
        http_get_flush_value("getCND",6,device_status.info.cnd);
    }
    else
    {
        http_get_flush_value("getCND",6,0);
    }
    return ret;
}
int32_t http_net_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_net_callback GET\r\n");
    wifi_uart_write_command_value(NET_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&TEM_EventHandler,EVENT_TEM_NET_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_TEM_NET_GET)
    {
        http_get_flush_value("getNET",6,device_status.tem.net);
    }
    else
    {
        http_get_flush_value("getNET",6,0);
    }
    return ret;
}
int32_t http_bat_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_bat_callback GET\r\n");
    wifi_uart_write_command_value(BAT_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&TEM_EventHandler,EVENT_TEM_BAT_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_TEM_BAT_GET)
    {
        http_get_flush_value("getBAT",6,device_status.tem.bat);
    }
    else
    {
        http_get_flush_value("getBAT",6,0);
    }
    return ret;
}
int32_t http_ala_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_ala_callback GET\r\n");
    wifi_uart_write_command_value(ALA_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&TEM_EventHandler,EVENT_TEM_ALA_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_TEM_ALA_GET)
    {
        http_get_flush_value("getALA",6,device_status.tem.ala);
    }
    else
    {
        http_get_flush_value("getALA",6,0);
    }
    return ret;
}
int32_t http_alr_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_alr_callback GET\r\n");
    wifi_uart_write_command_value(ALR_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&TEM_EventHandler,EVENT_TEM_ALR_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_TEM_ALR_GET)
    {
        http_get_flush_value("getALR",6,device_status.tem.alr);
    }
    else
    {
        http_get_flush_value("getALR",6,0);
    }
    return ret;
}
int32_t http_sup_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_sup_callback GET\r\n");
    wifi_uart_write_command_value(SUP_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&Info_EventHandler,EVENT_INFO_SUP_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_INFO_SUP_GET)
    {
        http_get_flush_value("getSUP",6,device_status.info.sup);
    }
    else
    {
        http_get_flush_value("getSUP",6,0);
    }
    return ret;
}
int32_t http_rse_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_rse_callback GET\r\n");
    wifi_uart_write_command_value(RSE_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_RSE_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_RSE_GET)
    {
        http_get_flush_value("getRSE",6,device_status.config.rse);
    }
    else
    {
        http_get_flush_value("getRSE",6,0);
    }
    return ret;
}
int32_t http_rse_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    uint32_t ret,events;
    now_stream = stream;
    char* value_buf = flite_value(url_path,(void *)&size);
    value = atoi(value_buf);
    LOG_D("http_rse_set_callback value %ld,size %ld\r\n",value,size);
    wifi_uart_write_command_value(RSE_SET_CMD,value);
    ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_RSE_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_RSE_SET)
    {
         http_set_flush_value(SUCCESS_CODE,"setrse",6,device_status.config.rse);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setrse",6,device_status.config.rse);
    }
    return ret;
}
int32_t http_rsa_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_rsa_callback GET\r\n");
    wifi_uart_write_command_value(RSA_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_RSA_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_RSA_GET)
    {
        http_get_flush_value("getRSA",6,device_status.config.rsa);
    }
    else
    {
        http_get_flush_value("getRSA",6,0);
    }
    return ret;
}
int32_t http_rsa_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    uint32_t ret,events;
    now_stream = stream;
    char* value_buf = flite_value(url_path,(void *)&size);
    value = atoi(value_buf);
    LOG_D("http_rsa_set_callback value %ld,size %ld\r\n",value,size);
    wifi_uart_write_command_value(RSA_SET_CMD,value);
    ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_RSA_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_RSA_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setrsa",6,device_status.config.rsa);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setrsa",6,device_status.config.rsa);
    }
    return ret;
}
int32_t http_rsi_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_rsi_callback GET\r\n");
    wifi_uart_write_command_value(RSI_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_RSI_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_RSI_GET)
    {
        http_get_flush_value("getRSI",6,device_status.config.rsi);
    }
    else
    {
        http_get_flush_value("getRSI",6,0);
    }
    return ret;
}
int32_t http_rsi_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    uint32_t ret,events;
    now_stream = stream;
    char* value_buf = flite_value(url_path,(void *)&size);
    value = atoi(value_buf);
    LOG_D("http_rsi_set_callback value %ld,size %ld\r\n",value,size);
    wifi_uart_write_command_value(RSI_SET_CMD,value);
    ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_RSI_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_RSI_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setrsi",6,device_status.config.rsi);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setrsi",6,device_status.config.rsi);
    }
    return ret;
}
int32_t http_rsd_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_rsd_callback GET\r\n");
    wifi_uart_write_command_value(RSD_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_RSD_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_RSD_GET)
    {
        http_get_flush_value("getRSD",6,device_status.config.rsd);
    }
    else
    {
        http_get_flush_value("getRSD",6,0);
    }
    return ret;
}
int32_t http_rsd_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    uint32_t ret,events;
    now_stream = stream;
    char* value_buf = flite_value(url_path,(void *)&size);
    value = atoi(value_buf);
    LOG_D("http_rsd_set_callback value %ld,size %ld\r\n",value,size);
    wifi_uart_write_command_value(RSD_SET_CMD,value);
    ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_RSD_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_RSD_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setrsd",6,device_status.config.rsd);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setrsd",6,device_status.config.rsd);
    }
    return ret;
}
int32_t http_cnf_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_cnf_callback GET\r\n");
    wifi_uart_write_command_value(CNF_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_CNF_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_CNF_GET)
    {
        http_get_flush_value("getCNF",6,device_status.config.cnf);
    }
    else
    {
        http_get_flush_value("getCNF",6,0);
    }
    return ret;
}
int32_t http_cnf_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    uint32_t ret,events;
    now_stream = stream;
    char* value_buf = flite_value(url_path,(void *)&size);
    value = atoi(value_buf);
    LOG_D("http_cnf_set_callback value %ld,size %ld\r\n",value,size);
    wifi_uart_write_command_value(CNF_SET_CMD,value);
    ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_CNF_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_CNF_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setcnf",6,device_status.config.cnf);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setcnf",6,device_status.config.cnf);
    }
    return ret;
}
int32_t http_cnl_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_cnl_callback GET\r\n");
    wifi_uart_write_command_value(CNL_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_CNL_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_CNL_GET)
    {
        http_get_flush_value("getCNL",6,device_status.config.cnl);
    }
    else
    {
        http_get_flush_value("getCNL",6,0);
    }
    return ret;
}
int32_t http_cnl_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    uint32_t ret,events;
    now_stream = stream;
    char* value_buf = flite_value(url_path,(void *)&size);
    value = atoi(value_buf);
    LOG_D("http_cnl_set_callback value %ld,size %ld\r\n",value,size);
    wifi_uart_write_command_value(CNL_SET_CMD,value);
    ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_CNL_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_CNL_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setcnl",6,device_status.config.cnl);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setcnl",6,device_status.config.cnl);
    }
    return ret;
}
int32_t http_sse_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_sse_callback GET\r\n");
    wifi_uart_write_command_value(SSE_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_SSE_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_SSE_GET)
    {
        http_get_flush_value("getSSE",6,device_status.config.sse);
    }
    else
    {
        http_get_flush_value("getSSE",6,0);
    }
    return ret;
}
int32_t http_sse_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    uint32_t ret,events;
    now_stream = stream;
    char* value_buf = flite_value(url_path,(void *)&size);
    value = atoi(value_buf);
    LOG_D("http_sse_set_callback value %ld,size %ld\r\n",value,size);
    wifi_uart_write_command_value(SSE_SET_CMD,value);
    ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_SSE_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_SSE_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setsse",6,device_status.config.sse);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setsse",6,device_status.config.sse);
    }
    return ret;
}
int32_t http_ssa_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_ssa_callback GET\r\n");
    wifi_uart_write_command_value(SSA_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_SSA_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_SSA_GET)
    {
        http_get_flush_value("getSSA",6,device_status.config.ssa);
    }
    else
    {
        http_get_flush_value("getSSA",6,0);
    }
    return ret;
}
int32_t http_ssa_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    uint32_t ret,events;
    now_stream = stream;
    char* value_buf = flite_value(url_path,(void *)&size);
    value = atoi(value_buf);
    LOG_D("http_ssa_set_callback value %ld,size %ld\r\n",value,size);
    wifi_uart_write_command_value(SSA_SET_CMD,value);
    ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_SSA_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_SSA_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setssa",6,device_status.config.ssa);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setssa",6,device_status.config.ssa);
    }
    return ret;
}
int32_t http_ssd_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_ssd_callback GET\r\n");
    wifi_uart_write_command_value(SSD_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_SSD_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_SSD_GET)
    {
        http_get_flush_value("getSSD",6,device_status.config.ssd);
    }
    else
    {
        http_get_flush_value("getSSD",6,0);
    }
    return ret;
}
int32_t http_ssd_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    uint32_t ret,events;
    now_stream = stream;
    char* value_buf = flite_value(url_path,(void *)&size);
    value = atoi(value_buf);
    LOG_D("http_ssd_set_callback value %ld,size %ld\r\n",value,size);
    wifi_uart_write_command_value(SSD_SET_CMD,value);
    ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_SSD_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_SSD_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setssd",6,device_status.config.ssd);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setssd",6,device_status.config.ssd);
    }
    return ret;
}
int32_t http_lng_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_lng_callback GET\r\n");
    wifi_uart_write_command_value(LNG_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_LNG_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_LNG_GET)
    {
        http_get_flush_value("getLNG",6,device_status.config.lng);
    }
    else
    {
        http_get_flush_value("getLNG",6,0);
    }
    return ret;
}
int32_t http_lng_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    uint32_t ret,events;
    now_stream = stream;
    char* value_buf = flite_value(url_path,(void *)&size);
    value = atoi(value_buf);
    LOG_D("http_lng_set_callback value %ld,size %ld\r\n",value,size);
    wifi_uart_write_command_value(LNG_SET_CMD,value);
    ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_LNG_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_LNG_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setlng",6,device_status.config.lng);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setlng",6,device_status.config.lng);
    }
    return ret;
}
int32_t http_ver_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    now_stream = stream;
    LOG_D("http_ver_callback GET\r\n");
    http_get_flush_string("getVER",6,device_status.info.ver,strlen(device_status.info.ver));
    return 0;
}
int32_t http_ver2_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    extern char wifi_version[];
    now_stream = stream;
    LOG_D("http_ver2_callback GET\r\n");
    http_get_flush_string("getVER2",7,wifi_version,strlen(wifi_version));
    return 0;
}
int32_t http_srn_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    now_stream = stream;
    LOG_D("http_srn_callback GET\r\n");
    if(strlen(device_status.info.srn))
    {
        http_get_flush_string("getSRN",6,device_status.info.srn,strlen(device_status.info.srn));
    }
    else
    {
        http_get_flush_value("getSRN",6,0);
    }
    return 0;
}
int32_t http_srn2_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    char *id = murata_id_read();
    now_stream = stream;
    LOG_D("http_srn2_callback GET\r\n");
    http_get_flush_string("getSRN2",7 ,id,strlen(id));
    return 0;
}
int32_t http_com_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_com_callback GET\r\n");
    wifi_uart_write_command_value(COM_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&Info_EventHandler,EVENT_INFO_COM_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_INFO_COM_GET)
    {
        http_get_flush_value("getCOM",6,device_status.info.com);
    }
    else
    {
        http_get_flush_value("getCOM",6,0);
    }
    return ret;
}
int32_t http_com_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    uint32_t ret,events;
    now_stream = stream;
    char* value_buf = flite_value(url_path,(void *)&size);
    value = atoi(value_buf);
    LOG_D("http_com_set_callback value %ld,size %ld\r\n",value,size);
    wifi_uart_write_command_value(COM_SET_CMD,value);
    ret = wiced_rtos_wait_for_event_flags(&Info_EventHandler,EVENT_INFO_COM_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_INFO_COM_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setcom",6,device_status.info.com);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setcom",6,device_status.info.com);
    }
    return ret;
}
int32_t http_coa_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_coa_callback GET\r\n");
    wifi_uart_write_command_value(COA_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&Info_EventHandler,EVENT_INFO_COA_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_INFO_COA_GET)
    {
        http_get_flush_value("getCOA",6,device_status.info.coa);
    }
    else
    {
        http_get_flush_value("getCOA",6,0);
    }
    return ret;
}
int32_t http_coa_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    uint32_t ret,events;
    now_stream = stream;
    char* value_buf = flite_value(url_path,(void *)&size);
    value = atoi(value_buf);
    LOG_D("http_coa_set_callback value %ld,size %ld\r\n",value,size);
    wifi_uart_write_command_value(COA_SET_CMD,value);
    ret = wiced_rtos_wait_for_event_flags(&Info_EventHandler,EVENT_INFO_COA_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_INFO_COA_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setcoa",6,device_status.info.coa);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setcoa",6,device_status.info.coa);
    }
    return ret;
}
int32_t http_cod_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_cod_callback GET\r\n");
    wifi_uart_write_command_value(COD_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&Info_EventHandler,EVENT_INFO_COD_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_INFO_COD_GET)
    {
        http_get_flush_value("getCOD",6,device_status.info.cod);
    }
    else
    {
        http_get_flush_value("getCOD",6,0);
    }
    return ret;
}
int32_t http_cod_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    uint32_t ret,events;
    now_stream = stream;
    char* value_buf = flite_value(url_path,(void *)&size);
    value = atoi(value_buf);
    LOG_D("http_cod_set_callback value %ld,size %ld\r\n",value,size);
    wifi_uart_write_command_value(COD_SET_CMD,value);
    ret = wiced_rtos_wait_for_event_flags(&Info_EventHandler,EVENT_INFO_COD_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_INFO_COD_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setcod",6,device_status.info.cod);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setcod",6,device_status.info.cod);
    }
    return ret;
}
int32_t http_coe_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_coe_callback GET\r\n");
    wifi_uart_write_command_value(COE_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&Info_EventHandler,EVENT_INFO_COE_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_INFO_COE_GET)
    {
        http_get_flush_value("getCOE",6,device_status.info.coe);
    }
    else
    {
        http_get_flush_value("getCOE",6,0);
    }
    return ret;
}
int32_t http_coe_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size,value;
    uint32_t ret,events;
    now_stream = stream;
    char* value_buf = flite_value(url_path,(void *)&size);
    value = atoi(value_buf);
    LOG_D("http_coe_set_callback value %ld,size %ld\r\n",value,size);
    wifi_uart_write_command_value(COE_SET_CMD,value);
    ret = wiced_rtos_wait_for_event_flags(&Info_EventHandler,EVENT_INFO_COE_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_INFO_COE_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setcoe",6,device_status.info.coe);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setcoe",6,device_status.info.coe);
    }
    return ret;
}
int32_t http_wfc_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    now_stream = stream;
    LOG_D("http_wfc_callback GET\r\n");
    if(save_ssid.length)
    {
        http_get_flush_string("getWFC",6,(char*)save_ssid.value,save_ssid.length);
    }
    else
    {
        http_get_flush_value("getWFC",6,0);
    }
    return 0;
}
int32_t http_wfc_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size;
    now_stream = stream;
    char* value_buf = flite_value(url_path,(void *)&size);
    LOG_D("http_wfc_callback POST,value %s,len is %ld\r\n",value_buf, size);
    if(size<=32)
    {
        memcpy(&save_ssid.value,value_buf,size);
        save_ssid.length = size;
        http_set_flush_string(SUCCESS_CODE,"setwfc",6,(char*)save_ssid.value,save_ssid.length);
        set_config(save_ssid,save_password);
        rst_work();
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setwfc",6,0);
    }
    return 0;
}
int32_t http_wfk_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    now_stream = stream;
    LOG_D("http_wfk_callback GET\r\n");
    if(save_password.length)
    {
        http_get_flush_string("getWFK",6,(char*)save_password.data,save_password.length);
    }
    else
    {
        http_get_flush_value("getWFK",6,0);
    }
    return 0;
}
int32_t http_wfk_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t size;
    now_stream = stream;
    char* value_buf = flite_value(url_path,(void *)&size);
    LOG_D("http_wfk_callback POST,value %s,len is %ld\r\n",value_buf, size);
    if(size<=64)
    {
        memcpy(&save_password.data,value_buf,size);
        save_password.length = size;
        http_set_flush_string(SUCCESS_CODE,"setwfk",6,(char*)save_password.data,save_password.length);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setwfk",6,0);
    }
    return 0;
}
int32_t http_wfs_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    now_stream = stream;
    LOG_D("http_wfs_callback GET\r\n");
    http_set_flush_value(SUCCESS_CODE,"getwfs",6,wifi_status_get());
    return 0;
}
int32_t http_azc_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        LOG_D("http_azc_get_callback\r\n");
        azc_flush();
        break;
    default:break;
    }
    return 0;
}
int32_t http_azc_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_POST_REQUEST:
        LOG_D("http_azc_set_callback %s\r\n",http_message_body->data);
        azc_parse(http_message_body->data,http_message_body->message_data_length);
        azc_flush();
        break;
    default:break;
    }
    return 0;
}
