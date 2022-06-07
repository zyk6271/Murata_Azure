#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <mqtt_api.h>
#include "wiced.h"
#include "mqtt_handle.h"

#include "iot_sample_common.h"
#include "azure/az_core.h"
#include "azure/az_iot.h"
#include "twin_upload.h"

#include "mqtt_api.h"
#include "resources.h"

extern az_iot_hub_client hub_client;
wiced_semaphore_t mqtt_restart_sem;

static wiced_ip_address_t    broker_address;
static wiced_mqtt_callback_t callbacks = mqtt_connection_event_cb;
static wiced_mqtt_event_type_t expected_event;
wiced_semaphore_t semaphore;
static wiced_mqtt_security_t security;
wiced_mqtt_object_t mqtt_object;

//#define MQTT_BROKER_ADDRESS                 "SYRKR.azure-devices.net"
//#define WICED_PUBTOPIC                      "devices/syr_1/messages/events/"
//#define WICED_SUBTOPIC                      "devices/syr_1/messages/devicebound/#"
//#define WICED_MESSAGE_STR                   "HELLO WICED"
//#define CLIENT_ID                           "syr_1"
//#define WICED_MQTT_TIMEOUT                  (8000)
//#define WICED_MQTT_DELAY_IN_MILLISECONDS    (1000)
//#define MQTT_MAX_RESOURCE_SIZE              (0x7fffffff)
//#define USERNAME                           "SYRKR.azure-devices.net/syr_1/?api-version=2020-09-30"
//#define PASSWORD                           "SharedAccessSignature sr=SYRKR.azure-devices.net%2Fdevices%2Fsyr_1&sig=R%2B6FcDtPHSHo7muafCA3oGWd3iNDquVSXmOcuFoE4Gw%3D&se=36001654129759"

#define MQTT_BROKER_ADDRESS                 "iotChinaTest.azure-devices.net"
#define WICED_TOPIC                         "devices/rsa22030001/messages/events/"
#define CLIENT_ID                           "rsa22030001"
#define WICED_MQTT_TIMEOUT                  (10000)
#define WICED_MQTT_DELAY_IN_MILLISECONDS    (1000)
#define MQTT_MAX_RESOURCE_SIZE              (0x7fffffff)
#define USERNAME                           "iotChinaTest.azure-devices.net/rsa22030001/?api-version=2020-09-30"
#define PASSWORD                           "SharedAccessSignature sr=iotChinaTest.azure-devices.net%2Fdevices%2Frsa22030001&sig=RSckYK2T1ilb13XrneeUaGG85y71rvu8vVLQyRDeeMk%3D&se=101649317205"
void handle_iot_message(wiced_mqtt_topic_msg_t* msg)
{
  //Initialize the incoming topic to a span
  az_span incoming_topic = az_span_create(msg->topic, msg->topic_len);

  //The message could be for three features so parse the topic to see which it is for
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
            expected_event = event->type;
            wiced_rtos_set_semaphore( &semaphore );
            break;
        case WICED_MQTT_EVENT_TYPE_DISCONNECTED:
            expected_event = event->type;
            wiced_rtos_set_semaphore( &semaphore );
            break;
        case WICED_MQTT_EVENT_TYPE_PUBLISHED:
            expected_event = event->type;
            wiced_rtos_set_semaphore( &semaphore );
            break;
        case WICED_MQTT_EVENT_TYPE_SUBSCRIBED:
            expected_event = event->type;
            wiced_rtos_set_semaphore( &semaphore );
            break;
        case WICED_MQTT_EVENT_TYPE_UNSUBSCRIBED:
            expected_event = event->type;
            wiced_rtos_set_semaphore( &semaphore );
            break;
        case WICED_MQTT_EVENT_TYPE_PUBLISH_MSG_RECEIVED:
        {
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

wiced_result_t mqtt_wait_for( wiced_mqtt_event_type_t event, uint32_t timeout )
{
    if ( wiced_rtos_get_semaphore( &semaphore, timeout ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }
    else
    {
        if ( event != expected_event )
        {
            printf("mqtt_wait_for event is %d,expected is %d\r\n",event,expected_event);
            return WICED_ERROR;
        }
    }
    return WICED_SUCCESS;
}

wiced_result_t mqtt_conn_open( wiced_mqtt_object_t mqtt_obj, wiced_ip_address_t *address, wiced_interface_t interface, wiced_mqtt_callback_t callback, wiced_mqtt_security_t *security )
{
    wiced_mqtt_pkt_connect_t conninfo;
    wiced_result_t ret = WICED_SUCCESS;

    memset( &conninfo, 0, sizeof( conninfo ) );

    conninfo.port_number = 0;                   /* set to 0 indicates library to use default settings */
    conninfo.mqtt_version = WICED_MQTT_PROTOCOL_VER4;
    conninfo.clean_session = 1;
    conninfo.client_id = (uint8_t*) CLIENT_ID;
    conninfo.keep_alive = 60;
    conninfo.password = (uint8_t*)PASSWORD;
    conninfo.username = (uint8_t*)USERNAME;
//    conninfo.password = NULL;
//    conninfo.username = NULL;
    conninfo.peer_cn = NULL;

    ret = wiced_mqtt_connect( mqtt_obj, address, interface, callback, security, &conninfo );
    if ( ret != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }
    if ( mqtt_wait_for( WICED_MQTT_EVENT_TYPE_CONNECT_REQ_STATUS, WICED_MQTT_TIMEOUT ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }
    return WICED_SUCCESS;
}

wiced_result_t mqtt_app_subscribe( wiced_mqtt_object_t mqtt_obj, char *topic, uint8_t qos )
{
    wiced_mqtt_msgid_t pktid;
    pktid = wiced_mqtt_subscribe( mqtt_obj, topic, qos );
    if ( pktid == 0 )
    {
        return WICED_ERROR;
    }
    if ( mqtt_wait_for( WICED_MQTT_EVENT_TYPE_SUBSCRIBED, WICED_MQTT_TIMEOUT ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }
    return WICED_SUCCESS;
}

wiced_result_t mqtt_app_publish( wiced_mqtt_object_t mqtt_obj, uint8_t qos, char *topic, uint8_t *data, uint32_t data_len )
{
    wiced_mqtt_msgid_t pktid;
    pktid = wiced_mqtt_publish( mqtt_obj, topic, data, data_len, qos );

    if ( pktid == 0 )
    {
        return WICED_ERROR;
    }

    if ( mqtt_wait_for( WICED_MQTT_EVENT_TYPE_PUBLISHED, WICED_MQTT_TIMEOUT ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }
    return WICED_SUCCESS;
}
void mqtt_init(void)
{
    uint32_t size_out;
    wiced_result_t ret = WICED_SUCCESS;

    wiced_rtos_init_semaphore( &mqtt_restart_sem );
    wiced_rtos_init_semaphore( &semaphore );

    resource_get_readonly_buffer( &resources_apps_DIR_azure_iot_hub_DIR_rootca_cer, 0, MQTT_MAX_RESOURCE_SIZE, &size_out, (const void **) &security.ca_cert );
    security.ca_cert_len = size_out;

    mqtt_object = (wiced_mqtt_object_t) malloc( WICED_MQTT_OBJECT_MEMORY_SIZE_REQUIREMENT );
    if ( mqtt_object == NULL )
    {
        WPRINT_APP_ERROR(("Dont have memory to allocate for mqtt object...\n"));
        return;
    }
    WPRINT_APP_INFO( ( "Resolving IP address of MQTT broker...\n" ) );
    ret = wiced_hostname_lookup( MQTT_BROKER_ADDRESS, &broker_address, 10000, WICED_STA_INTERFACE );
    WPRINT_APP_INFO(("Resolved Broker IP: %u.%u.%u.%u\n\n", (uint8_t)(GET_IPV4_ADDRESS(broker_address) >> 24),
                     (uint8_t)(GET_IPV4_ADDRESS(broker_address) >> 16),
                     (uint8_t)(GET_IPV4_ADDRESS(broker_address) >> 8),
                     (uint8_t)(GET_IPV4_ADDRESS(broker_address) >> 0)));
    if ( ret == WICED_ERROR || broker_address.ip.v4 == 0 )
    {
        WPRINT_APP_INFO(("Error in resolving DNS\n"));
        return;
    }
    WPRINT_APP_INFO(("[MQTT] Start Init\r\n..."));
    wiced_mqtt_init( mqtt_object );
    ret = mqtt_conn_open( mqtt_object,&broker_address, WICED_STA_INTERFACE, callbacks, &security );
    //ret = mqtt_conn_open( mqtt_object,&broker_address, WICED_STA_INTERFACE, callbacks, NULL );
    mqtt_print_status( ret, NULL, "Did you configure you broker IP address?\n" );
    if(ret == WICED_SUCCESS)
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
        WPRINT_APP_INFO(("[MQTT] Connect successfully\r\n..."));
        heart_init();
    }
    else
    {
//        wiced_rtos_set_semaphore( &mqtt_restart_sem );
        platform_mcu_reset();
    }
}
uint8_t mqtt_connect_try(void)
{
    wiced_result_t ret = WICED_SUCCESS;
    ret = wiced_hostname_lookup( MQTT_BROKER_ADDRESS, &broker_address, 10000, WICED_STA_INTERFACE );
    WPRINT_APP_INFO(("Resolved Broker IP: %u.%u.%u.%u\n\n", (uint8_t)(GET_IPV4_ADDRESS(broker_address) >> 24),
                    (uint8_t)(GET_IPV4_ADDRESS(broker_address) >> 16),
                    (uint8_t)(GET_IPV4_ADDRESS(broker_address) >> 8),
                    (uint8_t)(GET_IPV4_ADDRESS(broker_address) >> 0)));
    WPRINT_APP_INFO(("[MQTT] Opening connection..."));
    wiced_mqtt_deinit(mqtt_object);
    mqtt_wait_for( WICED_MQTT_EVENT_TYPE_PUBLISHED, WICED_MQTT_TIMEOUT );
    wiced_mqtt_init( mqtt_object );
    ret = mqtt_conn_open( mqtt_object,&broker_address, WICED_STA_INTERFACE, callbacks, &security );
    //ret = mqtt_conn_open( mqtt_object,&broker_address, WICED_STA_INTERFACE, callbacks, NULL );
    mqtt_print_status( ret, NULL, "Did you configure you broker IP address?\n" );
    if(ret == WICED_SUCCESS)
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
        WPRINT_APP_INFO(("[MQTT] Connect successfully\r\n..."));
    }
    return ret;
}

