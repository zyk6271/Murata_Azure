#include "wiced.h"
#include "http_server.h"
#include "resources.h"
#include "sntp.h"
#include "cJSON.h"
#include "http_callback.h"
#include "http_api.h"
#include "storage.h"

typedef struct {
    wiced_bool_t device_configured;
    wiced_config_ap_entry_t ap_entry;
} save_config;

wiced_timer_t rst_timer;
extern wiced_http_response_stream_t* now_stream;

char* flite_value(const char *source, uint32_t **size)
{
    char* dest = calloc((strlen(source) + 1), sizeof(char));
    char* tmp = strrchr(source, 0x2F);
    *size = (void*)strlen(tmp) - 1;
    strncpy(dest, (const char*)&tmp[1], (size_t)*size);
    if (dest == NULL) {
        return 0;
    } else {
        return dest;
    }
}
void http_get_flush_value(char *msg, uint32_t msg_size, uint32_t value)
{
    char *cjson_str = NULL;
    char *key_tmp = calloc(msg_size + 1, sizeof(char));
    strncpy(key_tmp, msg, msg_size);
    cJSON * root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, key_tmp, cJSON_CreateNumber(value));
    cjson_str = cJSON_PrintUnformatted(root);
    wiced_http_response_stream_enable_chunked_transfer(now_stream);
    wiced_http_response_stream_write(now_stream, (const void*) cjson_str,
            strlen(cjson_str));
    wiced_http_response_stream_flush(now_stream);
    wiced_http_response_stream_disable_chunked_transfer(now_stream);
    cJSON_Delete(root);
    free(key_tmp);
    free(cjson_str);
}
void http_get_flush_string(char *msg, uint32_t msg_size, char* value,uint32_t value_size)
{
    char *cjson_str = NULL;
    char *key_tmp = calloc(msg_size + 1, sizeof(char));
    strncpy(key_tmp, msg, msg_size);
    char *value_tmp = calloc(value_size + 1, sizeof(char));
    strncpy(value_tmp, value, value_size);
    cJSON * root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, key_tmp, cJSON_CreateString(value_tmp));
    cjson_str = cJSON_PrintUnformatted(root);
    wiced_http_response_stream_enable_chunked_transfer(now_stream);
    wiced_http_response_stream_write(now_stream, (const void*) cjson_str,
            strlen(cjson_str));
    wiced_http_response_stream_flush(now_stream);
    wiced_http_response_stream_disable_chunked_transfer(now_stream);
    cJSON_Delete(root);
    free(key_tmp);
    free(value_tmp);
    free(cjson_str);
}
void http_set_flush_value(char code, char *msg, uint32_t msg_size,uint32_t value)
{
    char *cjson_str = NULL;
    char *key_tmp = calloc(msg_size + 1, sizeof(char));
    strncpy(key_tmp, msg, msg_size);
    char *value_tmp = calloc(10, sizeof(char));
    itoa(value, value_tmp, 10);
    char *cat_tmp = calloc(msg_size + strlen(value_tmp) + 1, sizeof(char));
    strcpy(cat_tmp, key_tmp);
    strcat(cat_tmp, value_tmp);
    cJSON * root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, cat_tmp, cJSON_CreateString("OK"));
    cjson_str = cJSON_PrintUnformatted(root);
    wiced_http_response_stream_enable_chunked_transfer(now_stream);
    wiced_http_response_stream_write(now_stream, (const void*) cjson_str,
            strlen(cjson_str));
    wiced_http_response_stream_flush(now_stream);
    wiced_http_response_stream_disable_chunked_transfer(now_stream);
    cJSON_Delete(root);
    free(cat_tmp);
    free(key_tmp);
    free(value_tmp);
    free(cjson_str);
}
void http_set_flush_string(char code, char *msg, uint32_t msg_size,char * value, uint32_t value_size)
{
    char *cjson_str = NULL;
    char *key_tmp = calloc(msg_size + 1, sizeof(char));
    strncpy(key_tmp, msg, msg_size);
    char *value_tmp = calloc(value_size + 1, sizeof(char));
    strncpy(value_tmp, value, value_size);
    char *cat_tmp = calloc(msg_size + value_size + 1, sizeof(char));
    strcpy(cat_tmp, key_tmp);
    strcat(cat_tmp, value_tmp);
    cJSON * root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, cat_tmp, cJSON_CreateString("OK"));
    cjson_str = cJSON_PrintUnformatted(root);
    wiced_http_response_stream_enable_chunked_transfer(now_stream);
    wiced_http_response_stream_write(now_stream, (const void*) cjson_str,
            strlen(cjson_str));
    wiced_http_response_stream_flush(now_stream);
    wiced_http_response_stream_disable_chunked_transfer(now_stream);
    cJSON_Delete(root);
    free(cat_tmp);
    free(key_tmp);
    free(value_tmp);
    free(cjson_str);
}
void rst_timer_callback(void* arg)
{
    platform_mcu_reset();
}
void rst_work(void)
{
    wiced_rtos_init_timer(&rst_timer, 1000, rst_timer_callback, 0);
    wiced_rtos_start_timer(&rst_timer);
}
void set_config(wiced_ssid_t ssid, wiced_wpa_key_t password)
{
    uint8_t ret = WICED_SUCCESS;
    save_config temp_config = {0};
    if (ssid.length == 0) {
        return;
    }
    if (password.length == 0) {
        temp_config.ap_entry.details.security = WICED_SECURITY_OPEN;
    }
    else if(password.length > 0 && password.length < 8)
    {
        printf("password is too short\r\n");
        return;
    }
    else if(password.length >= 8 && password.length <= 64)
    {
        temp_config.ap_entry.details.security = WICED_SECURITY_WPA2_MIXED_PSK;
        strncpy((char*)temp_config.ap_entry.security_key, (const char*)password.data,password.length);
        temp_config.ap_entry.security_key_length = password.length;
    }
    strncpy((char*)temp_config.ap_entry.details.SSID.value, (const char*)ssid.value, ssid.length);
    temp_config.ap_entry.details.SSID.length = ssid.length;
    temp_config.device_configured = WICED_FALSE;
    ret = wiced_dct_write(&temp_config, DCT_WIFI_CONFIG_SECTION, 0,sizeof(temp_config));
    if (ret == WICED_SUCCESS)
    {
        printf("set is ok\r\n");
    }
    else
    {
        printf("set is fail,code is %d\r\n", ret);
    }
}
uint8_t set_factory(void)
{
    uint8_t ret = WICED_SUCCESS;
    save_config temp_config;
    temp_config.ap_entry.details.security = WICED_SECURITY_OPEN;
    temp_config.device_configured = WICED_TRUE;
    wiced_dct_write(&temp_config, DCT_WIFI_CONFIG_SECTION, 0,
            sizeof(temp_config));
    if (ret == WICED_SUCCESS) {
        printf("set_factory is ok\r\n");
        return 1;
    } else {
        printf("set_factory is fail,code is %d\r\n", ret);
        return 0;
    }
}
void azc_parse(const uint8_t*pMsg, uint32_t size)
{
    cJSON * root = NULL;
    cJSON * item = NULL;
    user_app_t *app_dct_write;
    app_dct_write = calloc(sizeof(user_app_t),sizeof(char));

    if (NULL == pMsg) {
        return;
    }
    root = cJSON_Parse((char*)pMsg);
    if (NULL == root) {
        return;
    }
    item = cJSON_GetObjectItem(root, "deviceID");
    if (NULL != item) {
        printf("deviceID is %s\r\n", item->valuestring);
        strncpy(app_dct_write->device_id, item->valuestring,
                strlen(item->valuestring));
    }
    item = cJSON_GetObjectItem(root, "primaryKey");
    if (NULL != item) {
        printf("primaryKey is %s\r\n", item->valuestring);
        strncpy(app_dct_write->primaryKey, item->valuestring,
                strlen(item->valuestring));
    }
    item = cJSON_GetObjectItem(root, "endpointAddress");
    if (NULL != item) {
        printf("endpointAddress is %s\r\n", item->valuestring);
        strncpy(app_dct_write->endpointAddress, item->valuestring,
                strlen(item->valuestring));
    }
    item = cJSON_GetObjectItem(root, "mac1");
    if (NULL != item) {
        printf("mac1 is %s\r\n", item->valuestring);
        strncpy(app_dct_write->mac1, item->valuestring,
                strlen(item->valuestring));
    }
    dct_app_azc_write(app_dct_write);
    free(app_dct_write);
    cJSON_Delete(root);
    rst_work();
}
void azc_flush(void)
{
    user_app_t *app_t = calloc(sizeof(user_app_t), sizeof(char));
    char *cjson_str = NULL;
    dct_app_all_read(&app_t);
    cJSON * root = cJSON_CreateObject();
    cJSON_AddItemToObject(root, "deviceID",
            cJSON_CreateString(app_t->device_id));
    cJSON_AddItemToObject(root, "primaryKey",
            cJSON_CreateString(app_t->primaryKey));
    cJSON_AddItemToObject(root, "endpointAddress",
            cJSON_CreateString(app_t->endpointAddress));
    cJSON_AddItemToObject(root, "mac1", cJSON_CreateString(app_t->mac1));
    cjson_str = cJSON_PrintUnformatted(root);
    wiced_http_response_stream_enable_chunked_transfer(now_stream);
    wiced_http_response_stream_write(now_stream, (const void*) cjson_str,
            strlen(cjson_str));
    wiced_http_response_stream_flush(now_stream);
    wiced_http_response_stream_disable_chunked_transfer(now_stream);
    cJSON_Delete(root);
    free(cjson_str);
    free(app_t);
}
