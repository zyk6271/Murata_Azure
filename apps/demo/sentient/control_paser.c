#include "cJSON.h"
#include "wiced.h"
#include "mqtt_api.h"
#include "resources.h"
#include "sentient.h"
#include "wifi_paser.h"
#include "udp_transmit.h"
#include "mqttwork.h"
#include "control_paser.h"
#include "uart.h"
void heart_report(void)
{
    char *cjson_str = NULL;
    cJSON * root =   cJSON_CreateObject();
    cJSON_AddItemToObject(root, "heart", cJSON_CreateString("pong"));
    cjson_str = cJSON_PrintUnformatted(root);
    printf("wifi_encoder result is %s\r\n",cjson_str);
    mqtt_send(strdup(cjson_str));
    //printf(cjson_str);
    cJSON_Delete(root);
    free(cjson_str);
}
void control_encode(uint8_t control,uint32_t value)
{
    char *cjson_str = NULL;
    cJSON * root =   cJSON_CreateObject();
    cJSON_AddItemToObject(root, "reponse", cJSON_CreateNumber(control));
    cJSON_AddItemToObject(root, "value", cJSON_CreateNumber(value));
    cjson_str = cJSON_PrintUnformatted(root);
    mqtt_send(strdup(cjson_str));
    cJSON_Delete(root);
    free(cjson_str);
}
void control_decoder(char * pMsg)
{
    cJSON * pJson = NULL;
    cJSON * pSub =NULL;
    char *heart;
    char control=0;
    char value=0;

    if (NULL == pMsg)
    {
        return;
    }
    pJson = cJSON_Parse(pMsg);
    if (NULL == pJson)
    {
        return;
    }
    pSub = cJSON_GetObjectItem(pJson, "heart");
    if (NULL != pSub)
    {
        heart=pSub->valuestring;
        printf("heart is %s\r\n",heart);
        heart_report();
        cJSON_Delete(pJson);
        return;
    }
    pSub = cJSON_GetObjectItem(pJson, "control");
    if (NULL != pSub)
    {
        control = pSub->valueint;
        printf("control is %d\r\n",control);
        pSub = cJSON_GetObjectItem(pJson, "value");
        if (NULL != pSub)
        {
            value = pSub->valueint;
            printf("value is %d\r\n",value);
            uart_send(control,value);
        }
    }
    cJSON_Delete(pJson);
}
