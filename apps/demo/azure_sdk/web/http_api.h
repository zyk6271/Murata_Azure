#ifndef HTTP_API_H
#define HTTP_API_H

char* flite_value(const char *source,uint32_t **size);
uint32_t extract_value_from_path(const char* path, char* dest);
uint32_t flite_url_key(const char *source, char *dest);
uint32_t flite_url_value(const char *source, char *dest);
uint32_t flite_extract_get_path(const char *source, char **dest);
void http_set_flush_value(char code,char *msg,uint32_t msg_size,uint32_t value);
void http_set_flush_string(char code,char *msg,uint32_t msg_size,char * value,uint32_t value_size);
void http_get_flush_value(char *msg,uint32_t msg_size,uint32_t value);
void http_get_flush_string(char *msg,uint32_t msg_size,char* value,uint32_t value_size);
void set_config(wiced_ssid_t ssid,wiced_wpa_key_t password);
void rst_work(void);
void azc_parse(const uint8_t*pMsg,uint32_t size);
void azc_flush(void);

#endif
