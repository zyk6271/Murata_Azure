#ifndef HTTP_API_H
#define HTTP_API_H

int verification_value(char *keyword,const uint8_t *source,int length);
int verification_string(char *keyword,const char *source,int length,char* result);
void http_flush_value(char code,char *msg,uint32_t value);
void http_flush_string(char code,char *msg,uint8_t *value);
uint8_t set_config(wiced_ssid_t ssid,char security,wiced_wep_key_t password);
void rst_work(void);

#endif
