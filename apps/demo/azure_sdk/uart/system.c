#include "wiced.h"
#include "uart_core.h"
#include "twin_upload.h"
#include "twin_parse.h"
#include "system.h"
#include "cJSON.h"

extern syr_status device_status;
extern wiced_event_flags_t Config_EventHandler;
extern wiced_event_flags_t Info_EventHandler;
extern wiced_event_flags_t TEM_EventHandler;
extern wiced_event_flags_t C2D_EventHandler;

void dp_download_handle(unsigned char dpid,const unsigned char value[], unsigned short length)
{
    /****************************************************************
            当前函数处理可下发/可上报数据调用,具体函数内需要实现下发数据处理
            完成用需要将处理结果反馈至APP端,否则APP会认为下发失败
            请在该函数根据子设备的id自行实现子设备的dpid处理
    ****************************************************************/
    unsigned long update_value;
    switch(dpid) {
        case RSE_GET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.rse = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_RSE_GET);
            break;
        case RSE_SET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.rse = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_RSE_SET);
            break;
        case RSA_GET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.rsa = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_RSA_GET);
            break;
        case RSA_SET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.rsa = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_RSA_SET);
            break;
        case RSI_GET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.rsi = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_RSI_GET);
            break;
        case RSI_SET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.rsi = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_RSI_SET);
            break;
        case RSD_GET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.rsd = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_RSD_GET);
            break;
        case RSD_SET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.rsd = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_RSD_SET);
            break;
        case CNF_GET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.cnf = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_CNF_GET);
            break;
        case CNF_SET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.cnf = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_CNF_SET);
            break;
        case CNL_GET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.cnl = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_CNL_GET);
            break;
        case CNL_SET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.cnl = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_CNL_SET);
            break;
        case SSE_GET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.sse = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_SSE_GET);
            break;
        case SSE_SET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.sse = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_SSE_SET);
            break;
        case SSA_GET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.ssa = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_SSA_GET);
            break;
        case SSA_SET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.ssa = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_SSA_SET);
            break;
        case SSD_GET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.ssd = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_SSD_GET);
            break;
        case SSD_SET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.ssd = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_SSD_SET);
            break;
        case LNG_GET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.lng = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_LNG_GET);
            break;
        case LNG_SET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.lng = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_LNG_SET);
            break;
        case COM_GET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.info.com = update_value;
            wiced_rtos_set_event_flags(&Info_EventHandler,EVENT_INFO_COM_GET);
            break;
        case COM_SET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.info.com = update_value;
            wiced_rtos_set_event_flags(&Info_EventHandler,EVENT_INFO_COM_SET);
            break;
        case COA_GET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.info.coa = update_value;
            wiced_rtos_set_event_flags(&Info_EventHandler,EVENT_INFO_COA_GET);
            break;
        case COA_SET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.info.coa = update_value;
            wiced_rtos_set_event_flags(&Info_EventHandler,EVENT_INFO_COA_SET);
            break;
        case COD_GET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.info.cod = update_value;
            wiced_rtos_set_event_flags(&Info_EventHandler,EVENT_INFO_COD_GET);
            break;
        case COD_SET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.info.cod = update_value;
            wiced_rtos_set_event_flags(&Info_EventHandler,EVENT_INFO_COD_SET);
            break;
        case COE_GET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.info.coe = update_value;
            wiced_rtos_set_event_flags(&Info_EventHandler,EVENT_INFO_COE_GET);
            break;
        case COE_SET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.info.coe = update_value;
            wiced_rtos_set_event_flags(&Info_EventHandler,EVENT_INFO_COE_SET);
            break;
        case CND_GET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.info.cnd = update_value;
            wiced_rtos_set_event_flags(&Info_EventHandler,EVENT_INFO_CND_GET);
            break;
        case SUP_GET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.info.sup = update_value;
            wiced_rtos_set_event_flags(&Info_EventHandler,EVENT_INFO_SUP_GET);
            break;
        case VLV_GET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.info.vlv = update_value;
            wiced_rtos_set_event_flags(&Info_EventHandler,EVENT_INFO_VLV_GET);
            break;
        case ALM_GET_CMD:
            strncpy(device_status.info.alm_array,value,length);
            wiced_rtos_set_event_flags(&Info_EventHandler,EVENT_INFO_ALM_GET);
            break;
        case ALW_GET_CMD:
            strncpy(device_status.info.alw_array,value,length);
            wiced_rtos_set_event_flags(&Info_EventHandler,EVENT_INFO_ALW_GET);
            break;
        case ALN_GET_CMD:
            strncpy(device_status.info.aln_array,value,length);
            wiced_rtos_set_event_flags(&Info_EventHandler,EVENT_INFO_ALN_GET);
            break;
        case NET_GET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.tem.net = update_value;
            wiced_rtos_set_event_flags(&TEM_EventHandler,EVENT_TEM_NET_GET);
            break;
        case BAT_GET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.tem.bat = update_value;
            wiced_rtos_set_event_flags(&TEM_EventHandler,EVENT_TEM_BAT_GET);
            break;
        case RAS_SET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.c2d.ras = update_value;
            wiced_rtos_set_event_flags(&C2D_EventHandler,EVENT_C2D_RAS_SET);
            break;
        case EMR_SET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.emr = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_EMR_SET);
            break;
        case EMR_GET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.emr = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_EMR_GET);
            break;
        case RCP_SET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.rcp = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_RCP_SET);
            break;
        case RCP_GET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.rcp = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_RCP_GET);
            break;
        case WTI_SET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.wti = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_WTI_SET);
            break;
        case WTI_GET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.wti = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_WTI_GET);
            break;
        case APT_SET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.apt = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_APT_SET);
            break;
        case APT_GET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.apt = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_APT_GET);
            break;
        case WAD_SET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.wad = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_WAD_SET);
            break;
        case WAD_GET_CMD:
            update_value = mcu_get_dp_download_value(value,length);
            device_status.config.wad = update_value;
            wiced_rtos_set_event_flags(&Config_EventHandler,EVENT_CONFIG_WAD_GET);
            break;
        default:
            break;
        }
}
void product_info_request(void)
{
    wifi_uart_write_frame(PRODUCT_INFO_CMD, MCU_TX_VER, 0);
}

void factory_set_request(void)
{
    set_factory();
}

void wfc_start_request(void)
{
    wifi_uart_write_frame(WFC_CONTROL_CMD, MCU_TX_VER, 0);
}

void product_info_parse(unsigned char* data_buf,unsigned short data_len)
{
    cJSON *root = NULL,*item = NULL;

    unsigned char pstr[data_len + 1];
    my_memset(pstr, 0x00, data_len+1);
    my_memcpy(pstr, data_buf, data_len);

    root = cJSON_Parse(pstr);
    if(NULL == root){
        goto PARSE_ERR;
    }

    item = cJSON_GetObjectItem(root, "ver");
    if(NULL == item){
        goto PARSE_ERR;
    }
    strncpy(device_status.info.ver,item->valuestring,strlen(item->valuestring));

    item = cJSON_GetObjectItem(root, "ap");
    if(NULL == item){
        goto PARSE_ERR;
    }
    wifi_ap_control(item->valueint);

    cJSON_Delete(root);
    return;
PARSE_ERR:
    if(NULL != root) {
        cJSON_Delete(root);
    }
}
void ota_control_send(uint16_t value)
{
    uint32_t send_len = 0;
    send_len = set_wifi_uart_byte(send_len,(value&0xFF00)>>8);
    send_len = set_wifi_uart_byte(send_len,value);
    wifi_uart_write_frame(UPDATE_CONTROL_CMD, MCU_TX_VER, send_len);
}
void ota_control_parse(unsigned char data)
{
    ota_event_send(data);
}
void wifi_ap_control(unsigned char data)
{
    http_control(data);
}
