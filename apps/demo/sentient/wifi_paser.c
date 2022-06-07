#include "cJSON.h"
#include "wiced.h"
#include "mqtt_api.h"
#include "resources.h"
#include "sentient.h"
#include "wifi_paser.h"
#include "udp_transmit.h"
#include "mqttwork.h"
char *ssid = 0;
char security_id = 0;
char *password = 0;
void set_config(char *ssid,char security,char *password)
{
    uint8_t ret = WICED_SUCCESS;
    struct
    {
        wiced_bool_t             device_configured;
        wiced_config_ap_entry_t  ap_entry;
    } temp_config;
    wiced_ssid_t temp_ssid;
    if(ssid == NULL)
    {
        ret = WICED_ERROR;
    }
    if(security_id >0 && strlen(password)<8)
    {
        ret = WICED_ERROR;
    }
    memcpy(&temp_ssid.value,ssid, strlen(ssid));
    temp_ssid.length = strlen(ssid);
    memset(&temp_config, 0, strlen(&temp_config));
    memcpy(&temp_config.ap_entry.details.SSID, &temp_ssid, strlen(&temp_ssid));
    switch(security_id)
    {
    case 0:temp_config.ap_entry.details.security = WICED_SECURITY_OPEN;
        break;
    case 1:temp_config.ap_entry.details.security = WICED_SECURITY_WEP_SHARED;
        memcpy(temp_config.ap_entry.security_key,password, strlen(password));
        temp_config.ap_entry.security_key_length = strlen(password);
        break;
    case 2:temp_config.ap_entry.details.security = WICED_SECURITY_WPA2_MIXED_PSK;
        memcpy(temp_config.ap_entry.security_key,password, strlen(password));
        temp_config.ap_entry.security_key_length = strlen(password);
        break;
    }
    temp_config.device_configured = WICED_FALSE;
    wiced_dct_write( &temp_config, DCT_WIFI_CONFIG_SECTION, 0, sizeof(temp_config) );
    if(ret == WICED_SUCCESS)
    {
        printf("set is ok\r\n");
        wifi_encoder(0,1);
        wiced_rtos_delay_milliseconds( 1000 );
        platform_mcu_reset( );
    }
    else
    {
        printf("set is fail\r\n");
        wifi_encoder(0,0);
    }
}

void wifi_decoder(char * pMsg)
{
    cJSON * pJson = NULL;
    cJSON * pSub =NULL;

    if (NULL == pMsg)
    {
        return;
    }
    pJson = cJSON_Parse(pMsg);
    if (NULL == pJson)
    {
        return;
    }
    pSub = cJSON_GetObjectItem(pJson, "ssid");
    if (NULL != pSub)
    {
        ssid=pSub->valuestring;
    }
    pSub = cJSON_GetObjectItem(pJson, "security");
    if (NULL != pSub)
    {
        security_id = pSub->valueint;
    }
    pSub = cJSON_GetObjectItem(pJson, "password");
    if (NULL != pSub)
    {
        password=pSub->valuestring;
    }
    printf("wifi ssid is %s\r\n",ssid);
    printf("wifi security is %d\r\n",security_id);
    printf("wifi password is %s\r\n",password);
    set_config(ssid,security_id,password);
    cJSON_Delete(pJson);
}
void wifi_encoder(uint8_t func,uint8_t data)
{
    char *cjson_str = NULL;
    cJSON * root =   cJSON_CreateObject();
    switch(func)
    {
    case 0:
        cJSON_AddItemToObject(root, "result", cJSON_CreateNumber(data));
        break;
    }
    cjson_str = cJSON_PrintUnformatted(root);
    printf("wifi_encoder result is %s\r\n",cjson_str);
    cJSON_Delete(root);
    udp_packet_send(strdup(cjson_str),strlen(cjson_str));
    printf(cjson_str);
    free(cjson_str);
}
