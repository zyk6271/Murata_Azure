#ifndef TWIN_PARSE_H
#define TWIN_PARSE_H

#include "wiced.h"
#include "mqtt_api.h"
#include "azure/az_core.h"
#include "azure/az_iot.h"

typedef struct _syr_config
{
    uint32_t rse;
    uint32_t rsa;
    uint32_t rsi;
    uint32_t rsd;
    uint32_t cnf;
    uint32_t cnl;
    uint32_t lng;
    uint32_t sse;
    uint32_t ssa;
    uint32_t ssd;
}syr_config;
typedef struct _syr_info
{
    uint32_t com;
    uint32_t coa;
    uint32_t cod;
    uint32_t coe;
    uint32_t sup;
    uint32_t ver;
    uint32_t srn;
    uint32_t cnd;
}syr_info;
typedef struct _syr_tem
{
    uint32_t net;
    uint32_t bat;
    uint32_t ala;
    uint32_t alr;
}syr_tem;
typedef struct _syr_c2d
{
    uint32_t rst;
    uint32_t def;
    uint32_t ras;
}syr_c2d;
typedef struct _syr_status
{
    syr_config config;
    syr_info info;
    syr_tem tem;
    syr_c2d c2d;
}syr_status;

void get_device_twin_document(void);
void handle_device_twin_message(wiced_mqtt_topic_msg_t const* message,az_iot_hub_client_twin_response const* twin_response);

#endif
