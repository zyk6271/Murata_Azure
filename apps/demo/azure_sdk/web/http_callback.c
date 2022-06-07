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
int32_t http_rst_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_rst_callback GET\r\n");
        http_flush_value(ERRREQS_CODE,"getRST",0);
        break;
    case WICED_HTTP_POST_REQUEST:
        printf("http_rst_callback POST\r\n");
        wifi_uart_write_command_value(RST_SET_CMD,1);
        EventValue = xEventGroupWaitBits(C2D_EventHandler,EVENT_C2D_RST_SET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_C2D_RST_SET)
        {
            http_flush_value(SUCCESS_CODE,"setRST",1);
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"setRST",1);
        }
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errRST",0);
        break;
    }
    return 0;
}
int32_t http_def_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_def_callback GET\r\n");
        http_flush_value(ERRREQS_CODE,"getDEF",0);
        break;
    case WICED_HTTP_POST_REQUEST:
        printf("http_def_callback POST\r\n");
        wifi_uart_write_command_value(DEF_SET_CMD,1);
        EventValue = xEventGroupWaitBits(C2D_EventHandler,EVENT_C2D_DEF_SET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_C2D_DEF_SET)
        {
            http_flush_value(SUCCESS_CODE,"setDEF",1);
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"setDEF",1);
        }
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errDEF",0);
        break;
    }
    return 0;
}
int32_t http_ras_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_ras_callback GET\r\n");
        http_flush_value(ERRREQS_CODE,"getRAS",0);
        break;
    case WICED_HTTP_POST_REQUEST:
        printf("http_ras_callback POST\r\n");
        wifi_uart_write_command_value(RAS_SET_CMD,1);
        EventValue = xEventGroupWaitBits(C2D_EventHandler,EVENT_C2D_RAS_SET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_C2D_RAS_SET)
        {
            http_flush_value(SUCCESS_CODE,"setRAS",1);
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"setRAS",1);
        }
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errRAS",0);
        break;
    }
    return 0;
}
int32_t http_cnd_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_cnd_callback GET\r\n");
        wifi_uart_write_frame(CND_GET_CMD,MCU_TX_VER,0);
        EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_CND_GET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_INFO_CND_GET)
        {
            http_flush_value(SUCCESS_CODE,"getCND",device_status.info.cnd);
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"getCND",0);
        }
        break;
    case WICED_HTTP_POST_REQUEST:
        printf("http_cnd_callback POST\r\n");
        http_flush_value(ERRREQS_CODE,"setCND",0);
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errCND",0);
        break;
    }
    return 0;
}
int32_t http_net_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_net_callback GET\r\n");
        wifi_uart_write_frame(NET_GET_CMD,MCU_TX_VER,0);
        EventValue = xEventGroupWaitBits(TEM_EventHandler,EVENT_TEM_NET_GET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_TEM_NET_GET)
        {
            http_flush_value(SUCCESS_CODE,"getNET",device_status.tem.net);
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"getNET",0);
        }
        break;
    case WICED_HTTP_POST_REQUEST:
        printf("http_net_callback POST\r\n");
        http_flush_value(ERRREQS_CODE,"setNET",0);
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errNET",0);
        break;
    }
    return 0;
}
int32_t http_bat_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_bat_callback GET\r\n");
        wifi_uart_write_frame(BAT_GET_CMD,MCU_TX_VER,0);
        EventValue = xEventGroupWaitBits(TEM_EventHandler,EVENT_TEM_BAT_GET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_TEM_BAT_GET)
        {
            http_flush_value(SUCCESS_CODE,"getBAT",device_status.tem.bat);
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"getBAT",0);
        }
        break;
    case WICED_HTTP_POST_REQUEST:
        printf("http_bat_callback POST\r\n");
        http_flush_value(ERRREQS_CODE,"setBAT",0);
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errBAT",0);
        break;
    }
    return 0;
}
int32_t http_ala_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_ala_callback GET\r\n");
        wifi_uart_write_frame(ALA_GET_CMD,MCU_TX_VER,0);
        EventValue = xEventGroupWaitBits(TEM_EventHandler,EVENT_TEM_ALA_GET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_TEM_ALA_GET)
        {
            http_flush_value(SUCCESS_CODE,"getALA",device_status.tem.ala);
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"getALA",0);
        }
        break;
    case WICED_HTTP_POST_REQUEST:
        printf("http_ala_callback POST\r\n");
        http_flush_value(ERRREQS_CODE,"setALA",0);
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errALA",0);
        break;
    }
    return 0;
}
int32_t http_alr_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_alr_callback GET\r\n");
        wifi_uart_write_frame(ALR_GET_CMD,MCU_TX_VER,0);
        EventValue = xEventGroupWaitBits(TEM_EventHandler,EVENT_TEM_ALR_GET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_TEM_ALR_GET)
        {
            http_flush_value(SUCCESS_CODE,"getALR",device_status.tem.alr);
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"getALR",0);
        }
        break;
    case WICED_HTTP_POST_REQUEST:
        printf("http_alr_callback POST\r\n");
        http_flush_value(ERRREQS_CODE,"setALR",0);
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errALR",0);
        break;
    }
    return 0;
}
int32_t http_sup_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_sup_callback GET\r\n");
        wifi_uart_write_frame(SUP_GET_CMD,MCU_TX_VER,0);
        EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_SUP_GET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_INFO_SUP_GET)
        {
            http_flush_value(SUCCESS_CODE,"getSUP",device_status.info.sup);
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"getSUP",0);
        }
        break;
    case WICED_HTTP_POST_REQUEST:
        printf("http_sup_callback POST\r\n");
        http_flush_value(ERRREQS_CODE,"setSUP",0);
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errSUP",0);
        break;
    }
    return 0;
}
int32_t http_rse_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    uint32_t target_value = 0;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_rse_callback GET\r\n");
        wifi_uart_write_frame(RSE_GET_CMD,MCU_TX_VER,0);
        EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_RSE_GET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_CONFIG_RSE_GET)
        {
            http_flush_value(SUCCESS_CODE,"getRSE",device_status.config.rse);
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"getRSE",0);
        }
        break;
    case WICED_HTTP_POST_REQUEST:
        target_value = verification_value("value",http_message_body->data,http_message_body->message_data_length);
        printf("http_rse_callback POST\r\n");
        wifi_uart_write_command_value(RSE_SET_CMD,target_value);
        EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_RSE_SET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_CONFIG_RSE_SET)
        {
            if(device_status.config.rse == target_value)
            {
                http_flush_value(SUCCESS_CODE,"setRSE",device_status.config.rse);
            }
            else
            {
                http_flush_value(ERRDATA_CODE,"setRSE",0);
            }
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"setRSE",0);
        }
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errRSE",0);
        break;
    }
    return 0;
}
int32_t http_rsa_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    uint32_t target_value = 0;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_rsa_callback GET\r\n");
        wifi_uart_write_frame(RSA_GET_CMD,MCU_TX_VER,0);
        EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_RSA_GET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_CONFIG_RSA_GET)
        {
            http_flush_value(SUCCESS_CODE,"getRSA",device_status.config.rsa);
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"getRSA",0);
        }
        break;
    case WICED_HTTP_POST_REQUEST:
        target_value = verification_value("value",http_message_body->data,http_message_body->message_data_length);
        printf("http_rsa_callback POST\r\n");
        wifi_uart_write_command_value(RSA_SET_CMD,target_value);
        EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_RSA_SET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_CONFIG_RSA_SET)
        {
            if(device_status.config.rsa == target_value)
            {
                http_flush_value(SUCCESS_CODE,"setRSA",device_status.config.rsa);
            }
            else
            {
                http_flush_value(ERRDATA_CODE,"setRSA",0);
            }
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"setRSA",0);
        }
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errRSA",0);
        break;
    }
    return 0;
}
int32_t http_rsi_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    uint32_t target_value = 0;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_rsi_callback GET\r\n");
        wifi_uart_write_frame(RSI_GET_CMD,MCU_TX_VER,0);
        EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_RSI_GET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_CONFIG_RSI_GET)
        {
            http_flush_value(SUCCESS_CODE,"getRSI",device_status.config.rsi);
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"getRSI",0);
        }
        break;
    case WICED_HTTP_POST_REQUEST:
        target_value = verification_value("value",http_message_body->data,http_message_body->message_data_length);
        printf("http_rsi_callback POST\r\n");
        wifi_uart_write_command_value(RSI_SET_CMD,target_value);
        EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_RSI_SET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_CONFIG_RSI_SET)
        {
            if(device_status.config.rsi == target_value)
            {
                http_flush_value(SUCCESS_CODE,"setRSI",device_status.config.rsi);
            }
            else
            {
                http_flush_value(ERRDATA_CODE,"setRSI",0);
            }
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"setRSI",0);
        }
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errRSI",0);
        break;
    }
    return 0;
}
int32_t http_rsd_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    uint32_t target_value = 0;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_rsd_callback GET\r\n");
        wifi_uart_write_frame(RSD_GET_CMD,MCU_TX_VER,0);
        EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_RSD_GET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_CONFIG_RSD_GET)
        {
            http_flush_value(SUCCESS_CODE,"getRSD",device_status.config.rsd);
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"getRSD",0);
        }
        break;
    case WICED_HTTP_POST_REQUEST:
        target_value = verification_value("value",http_message_body->data,http_message_body->message_data_length);
        printf("http_rsd_callback POST\r\n");
        wifi_uart_write_command_value(RSD_SET_CMD,target_value);
        EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_RSD_SET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_CONFIG_RSD_SET)
        {
            if(device_status.config.rsd == target_value)
            {
                http_flush_value(SUCCESS_CODE,"setRSD",device_status.config.rsd);
            }
            else
            {
                http_flush_value(ERRDATA_CODE,"setRSD",0);
            }
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"setRSD",0);
        }
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errRSD",0);
        break;
    }
    return 0;
}
int32_t http_cnf_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    uint32_t target_value = 0;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_cnf_callback GET\r\n");
        wifi_uart_write_frame(CNF_GET_CMD,MCU_TX_VER,0);
        EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_CNF_GET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_CONFIG_CNF_GET)
        {
            http_flush_value(SUCCESS_CODE,"getCNF",device_status.config.cnf);
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"getCNF",0);
        }
        break;
    case WICED_HTTP_POST_REQUEST:
        target_value = verification_value("value",http_message_body->data,http_message_body->message_data_length);
        printf("http_cnf_callback POST\r\n");
        wifi_uart_write_command_value(CNF_SET_CMD,target_value);
        EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_CNF_SET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_CONFIG_CNF_SET)
        {
            if(device_status.config.cnf == target_value)
            {
                http_flush_value(SUCCESS_CODE,"setCNF",device_status.config.cnf);
            }
            else
            {
                http_flush_value(ERRDATA_CODE,"setCNF",0);
            }
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"setCNF",0);
        }
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errCNF",0);
        break;
    }
    return 0;
}
int32_t http_cnl_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    uint32_t target_value = 0;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_cnl_callback GET\r\n");
        wifi_uart_write_frame(CNL_GET_CMD,MCU_TX_VER,0);
        EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_CNL_GET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_CONFIG_CNL_GET)
        {
            http_flush_value(SUCCESS_CODE,"getCNL",device_status.config.cnl);
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"getCNL",0);
        }
        break;
    case WICED_HTTP_POST_REQUEST:
        target_value = verification_value("value",http_message_body->data,http_message_body->message_data_length);
        printf("http_cnl_callback POST\r\n");
        wifi_uart_write_command_value(CNL_SET_CMD,target_value);
        EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_CNL_SET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_CONFIG_CNL_SET)
        {
            if(device_status.config.cnl == target_value)
            {
                http_flush_value(SUCCESS_CODE,"setCNL",device_status.config.cnl);
            }
            else
            {
                http_flush_value(ERRDATA_CODE,"setCNL",0);
            }
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"setCNL",0);
        }
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errCNL",0);
        break;
    }
    return 0;
}
int32_t http_sse_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    uint32_t target_value = 0;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_sse_callback GET\r\n");
        wifi_uart_write_frame(SSE_GET_CMD,MCU_TX_VER,0);
        EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_SSE_GET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_CONFIG_SSE_GET)
        {
            http_flush_value(SUCCESS_CODE,"getSSE",device_status.config.sse);
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"getSSE",0);
        }
        break;
    case WICED_HTTP_POST_REQUEST:
        target_value = verification_value("value",http_message_body->data,http_message_body->message_data_length);
        printf("http_sse_callback POST\r\n");
        wifi_uart_write_command_value(SSE_SET_CMD,target_value);
        EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_SSE_SET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_CONFIG_SSE_SET)
        {
            if(device_status.config.sse == target_value)
            {
                http_flush_value(SUCCESS_CODE,"setSSE",device_status.config.sse);
            }
            else
            {
                http_flush_value(ERRDATA_CODE,"setSSE",0);
            }
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"setSSE",0);
        }
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errSSE",0);
        break;
    }
    return 0;
}
int32_t http_ssa_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    uint32_t target_value = 0;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_ssa_callback GET\r\n");
        wifi_uart_write_frame(SSA_GET_CMD,MCU_TX_VER,0);
        EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_SSA_GET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_CONFIG_SSA_GET)
        {
            http_flush_value(SUCCESS_CODE,"getSSA",device_status.config.ssa);
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"getSSA",0);
        }
        break;
    case WICED_HTTP_POST_REQUEST:
        target_value = verification_value("value",http_message_body->data,http_message_body->message_data_length);
        printf("http_ssa_callback POST\r\n");
        wifi_uart_write_command_value(SSA_SET_CMD,target_value);
        EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_SSA_SET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_CONFIG_SSA_SET)
        {
            if(device_status.config.ssa == target_value)
            {
                http_flush_value(SUCCESS_CODE,"setSSA",device_status.config.ssa);
            }
            else
            {
                http_flush_value(ERRDATA_CODE,"setSSA",0);
            }
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"setSSA",0);
        }
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errSSA",0);
        break;
    }
    return 0;
}
int32_t http_ssd_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    uint32_t target_value = 0;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_ssd_callback GET\r\n");
        wifi_uart_write_frame(SSD_GET_CMD,MCU_TX_VER,0);
        EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_SSD_GET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_CONFIG_SSD_GET)
        {
            http_flush_value(SUCCESS_CODE,"getSSD",device_status.config.ssd);
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"getSSD",0);
        }
        break;
    case WICED_HTTP_POST_REQUEST:
        target_value = verification_value("value",http_message_body->data,http_message_body->message_data_length);
        printf("http_ssd_callback POST\r\n");
        wifi_uart_write_command_value(SSD_SET_CMD,target_value);
        EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_SSD_SET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_CONFIG_SSD_SET)
        {
            if(device_status.config.ssd == target_value)
            {
                http_flush_value(SUCCESS_CODE,"setSSD",device_status.config.ssd);
            }
            else
            {
                http_flush_value(ERRDATA_CODE,"setSSD",0);
            }
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"setSSD",0);
        }
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errSSD",0);
        break;
    }
    return 0;
}
int32_t http_lng_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    uint32_t target_value = 0;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_lng_callback GET\r\n");
        wifi_uart_write_frame(LNG_GET_CMD,MCU_TX_VER,0);
        EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_LNG_GET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_CONFIG_LNG_GET)
        {
            http_flush_value(SUCCESS_CODE,"getLNG",device_status.config.lng);
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"getLNG",0);
        }
        break;
    case WICED_HTTP_POST_REQUEST:
        target_value = verification_value("value",http_message_body->data,http_message_body->message_data_length);
        printf("http_lng_callback POST\r\n");
        wifi_uart_write_command_value(LNG_SET_CMD,target_value);
        EventValue = xEventGroupWaitBits(Config_EventHandler,EVENT_CONFIG_LNG_SET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_CONFIG_LNG_SET)
        {
            if(device_status.config.lng == target_value)
            {
                http_flush_value(SUCCESS_CODE,"setLNG",device_status.config.lng);
            }
            else
            {
                http_flush_value(ERRDATA_CODE,"setLNG",0);
            }
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"setLNG",0);
        }
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errLNG",0);
        break;
    }
    return 0;
}
int32_t http_ver_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_ver_callback GET\r\n");
        wifi_uart_write_frame(VER_GET_CMD,MCU_TX_VER,0);
        EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_VER_GET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_INFO_VER_GET)
        {
            http_flush_value(SUCCESS_CODE,"getVER",device_status.info.ver);
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"getVER",0);
        }
        break;
    case WICED_HTTP_POST_REQUEST:
        printf("http_ver_callback POST\r\n");
        http_flush_value(ERRREQS_CODE,"setVER",0);
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errVER",0);
        break;
    }
    return 0;
}
int32_t http_srn_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_srn_callback GET\r\n");
        wifi_uart_write_frame(SRN_GET_CMD,MCU_TX_VER,0);
        EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_SRN_GET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_INFO_SRN_GET)
        {
            http_flush_value(SUCCESS_CODE,"getSRN",device_status.info.srn);
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"getSRN",0);
        }
        break;
    case WICED_HTTP_POST_REQUEST:
        printf("http_srn_callback POST\r\n");
        http_flush_value(ERRREQS_CODE,"setSRN",0);
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errSRN",0);
        break;
    }
    return 0;
}
int32_t http_com_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    uint32_t target_value = 0;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_com_callback GET\r\n");
        wifi_uart_write_frame(COM_GET_CMD,MCU_TX_VER,0);
        EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_COM_GET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_INFO_COM_GET)
        {
            http_flush_value(SUCCESS_CODE,"getCOM",device_status.info.com);
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"getCOM",0);
        }
        break;
    case WICED_HTTP_POST_REQUEST:
        target_value = verification_value("value",http_message_body->data,http_message_body->message_data_length);
        printf("http_com_callback POST\r\n");
        wifi_uart_write_command_value(COM_SET_CMD,target_value);
        EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_COM_SET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_INFO_COM_SET)
        {
            if(device_status.info.com == target_value)
            {
                http_flush_value(SUCCESS_CODE,"setCOM",device_status.info.com);
            }
            else
            {
                http_flush_value(ERRDATA_CODE,"setCOM",0);
            }
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"setCOM",0);
        }
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errCOM",0);
        break;
    }
    return 0;
}
int32_t http_coa_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    uint32_t target_value = 0;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_coa_callback GET\r\n");
        wifi_uart_write_frame(COA_GET_CMD,MCU_TX_VER,0);
        EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_COA_GET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_INFO_COA_GET)
        {
            http_flush_value(SUCCESS_CODE,"getCOA",device_status.info.coa);
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"getCOA",0);
        }
        break;
    case WICED_HTTP_POST_REQUEST:
        target_value = verification_value("value",http_message_body->data,http_message_body->message_data_length);
        printf("http_coa_callback POST\r\n");
        wifi_uart_write_command_value(COA_SET_CMD,target_value);
        EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_COA_SET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_INFO_COA_SET)
        {
            if(device_status.info.coa == target_value)
            {
                http_flush_value(SUCCESS_CODE,"setCOA",device_status.info.coa);
            }
            else
            {
                http_flush_value(ERRDATA_CODE,"setCOA",0);
            }
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"setCOA",0);
        }
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errCOA",0);
        break;
    }
    return 0;
}
int32_t http_cod_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    uint32_t target_value = 0;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_cod_callback GET\r\n");
        wifi_uart_write_frame(COD_GET_CMD,MCU_TX_VER,0);
        EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_COD_GET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_INFO_COD_GET)
        {
            http_flush_value(SUCCESS_CODE,"getCOD",device_status.info.cod);
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"getCOD",0);
        }
        break;
    case WICED_HTTP_POST_REQUEST:
        target_value = verification_value("value",http_message_body->data,http_message_body->message_data_length);
        printf("http_cod_callback POST\r\n");
        wifi_uart_write_command_value(COD_SET_CMD,target_value);
        EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_COD_SET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_INFO_COD_SET)
        {
            if(device_status.info.cod == target_value)
            {
                http_flush_value(SUCCESS_CODE,"setCOD",device_status.info.cod);
            }
            else
            {
                http_flush_value(ERRDATA_CODE,"setCOD",0);
            }
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"setCOD",0);
        }
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errCOD",0);
        break;
    }
    return 0;
}
int32_t http_coe_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
{
    EventBits_t EventValue;
    uint32_t target_value = 0;
    now_stream = stream;
    switch(http_message_body->request_type)
    {
    case WICED_HTTP_GET_REQUEST:
        printf("http_coe_callback GET\r\n");
        wifi_uart_write_frame(COE_GET_CMD,MCU_TX_VER,0);
        EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_COE_GET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_INFO_COE_GET)
        {
            http_flush_value(SUCCESS_CODE,"getCOE",device_status.info.coe);
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"getCOE",0);
        }
        break;
    case WICED_HTTP_POST_REQUEST:
        target_value = verification_value("value",http_message_body->data,http_message_body->message_data_length);
        printf("http_coe_callback POST\r\n");
        wifi_uart_write_command_value(COE_SET_CMD,target_value);
        EventValue = xEventGroupWaitBits(Info_EventHandler,EVENT_INFO_COE_SET,pdTRUE,pdTRUE,100);
        if(EventValue & EVENT_INFO_COE_SET)
        {
            if(device_status.info.coe == target_value)
            {
                http_flush_value(SUCCESS_CODE,"setCOE",device_status.info.coe);
            }
            else
            {
                http_flush_value(ERRDATA_CODE,"setCOE",0);
            }
        }
        else
        {
            http_flush_value(TIMEOUT_CODE,"setCOE",0);
        }
        break;
    default:
        http_flush_value(ERRREQS_CODE,"errCOE",0);
        break;
    }
    return 0;
}
int32_t http_wfc_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
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

int32_t http_wfs_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
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

int32_t http_wfk_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
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
int32_t http_wsr_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body)
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
