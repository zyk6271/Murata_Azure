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

uint32_t extract_value_from_path(const char* path, char* dest)
{
    char* value_str = strrchr(path, '/') + 1;
    uint32_t value_len = strlen(value_str);
    strncpy(dest, value_str, value_len);
    dest[value_len] = '\0';
    return value_len;
}

uint32_t flite_url_key(const char *source, char *dest)
{
    char *ret;
    ret = strchr(source, 0x2F);
    strncpy(dest,&ret[1],strlen(ret)-1);
    printf("flite_url_value source is %s\r\n",source);
    printf("flite_url_value dest is %s\r\n",dest);
    return strlen(ret)-1;
}
uint32_t flite_url_value(const char *source, char *dest)
{
    char *ret;
    ret = strrchr(source, 0x2F);
    strncpy(dest,&ret[1],strlen(ret)-1);
    printf("flite_url_value source is %s\r\n",source);
    printf("flite_url_value dest is %s\r\n",dest);
    return strlen(ret)-1;
}

uint32_t flite_extract_get_path(const char *source, char **dest)
{
    char *ret = strrchr(source, '/');
    if (ret == NULL) {
        return 0;  // URL��û��'/'���޷���ȡֵ������0����
    }

    size_t copy_len = strlen(ret + 1);
    char *value = malloc(copy_len + 5);  // �����㹻����ڴ�ռ�
    if (value == NULL) {
        return 0;  // �ڴ����ʧ�ܣ�����0����
    }
    strncpy(value, ret + 1, copy_len);
    value[copy_len] = '\0';  // ����ַ�����β���

    for (int i = 0; i < copy_len; i++) {
        value[i] = toupper(value[i]);  // ����ȡ�����ַ���ת���ɴ�д
    }

    char *tmp = malloc(strlen(value) + 4);
    if (tmp == NULL) {
        free(value); // ����ʧ�ܣ��ͷ��ڴ�ռ�
        return 0;    // ����0����
    }
    sprintf(tmp, "get%s", value); // ��� "get" �ַ���

    *dest = tmp;
    free(value);
    return strlen(tmp);
}

uint32_t flite_extract_set_path(const char *source, char **dest)
{
    // ���� "/set/" �ַ���
   const char *set_str = strstr(source, "/set/");
   if (set_str == NULL) {
       return 0;  // URL ��û�� "/set/"���޷���ȡֵ������0����
   }
   set_str += 5;  // ���� "/set/" �Ӵ�

   // �ҵ� "/set/" ֮�����������
   const char *slash = strchr(set_str, '/');
   if (slash == NULL) {
       return 0;  // "/set/" ֮��û�� '/'���޷���ȡֵ������0����
   }

   // ��ȡ��ת����һ��ֵ
   size_t value1_len = slash - set_str;
   char *value1 = malloc(value1_len + 1);  // �����㹻����ڴ�ռ�
   if (value1 == NULL) {
       return 0;  // �ڴ����ʧ�ܣ�����0����
   }
   strncpy(value1, set_str, value1_len);
   value1[value1_len] = '\0';  // ����ַ�����β���
   for (int i = 0; i < value1_len; i++) {
       value1[i] = toupper(value1[i]);  // ����ȡ�����ַ���ת���ɴ�д
   }

   // ��ȡ��ת���ڶ���ֵ
   set_str = slash + 1;  // ���� '/' �ַ�
   size_t value2_len = strlen(set_str);
   char *value2 = malloc(value2_len + 1);  // �����㹻����ڴ�ռ�
   if (value2 == NULL) {
       free(value1); // �ڴ����ʧ�ܣ��ͷ� value1 �ڴ�
       return 0;     // ����0����
   }
   strncpy(value2, set_str, value2_len);
   value2[value2_len] = '\0';  // ����ַ�����β���
   for (int i = 0; i < value2_len; i++) {
       value2[i] = toupper(value2[i]);  // ����ȡ�����ַ���ת���ɴ�д
   }

   // ƴ�� "set" �� "value1" �� "value2"
   size_t result_len = 3 + value1_len + value2_len;
   char *result = malloc(result_len + 1);  // �����㹻����ڴ�ռ�
   if (result == NULL) {
       free(value1); // �ڴ����ʧ�ܣ��ͷ� value1 �� value2 �ڴ�
       free(value2);
       return 0;     // ����0����
   }
   snprintf(result, result_len + 1, "set%s%s", value1, value2);

   free(value1); // �ͷ� value1 �� value2 �ڴ�
   free(value2);

   *dest = result;
   return result_len;
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
    if(code == ERRDATA_CODE)
    {
        cJSON_AddItemToObject(root, cat_tmp, cJSON_CreateString("MIMA"));
    }
    else
    {
        cJSON_AddItemToObject(root, cat_tmp, cJSON_CreateString("OK"));
    }
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
    if(code == ERRDATA_CODE)
    {
        cJSON_AddItemToObject(root, cat_tmp, cJSON_CreateString("MIMA"));
    }
    else
    {
        cJSON_AddItemToObject(root, cat_tmp, cJSON_CreateString("OK"));
    }
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
    wiced_rtos_init_timer(&rst_timer, 3000, rst_timer_callback, 0);
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
    platform_dct_azure_config_t *app_dct_write;
    app_dct_write = calloc(sizeof(platform_dct_azure_config_t),sizeof(char));

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
    app_dct_write->init_flag = 1;
    dct_app_azc_write(app_dct_write);
    free(app_dct_write);
    cJSON_Delete(root);
}
void azc_flush(void)
{
    platform_dct_azure_config_t *app_t = calloc(sizeof(platform_dct_azure_config_t), sizeof(char));
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
