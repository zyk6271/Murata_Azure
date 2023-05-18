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

void http_rst_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t events;
    now_stream = stream;
    char* value_buf = malloc(32);
    uint32_t size = extract_value_from_path(url_path, value_buf);
    uint32_t value = atoi(value_buf);
    free(value_buf);
    LOG_D("http_rst_callback value %ld,size %ld\r\n",value,size);
    if (value < 0 || value > 1)
    {
        http_set_flush_value(ERRDATA_CODE, "setRST", 6, value);
        return;
    }
    wifi_uart_write_frame(DEVICE_REBOOT_CMD, MCU_TX_VER, 0);
    wiced_rtos_wait_for_event_flags(&C2D_EventHandler,EVENT_C2D_RST_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_C2D_RST_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setRST",6,value);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setRST",6,value);
    }
}
void http_def_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t events;
    now_stream = stream;
    char* value_buf = malloc(32);
    uint32_t size = extract_value_from_path(url_path, value_buf);
    uint32_t value = atoi(value_buf);
    free(value_buf);
    LOG_D("http_def_set_callback value %ld,size %ld\r\n",value,size);
    if (value < 0 || value > 1)
    {
        http_set_flush_value(ERRDATA_CODE, "setDEF", 6, value);
        return;
    }
    wifi_uart_write_frame(FACTORY_SET_CMD, MCU_TX_VER, 0);
    wiced_rtos_wait_for_event_flags(&C2D_EventHandler,EVENT_C2D_DEF_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_C2D_DEF_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setDEF",6,value);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setDEF",6,value);
    }
}
void http_ras_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t events;
    now_stream = stream;
    char* value_buf = malloc(32);
    uint32_t size = extract_value_from_path(url_path, value_buf);
    uint32_t value = atoi(value_buf);
    free(value_buf);
    LOG_D("http_def_set_callback value %ld,size %ld\r\n",value,size);
    if (value < 0 || value > 1)
    {
        http_set_flush_value(ERRDATA_CODE, "setRAS", 6, value);
        return;
    }
    wifi_uart_write_command_value(RAS_SET_CMD,1);
    wiced_rtos_wait_for_event_flags(&C2D_EventHandler,EVENT_C2D_RAS_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_C2D_RAS_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setRAS",6,value);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setRAS",6,value);
    }
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

void http_rse_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t events;
    now_stream = stream;
    char* value_buf = malloc(32);
    uint32_t size = extract_value_from_path(url_path, value_buf);
    uint32_t value = atoi(value_buf);
    free(value_buf);
    LOG_D("http_rse_set_callback value %ld,size %ld\r\n",value,size);
    if (value < 1 || value > 365)
    {
        http_set_flush_value(ERRDATA_CODE, "setRSE", 6, value);
        return;
    }
    wifi_uart_write_command_value(RSE_SET_CMD,value);
    wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_RSE_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_RSE_SET)
    {
         http_set_flush_value(SUCCESS_CODE,"setRSE",6,device_status.config.rse);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setRSE",6,device_status.config.rse);
    }
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
void http_rsa_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t events;
    now_stream = stream;
    char* value_buf = malloc(32);
    uint32_t size = extract_value_from_path(url_path, value_buf);
    uint32_t value = atoi(value_buf);
    free(value_buf);
    LOG_D("http_rsa_set_callback value %ld,size %ld\r\n",value,size);
    if (value < 1 || value > 365)
    {
        http_set_flush_value(ERRDATA_CODE, "setRSA", 6, value);
        return;
    }
    wifi_uart_write_command_value(RSA_SET_CMD,value);
    wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_RSA_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_RSA_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setRSA",6,device_status.config.rsa);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setRSA",6,device_status.config.rsa);
    }
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
void http_rsi_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t events;
    now_stream = stream;
    char* value_buf = malloc(32);
    uint32_t size = extract_value_from_path(url_path, value_buf);
    uint32_t value = atoi(value_buf);
    free(value_buf);
    LOG_D("http_rsi_set_callback value %ld,size %ld\r\n",value,size);
    if (value < 0 || value > 1)
    {
        http_set_flush_value(ERRDATA_CODE, "setRSI", 6, value);
        return;
    }
    wifi_uart_write_command_value(RSI_SET_CMD,value);
    wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_RSI_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_RSI_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setRSI",6,device_status.config.rsi);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setRSI",6,device_status.config.rsi);
    }
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
void http_rsd_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t events;
    now_stream = stream;
    char* value_buf = malloc(32);
    uint32_t size = extract_value_from_path(url_path, value_buf);
    uint32_t value = atoi(value_buf);
    free(value_buf);
    LOG_D("http_rsd_set_callback value %ld,size %ld\r\n",value,size);
    if (value < 1 || value > 100)
    {
        http_set_flush_value(ERRDATA_CODE, "setRSD", 6, value);
        return;
    }
    wifi_uart_write_command_value(RSD_SET_CMD,value);
    wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_RSD_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_RSD_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setRSD",6,device_status.config.rsd);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setRSD",6,device_status.config.rsd);
    }
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
void http_cnf_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t events;
    now_stream = stream;
    char* value_buf = malloc(32);
    uint32_t size = extract_value_from_path(url_path, value_buf);
    uint32_t value = atoi(value_buf);
    free(value_buf);
    LOG_D("http_cnf_set_callback value %ld,size %ld\r\n",value,size);
    if (value < 5 || value > 50)
    {
        http_set_flush_value(ERRDATA_CODE, "setCNF", 6, value);
        return;
    }
    wifi_uart_write_command_value(CNF_SET_CMD,value);
    wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_CNF_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_CNF_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setCNF",6,device_status.config.cnf);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setCNF",6,device_status.config.cnf);
    }
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
void http_cnl_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t events;
    now_stream = stream;
    char* value_buf = malloc(32);
    uint32_t size = extract_value_from_path(url_path, value_buf);
    uint32_t value = atoi(value_buf);
    free(value_buf);
    LOG_D("http_cnl_set_callback value %ld,size %ld\r\n",value,size);
    if (value < 0 || value > 5000)
    {
        http_set_flush_value(ERRDATA_CODE, "setCNL", 6, value);
        return;
    }
    wifi_uart_write_command_value(CNL_SET_CMD,value);
    wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_CNL_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_CNL_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setCNL",6,device_status.config.cnl);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setCNL",6,device_status.config.cnl);
    }
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
void http_sse_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t events;
    now_stream = stream;
    char* value_buf = malloc(32);
    uint32_t size = extract_value_from_path(url_path, value_buf);
    uint32_t value = atoi(value_buf);
    free(value_buf);
    LOG_D("http_sse_set_callback value %ld,size %ld\r\n",value,size);
    if (value < 0 || value > 1)
    {
        http_set_flush_value(ERRDATA_CODE, "setSSE", 6, value);
        return;
    }
    wifi_uart_write_command_value(SSE_SET_CMD,value);
    wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_SSE_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_SSE_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setSSE",6,device_status.config.sse);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setSSE",6,device_status.config.sse);
    }
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
void http_ssa_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t events;
    now_stream = stream;
    char* value_buf = malloc(32);
    uint32_t size = extract_value_from_path(url_path, value_buf);
    uint32_t value = atoi(value_buf);
    free(value_buf);
    LOG_D("http_ssa_set_callback value %ld,size %ld\r\n",value,size);
    if (value < 0 || value > 1)
    {
        http_set_flush_value(ERRDATA_CODE, "setSSA", 6, value);
        return;
    }
    wifi_uart_write_command_value(SSA_SET_CMD,value);
    wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_SSA_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_SSA_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setSSA",6,device_status.config.ssa);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setSSA",6,device_status.config.ssa);
    }
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
void http_ssd_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t events;
    now_stream = stream;
    char* value_buf = malloc(32);
    uint32_t size = extract_value_from_path(url_path, value_buf);
    uint32_t value = atoi(value_buf);
    free(value_buf);
    LOG_D("http_ssd_set_callback value %ld,size %ld\r\n",value,size);
    if (value < 0 || value > 1)
    {
        http_set_flush_value(ERRDATA_CODE, "setSSD", 6, value);
        return;
    }
    wifi_uart_write_command_value(SSD_SET_CMD,value);
    wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_SSD_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_SSD_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setSSD",6,device_status.config.ssd);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setSSD",6,device_status.config.ssd);
    }
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
void http_lng_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t events;
    now_stream = stream;
    char* value_buf = malloc(32);
    uint32_t size = extract_value_from_path(url_path, value_buf);
    uint32_t value = atoi(value_buf);
    free(value_buf);
    LOG_D("http_lng_set_callback value %ld,size %ld\r\n",value,size);
    if (value < 0 || value > 1)
    {
        http_set_flush_value(ERRDATA_CODE, "setLNG", 6, value);
        return;
    }
    wifi_uart_write_command_value(LNG_SET_CMD,value);
    wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_LNG_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_LNG_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setLNG",6,device_status.config.lng);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setLNG",6,device_status.config.lng);
    }
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
    uint8_t device_buffer[32];
    dct_read_device_id(device_buffer);
    now_stream = stream;
    LOG_D("http_srn_callback GET\r\n");
    http_get_flush_string("getSRN",7 ,device_buffer,strlen(device_buffer));
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
void http_com_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t events;
    now_stream = stream;
    char* value_buf = malloc(32);
    uint32_t size = extract_value_from_path(url_path, value_buf);
    uint32_t value = atoi(value_buf);
    free(value_buf);
    LOG_D("http_com_set_callback value %ld,size %ld\r\n",value,size);
    if (value > 0)
    {
        http_set_flush_value(ERRDATA_CODE, "setCOM", 6, value);
        return;
    }
    wifi_uart_write_command_value(COM_SET_CMD,value);
    wiced_rtos_wait_for_event_flags(&Info_EventHandler,EVENT_INFO_COM_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_INFO_COM_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setCOM",6,device_status.info.com);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setCOM",6,device_status.info.com);
    }
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
void http_coa_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t events;
    now_stream = stream;
    char* value_buf = malloc(32);
    uint32_t size = extract_value_from_path(url_path, value_buf);
    uint32_t value = atoi(value_buf);
    free(value_buf);
    LOG_D("http_coa_set_callback value %ld,size %ld\r\n",value,size);
    if (value > 0)
    {
        http_set_flush_value(ERRDATA_CODE, "setCOA", 6, value);
        return;
    }
    wifi_uart_write_command_value(COA_SET_CMD,value);
    wiced_rtos_wait_for_event_flags(&Info_EventHandler,EVENT_INFO_COA_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_INFO_COA_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setCOA",6,device_status.info.coa);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setCOA",6,device_status.info.coa);
    }
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
void http_cod_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t events;
    now_stream = stream;
    char* value_buf = malloc(32);
    uint32_t size = extract_value_from_path(url_path, value_buf);
    uint32_t value = atoi(value_buf);
    free(value_buf);
    LOG_D("http_cod_set_callback value %ld,size %ld\r\n",value,size);
    if (value > 0)
    {
        http_set_flush_value(ERRDATA_CODE, "setCOD", 6, value);
        return;
    }
    wifi_uart_write_command_value(COD_SET_CMD,value);
    wiced_rtos_wait_for_event_flags(&Info_EventHandler,EVENT_INFO_COD_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_INFO_COD_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setCOD",6,device_status.info.cod);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setCOD",6,device_status.info.cod);
    }
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
void http_coe_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t events;
    now_stream = stream;
    char* value_buf = malloc(32);
    uint32_t size = extract_value_from_path(url_path, value_buf);
    uint32_t value = atoi(value_buf);
    free(value_buf);
    LOG_D("http_coe_set_callback value %ld,size %ld\r\n",value,size);
    if (value > 0)
    {
        http_set_flush_value(ERRDATA_CODE, "setCOE", 6, value);
        return;
    }
    wifi_uart_write_command_value(COE_SET_CMD,value);
    wiced_rtos_wait_for_event_flags(&Info_EventHandler,EVENT_INFO_COE_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_INFO_COE_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setCOE",6,device_status.info.coe);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setCOE",6,device_status.info.coe);
    }
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
        http_set_flush_string(SUCCESS_CODE,"setWFC",6,(char*)save_ssid.value,save_ssid.length);
        rst_work();
        set_config(save_ssid,save_password);
        wfc_start_request();
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setWFC",6,0);
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
        http_set_flush_string(SUCCESS_CODE,"setWFK",6,(char*)save_password.data,save_password.length);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setWFK",6,0);
    }
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
        rst_work();
        azc_parse(http_message_body->data,http_message_body->message_data_length);
        azc_flush();
        break;
    default:break;
    }
    return 0;
}
int32_t http_nsc_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    char *value_from_url = NULL;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        LOG_D("http_nsc_get_callback url_path is %s\r\n",url_path);
        flite_extract_get_path(url_path,&value_from_url);
        LOG_D("flite_url is %s\r\n",value_from_url);
        http_get_flush_string(value_from_url,strlen(value_from_url),"NSC",3);
        free(value_from_url);
        break;
    default:
        break;
    }
    return 0;
}
void http_nsc_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    char* value_from_url = NULL;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        LOG_D("http_nsc_set_callbacks\r\n");
        flite_extract_set_path(url_path,&value_from_url);
        LOG_D("flite_url is %s\r\n",value_from_url);
        http_get_flush_string(value_from_url,strlen(value_from_url),"NSC",3);
        free(value_from_url);
        break;
    default:
        break;
    }
}
int32_t http_vlv_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_vlv_get_callback GET\r\n");
    wifi_uart_write_command_value(VLV_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&Info_EventHandler,EVENT_INFO_VLV_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_INFO_VLV_GET)
    {
        http_get_flush_value("getVLV",6,device_status.info.vlv);
    }
    else
    {
        http_get_flush_value("getVLV",6,0);
    }
    return ret;
}

int32_t http_wfs_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_wfs_get_callback GET\r\n");
    switch(wifi_status_get())
    {
    case 0:
        http_get_flush_value("getWFS",6,0);
        break;
    case 1:
        http_get_flush_value("getWFS",6,0);
        break;
    case 2:
        http_get_flush_value("getWFS",6,1);
        break;
    case 3:
        http_get_flush_value("getWFS",6,1);
        break;
    case 4:
        http_get_flush_value("getWFS",6,2);
        break;
    default:
        break;
    }
    return ret;
}

int32_t http_alm_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_alm_callback GET\r\n");
    wifi_uart_write_command_value(ALM_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&Info_EventHandler,EVENT_INFO_ALM_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_INFO_ALM_GET)
    {
        http_get_flush_string("getALM",6,device_status.info.alm_array,strlen(device_status.info.alm_array));
    }
    else
    {
        http_get_flush_value("getALM",6,0);
    }
    return ret;
}

int32_t http_aln_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_aln_callback GET\r\n");
    wifi_uart_write_command_value(ALN_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&Info_EventHandler,EVENT_INFO_ALN_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_INFO_ALN_GET)
    {
        http_get_flush_string("getALN",6,device_status.info.aln_array,strlen(device_status.info.aln_array));
    }
    else
    {
        http_get_flush_value("getALN",6,0);
    }
    return ret;
}

int32_t http_alw_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_alw_callback GET\r\n");
    wifi_uart_write_command_value(ALW_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&Info_EventHandler,EVENT_INFO_ALW_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_INFO_ALW_GET)
    {
        http_get_flush_string("getALW",6,device_status.info.alw_array,strlen(device_status.info.alw_array));
    }
    else
    {
        http_get_flush_value("getALW",6,0);
    }
    return ret;
}

int32_t http_ala_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_ala_get_callback GET\r\n");
    wifi_uart_write_command_value(ALA_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&Info_EventHandler,EVENT_INFO_ALA_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_INFO_ALA_GET)
    {
        http_get_flush_value("getALA",6,device_status.info.ala);
    }
    else
    {
        http_get_flush_value("getALA",6,0);
    }
    return ret;
}

int32_t http_not_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_not_get_callback GET\r\n");
    wifi_uart_write_command_value(NOT_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&Info_EventHandler,EVENT_INFO_NOT_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_INFO_NOT_GET)
    {
        http_get_flush_value("getNOT",6,device_status.info.not);
    }
    else
    {
        http_get_flush_value("getNOT",6,0);
    }
    return ret;
}

int32_t http_wrn_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_wrn_get_callback GET\r\n");
    wifi_uart_write_command_value(WRN_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&Info_EventHandler,EVENT_INFO_WRN_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_INFO_WRN_GET)
    {
        http_get_flush_value("getWRN",6,device_status.info.wrn);
    }
    else
    {
        http_get_flush_value("getWRN",6,0);
    }
    return ret;
}

int32_t http_apt_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_apt_get_callback GET\r\n");
    wifi_uart_write_command_value(APT_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_APT_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_APT_GET)
    {
        http_get_flush_value("getAPT",6,device_status.config.apt);
    }
    else
    {
        http_get_flush_value("getAPT",6,0);
    }
    return ret;
}
void http_apt_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t events;
    now_stream = stream;
    char* value_buf = malloc(32);
    uint32_t size = extract_value_from_path(url_path, value_buf);
    uint32_t value = atoi(value_buf);
    free(value_buf);
    LOG_D("http_apt_set_callback value %ld,size %ld\r\n",value,size);
    if (value < 0 || value > 3600)
    {
        http_set_flush_value(ERRDATA_CODE, "setAPT", 6, value);
        return;
    }
    wifi_uart_write_command_value(APT_SET_CMD,value);
    wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_APT_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_APT_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setAPT",6,device_status.config.apt);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setAPT",6,device_status.config.apt);
    }
}

int32_t http_emr_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_emr_get_callback GET\r\n");
    wifi_uart_write_command_value(EMR_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_EMR_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_EMR_GET)
    {
        http_get_flush_value("getEMR",6,device_status.config.emr);
    }
    else
    {
        http_get_flush_value("getEMR",6,0);
    }
    return ret;
}
void http_emr_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t events;
    now_stream = stream;
    char* value_buf = malloc(32);
    uint32_t size = extract_value_from_path(url_path, value_buf);
    uint32_t value = atoi(value_buf);
    free(value_buf);
    LOG_D("http_emr_set_callback value %ld,size %ld\r\n",value,size);
    if (value < 30 || value > 300)
    {
        http_set_flush_value(ERRDATA_CODE, "setEMR", 6, value);
        return;
    }
    wifi_uart_write_command_value(EMR_SET_CMD,value);
    wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_EMR_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_EMR_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setEMR",6,device_status.config.emr);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setEMR",6,device_status.config.emr);
    }
}

int32_t http_rcp_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_rcp_get_callback GET\r\n");
    wifi_uart_write_command_value(RCP_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_RCP_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_RCP_GET)
    {
        http_get_flush_value("getRCP",6,device_status.config.rcp);
    }
    else
    {
        http_get_flush_value("getRCP",6,0);
    }
    return ret;
}
void http_rcp_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t events;
    now_stream = stream;
    char* value_buf = malloc(32);
    uint32_t size = extract_value_from_path(url_path, value_buf);
    uint32_t value = atoi(value_buf);
    free(value_buf);
    LOG_D("http_rcp_set_callback value %ld,size %ld\r\n",value,size);
    if (value < 1 || value > 168)
    {
        http_set_flush_value(ERRDATA_CODE, "setRCP", 6, value);
        return;
    }
    wifi_uart_write_command_value(RCP_SET_CMD,value);
    wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_RCP_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_RCP_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setRCP",6,device_status.config.rcp);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setRCP",6,device_status.config.rcp);
    }
}

int32_t http_wti_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_wti_get_callback GET\r\n");
    wifi_uart_write_command_value(WTI_GET_CMD,0);
    ret = wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_WTI_GET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_WTI_GET)
    {
        http_get_flush_value("getWTI",6,device_status.config.wti);
    }
    else
    {
        http_get_flush_value("getWTI",6,0);
    }
    return ret;
}
void http_wti_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t events;
    now_stream = stream;
    char* value_buf = malloc(32);
    uint32_t size = extract_value_from_path(url_path, value_buf);
    uint32_t value = atoi(value_buf);
    free(value_buf);
    LOG_D("http_wti_set_callback value %ld,size %ld\r\n",value,size);
    if (value < 10 || value > 1800)
    {
        http_set_flush_value(ERRDATA_CODE, "setWTI", 6, value);
        return;
    }
    wifi_uart_write_command_value(WTI_SET_CMD,value);
    wiced_rtos_wait_for_event_flags(&Config_EventHandler,EVENT_CONFIG_WTI_SET, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,100);
    if(events & EVENT_CONFIG_WTI_SET)
    {
        http_set_flush_value(SUCCESS_CODE,"setWTI",6,device_status.config.wti);
    }
    else
    {
        http_set_flush_value(TIMEOUT_CODE,"setWTI",6,device_status.config.wti);
    }
}

int32_t http_wfr_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_wfr_get_callback GET\r\n");
    http_get_flush_value("getWFR",6,sta_rssi_get());
    return ret;
}

int32_t http_rtc_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    now_stream = stream;
    LOG_D("http_rtc_get_callback GET\r\n");
    http_get_flush_value("getRTC",6,get_time());
    return ret;
}

void http_rtc_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    uint32_t ret,events;
    char* value_buf = malloc(32);
    uint32_t size = extract_value_from_path(url_path, value_buf);
    uint64_t value = atol(value_buf);
    free(value_buf);
    now_stream = stream;
    LOG_D("http_rtc_set_callback,value %ld\r\n",value);
    value *= 1000;
    wiced_time_set_utc_time_ms(&value);
    http_get_flush_value("setRTC",6,get_time());
}
