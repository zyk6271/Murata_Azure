#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <mqtt_api.h>
#include "wiced.h"
#include "mqtt_handle.h"
#include "storage.h"
#include "base64.h"

#include "iot_sample_common.h"
#include "azure/az_core.h"
#include "azure/az_iot.h"
#include "twin_upload.h"
#include "azure_server.h"

#include "mqtt_api.h"
#include "resources.h"

wiced_mqtt_object_t mqtt_object;
extern az_iot_hub_client hub_client;

EventGroupHandle_t MQTT_EventHandler;

uint8_t MQTT_Resolving = WICED_NOT_CONNECTED;
uint8_t MQTT_Connected = WICED_NOT_CONNECTED;

char* endpoint_adress = NULL;
char* device_id = NULL;
char* endpoint_user = NULL;
char* endpoint_key = NULL;

#define MQTT_TYPE_CONNECT_REQ_EVENT         1<<0
#define MQTT_TYPE_DISCONNECTED_EVENT        1<<1
#define MQTT_TYPE_PUBLISHED_EVENT           1<<2
#define MQTT_TYPE_SUBSCRIBED_EVENT          1<<3
#define MQTT_TYPE_UNSUBSCRIBED_EVENT        1<<4
#define MQTT_TYPE_PUBLISH_MSG_EVENT         1<<5
#define MQTT_TYPE_RECEIVED_MSG_EVENT        1<<6

static wiced_ip_address_t    broker_address;
static wiced_mqtt_callback_t callbacks = mqtt_connection_event_cb;
static wiced_mqtt_security_t security;

char MQTT_BROKER_ADDRESS[]=                 "NULL";
char CLIENT_ID[]=                           "NULL";
int WICED_MQTT_TIMEOUT=                     (10000);
int WICED_MQTT_DELAY_IN_MILLISECONDS=       (1000);
int MQTT_MAX_RESOURCE_SIZE=                 (0x7fffffff);
char USERNAME[]=                            "NULL";
char PASSWORD[]=                            "NULL";

void handle_iot_message(wiced_mqtt_topic_msg_t* msg)
{
    //Initialize the incoming topic to a span
    az_span incoming_topic = az_span_create(msg->topic, msg->topic_len);

    az_iot_hub_client_method_request method_request;
    az_iot_hub_client_c2d_request c2d_request;
    az_iot_hub_client_twin_response twin_response;
    if (az_result_succeeded(az_iot_hub_client_methods_parse_received_topic(&hub_client, incoming_topic, &method_request)))
    {
      IOT_SAMPLE_LOG_SUCCESS("Methods request %.*s\r\n",(int) msg->data_len, msg->data);
      IOT_SAMPLE_LOG_SUCCESS("Client parsed method request.");
    }
    else if (az_result_succeeded(az_iot_hub_client_c2d_parse_received_topic(&hub_client, incoming_topic, &c2d_request)))
    {
      parse_c2d_message(msg->topic, msg->topic_len, msg, &c2d_request);
    }
    else if (az_result_succeeded(az_iot_hub_client_twin_parse_received_topic(&hub_client, incoming_topic, &twin_response)))
    {
      if(parse_device_twin_message(msg->topic, msg->topic_len, msg, &twin_response))
      {
          handle_device_twin_message(msg, &twin_response);
      }
    }
}
void mqtt_print_status( wiced_result_t result, const char * ok_message, const char * error_message )
{
    if ( result == WICED_SUCCESS )
    {
        if ( ok_message != NULL )
        {
            WPRINT_APP_INFO(( "OK (%s)\n\n", (ok_message)));
        }
        else
        {
            WPRINT_APP_INFO(( "OK.\n\n" ));
        }
    }
    else
    {
        if ( error_message != NULL )
        {
            WPRINT_APP_INFO(( "ERROR (%s)\n\n", (error_message)));
        }
        else
        {
            WPRINT_APP_INFO(( "ERROR.\n\n" ));
        }
    }
}
wiced_result_t mqtt_connection_event_cb( wiced_mqtt_object_t mqtt_object, wiced_mqtt_event_info_t *event )
{
    switch ( event->type )
    {

        case WICED_MQTT_EVENT_TYPE_CONNECT_REQ_STATUS:
            xEventGroupSetBits(MQTT_EventHandler,MQTT_TYPE_CONNECT_REQ_EVENT);
            break;
        case WICED_MQTT_EVENT_TYPE_DISCONNECTED:
            xEventGroupSetBits(MQTT_EventHandler,MQTT_TYPE_DISCONNECTED_EVENT);
            WPRINT_APP_INFO(( "WICED_MQTT_EVENT_TYPE_DISCONNECTED\n\n" ));
            break;
        case WICED_MQTT_EVENT_TYPE_PUBLISHED:
            xEventGroupSetBits(MQTT_EventHandler,MQTT_TYPE_PUBLISHED_EVENT);
            break;
        case WICED_MQTT_EVENT_TYPE_SUBSCRIBED:
            xEventGroupSetBits(MQTT_EventHandler,MQTT_TYPE_SUBSCRIBED_EVENT);
            break;
        case WICED_MQTT_EVENT_TYPE_UNSUBSCRIBED:
            xEventGroupSetBits(MQTT_EventHandler,MQTT_TYPE_UNSUBSCRIBED_EVENT);
            break;
        case WICED_MQTT_EVENT_TYPE_PUBLISH_MSG_RECEIVED:
        {
            xEventGroupSetBits(MQTT_EventHandler,MQTT_TYPE_RECEIVED_MSG_EVENT);
            wiced_mqtt_topic_msg_t msg = event->data.pub_recvd;
//            WPRINT_APP_INFO(( "[MQTT] Received %.*s  for TOPIC : %.*s\n\n", (int) msg.data_len, msg.data, (int) msg.topic_len, msg.topic ));
            handle_iot_message(&msg);
        }
            break;
        default:
            break;
    }
    return WICED_SUCCESS;
}
wiced_result_t mqtt_conn_open( wiced_mqtt_object_t mqtt_obj, wiced_ip_address_t *address, wiced_interface_t interface, wiced_mqtt_callback_t callback, wiced_mqtt_security_t *security )
{
    EventBits_t EventValue;
    wiced_mqtt_pkt_connect_t conninfo;
    wiced_result_t ret = WICED_SUCCESS;

    memset( &conninfo, 0, sizeof( conninfo ) );

    conninfo.port_number = 0;                   /* set to 0 indicates library to use default settings */
    conninfo.mqtt_version = WICED_MQTT_PROTOCOL_VER4;
    conninfo.clean_session = 1;
    conninfo.client_id = (uint8_t*) device_id;
    conninfo.keep_alive = 60;
    conninfo.username = (uint8_t*)endpoint_user;
    conninfo.password = (uint8_t*)endpoint_key;
    conninfo.peer_cn = NULL;

    ret = wiced_mqtt_connect( mqtt_obj, address, interface, callback, security, &conninfo );
    if ( ret != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }

    EventValue = xEventGroupWaitBits(MQTT_EventHandler,MQTT_TYPE_CONNECT_REQ_EVENT,pdTRUE,pdTRUE,WICED_MQTT_TIMEOUT);
    if(EventValue & MQTT_TYPE_CONNECT_REQ_EVENT)
    {
        return WICED_SUCCESS;
    }
    else
    {
        return WICED_ERROR;
    }
}

wiced_result_t mqtt_app_subscribe( wiced_mqtt_object_t mqtt_obj, char *topic, uint8_t qos )
{
    EventBits_t EventValue;
    wiced_mqtt_msgid_t pktid;
    pktid = wiced_mqtt_subscribe( mqtt_obj, topic, qos );
    if ( pktid == 0 )
    {
        return WICED_ERROR;
    }

    EventValue = xEventGroupWaitBits(MQTT_EventHandler,MQTT_TYPE_SUBSCRIBED_EVENT,pdTRUE,pdTRUE,WICED_MQTT_TIMEOUT);
    if(EventValue & MQTT_TYPE_SUBSCRIBED_EVENT)
    {
        return WICED_SUCCESS;
    }
    else
    {
        return WICED_ERROR;
    }
}

wiced_result_t mqtt_app_publish( wiced_mqtt_object_t mqtt_obj, uint8_t qos, char *topic, uint8_t *data, uint32_t data_len )
{
    EventBits_t EventValue;
    wiced_mqtt_msgid_t pktid;
    pktid = wiced_mqtt_publish( mqtt_obj, topic, data, data_len, qos );

    if ( pktid == 0 )
    {
        return WICED_ERROR;
    }

    EventValue = xEventGroupWaitBits(MQTT_EventHandler,MQTT_TYPE_PUBLISH_MSG_EVENT,pdTRUE,pdTRUE,WICED_MQTT_TIMEOUT);
    if(EventValue & MQTT_TYPE_PUBLISH_MSG_EVENT)
    {
        return WICED_SUCCESS;
    }
    else
    {
        return WICED_ERROR;
    }
}
void mqtt_connect_azure(void)
{
    wifi_status_change(3);
    wiced_result_t ret = WICED_SUCCESS;
    MQTT_Resolving = WICED_NOT_CONNECTED;
    MQTT_Connected = WICED_NOT_CONNECTED;
    while(MQTT_Resolving == WICED_NOT_CONNECTED)
    {
        WPRINT_APP_INFO( ( "Resolving IP address of MQTT broker...\n" ) );
        ret = wiced_hostname_lookup( endpoint_adress, &broker_address, 10000, WICED_STA_INTERFACE );
        WPRINT_APP_INFO(("Resolved Broker IP: %u.%u.%u.%u\n\n", (uint8_t)(GET_IPV4_ADDRESS(broker_address) >> 24),
                         (uint8_t)(GET_IPV4_ADDRESS(broker_address) >> 16),
                         (uint8_t)(GET_IPV4_ADDRESS(broker_address) >> 8),
                         (uint8_t)(GET_IPV4_ADDRESS(broker_address) >> 0)));
        if ( ret == WICED_ERROR || broker_address.ip.v4 == 0 )
        {
            WPRINT_APP_INFO(("Error in resolving DNS\n"));
        }
        else
        {
            MQTT_Resolving = WICED_SUCCESS;
        }
        wiced_rtos_delay_milliseconds(500);
    }

    while(MQTT_Connected == WICED_NOT_CONNECTED)
    {
        ret = mqtt_conn_open( mqtt_object,&broker_address, WICED_STA_INTERFACE, callbacks, &security );
        if ( ret == WICED_ERROR )
        {
            WPRINT_APP_INFO(("Did you configure you broker IP address?\n"));
        }
        else
        {
            WPRINT_APP_INFO(("[MQTT] Subscribing..."));
            ret = mqtt_app_subscribe( mqtt_object, AZ_IOT_HUB_CLIENT_TWIN_PATCH_SUBSCRIBE_TOPIC , WICED_MQTT_QOS_DELIVER_AT_LEAST_ONCE );
            mqtt_print_status( ret, NULL, "[MQTT]Subscribing Fail\r" );
            WPRINT_APP_INFO(("[MQTT] Subscribing..."));
            ret = mqtt_app_subscribe( mqtt_object, AZ_IOT_HUB_CLIENT_TWIN_RESPONSE_SUBSCRIBE_TOPIC , WICED_MQTT_QOS_DELIVER_AT_LEAST_ONCE );
            mqtt_print_status( ret, NULL, "[MQTT]Subscribing Fail\r" );
            WPRINT_APP_INFO(("[MQTT] Subscribing..."));
            ret = mqtt_app_subscribe( mqtt_object, AZ_IOT_HUB_CLIENT_C2D_SUBSCRIBE_TOPIC , WICED_MQTT_QOS_DELIVER_AT_LEAST_ONCE );//C2D
            mqtt_print_status( ret, NULL, "[MQTT]Subscribing Fail\r" );
            WPRINT_APP_INFO(("[MQTT] Subscribing..."));
            ret = mqtt_app_subscribe( mqtt_object, AZ_IOT_HUB_CLIENT_METHODS_SUBSCRIBE_TOPIC , WICED_MQTT_QOS_DELIVER_AT_LEAST_ONCE );
            mqtt_print_status( ret, NULL, "[MQTT]Subscribing Fail\r" );
            MQTT_Connected = WICED_SUCCESS;
        }
        wiced_rtos_delay_milliseconds(500);
    }
    wifi_status_change(4);
    azure_refresh();
}
void mqtt_reconnect_azure(void)
{
    wiced_mqtt_deinit(mqtt_object);
    wiced_mqtt_init( mqtt_object );
    mqtt_connect_azure();
}
void mqtt_disconnect_azure(void)
{
    wiced_mqtt_deinit(mqtt_object);
}
void urlencode(char *dst, char *src, int len) {
  char *hex = "0123456789abcdef";
  char *src_p = src;
  char *dst_p = dst;
  if (!len) len = (int)strlen(src);

  while( src_p && len-- ){
    if( ('a' <= *src_p && *src_p <= 'z')
        || ('A' <= *src_p && *src_p <= 'Z')
        || ('0' <= *src_p && *src_p <= '9')
        || ( *src_p == '-' )
        || ( *src_p == '_' )
        ){
      *dst_p++ = *src_p++;
    } else {
      *dst_p++ = ('%');
      *dst_p++ = (hex[*src_p >> 4]);
      *dst_p++ = (hex[*src_p & 15]);
      src_p++;
    }
  }
  *dst_p = '\0';
}
void signature_generate(char *pass, char*host, char *devname, char *key, char *time_exp)
{
    char sign_origin[256];
    strcpy(sign_origin, host);
    strcat(sign_origin, "%2Fdevices%2F");
    strcat(sign_origin, devname);
    strcat(sign_origin, "\n");
    strcat(sign_origin, time_exp);

    unsigned char key_decode[100];
    uint8_t key_decode_size;
    key_decode_size = base64_decode(key, strlen(key), key_decode,strlen(key), BASE64_STANDARD );

    unsigned char sign_calc[32];
    sha2_hmac((unsigned char*)key_decode,key_decode_size,(unsigned char*)sign_origin,strlen(sign_origin),&sign_calc,0);
    char signature_buffer[100];
    int result = base64_encode(sign_calc, 32, (unsigned char*)signature_buffer, 44, BASE64_STANDARD );

    char sas[128];
    urlencode(sas, signature_buffer, 44);

    strcpy(pass, "SharedAccessSignature sr=");
    strcat(pass, host);
    strcat(pass, "%2Fdevices%2F");
    strcat(pass, devname);
    strcat(pass, "&sig=");
    strcat(pass, sas);
    strcat(pass, "&se=");
    strcat(pass, time_exp);
    strcat(pass, "&skn=");
}
void mqtt_config_read(void)
{
    user_app_t *app_t = calloc(sizeof(user_app_t), sizeof(char));
    dct_app_all_read(&app_t);
    if(app_t->init_flag==1)
    {
        endpoint_adress = calloc(sizeof(app_t->endpointAddress), sizeof(char));
        strncpy(endpoint_adress,app_t->endpointAddress,strlen(app_t->endpointAddress));

        device_id = calloc(sizeof(app_t->device_id), sizeof(char));
        strncpy(device_id,app_t->device_id,strlen(app_t->device_id));

        endpoint_user = calloc(sizeof(app_t->endpointAddress) + sizeof(app_t->device_id) + 30, sizeof(char));
        strncpy(endpoint_user,app_t->endpointAddress,strlen(app_t->endpointAddress));
        strcat(endpoint_user,"/");
        strcat(endpoint_user,device_id);
        strcat(endpoint_user,"/?api-version=2020-09-30");

        endpoint_key = calloc(sizeof(app_t->primaryKey), sizeof(char));
        signature_generate(endpoint_key,endpoint_adress,device_id,app_t->primaryKey,"1693097521");
    }
    else
    {
        endpoint_adress = calloc(sizeof(MQTT_BROKER_ADDRESS), sizeof(char));
        strncpy(endpoint_adress,MQTT_BROKER_ADDRESS,strlen(MQTT_BROKER_ADDRESS));

        device_id = calloc(sizeof(CLIENT_ID), sizeof(char));
        strncpy(device_id,CLIENT_ID,strlen(CLIENT_ID));

        endpoint_user = calloc(sizeof(USERNAME), sizeof(char));
        strncpy(endpoint_user,USERNAME,strlen(USERNAME));

        endpoint_key = calloc(sizeof(PASSWORD), sizeof(char));
        strncpy(endpoint_key,PASSWORD,strlen(PASSWORD));
    }
    free(app_t);
    printf("endpoint_adress is %s\r\n",endpoint_adress);
    printf("device_id is %s\r\n",device_id);
    printf("endpoint_user is %s\r\n",endpoint_user);
    printf("endpoint_key is %s\r\n",endpoint_key);
}
void mqtt_init(void)
{
    uint32_t size_out;
    resource_get_readonly_buffer( &resources_apps_DIR_azure_iot_hub_DIR_rootca_cer, 0, MQTT_MAX_RESOURCE_SIZE, &size_out, (const void **) &security.ca_cert );
    security.ca_cert_len = size_out;

    MQTT_EventHandler = xEventGroupCreate();

    mqtt_object = (wiced_mqtt_object_t) malloc( WICED_MQTT_OBJECT_MEMORY_SIZE_REQUIREMENT );
    if ( mqtt_object == NULL )
    {
        WPRINT_APP_ERROR(("Dont have memory to allocate for mqtt object...\n"));
    }
    wiced_mqtt_init( mqtt_object );
    mqtt_connect_azure();
}
