#include "wiced.h"
#include "mqtt_api.h"
#include "resources.h"
#include "sentient.h"
#include "wifi_paser.h"
#include "mqttwork.h"
#include "control_paser.h"
#define RUN_COMMAND_PRINT_STATUS_AND_BREAK_ON_ERROR( command, ok_message, error_message )   \
        {                                                                                   \
            ret = (command);                                                                \
            mqtt_print_status( ret, (const char *)ok_message, (const char *)error_message );\
            if ( ret != WICED_SUCCESS ) break;                                              \
        }


/******************************************************
 *               Variable Definitions
 ******************************************************/
wiced_ip_address_t    broker_address;
wiced_mqtt_callback_t callbacks = mqtt_connection_event_cb;
wiced_mqtt_event_type_t expected_event;
wiced_semaphore_t semaphore;
wiced_mqtt_security_t security;
wiced_mqtt_object_t mqtt_object;

uint8_t mqtt_connect_flag = 0;

#define MQTT_BROKER_ADDRESS                 "dajie.tech"
/* Name of topic name */
#define WICED_SUBTOPIC                      "/Sentient/SYR/Demo/Rx"

#define WICED_PUBTOPIC                      "/Sentient/SYR/Demo/Tx"
/* MQTT message content */
#define WICED_MESSAGE_STR                   "1LD To Sever"
/* MQTT client ID */
#define CLIENT_ID                           "SYR_TEST"

#define WICED_MQTT_TIMEOUT                  (2000)

#define WICED_MQTT_DELAY_IN_MILLISECONDS    (5000)

#define MQTT_MAX_RESOURCE_SIZE              (0x7fffffff)

void mqtt_connect_try(void)
{
    platform_mcu_reset( );
}

void mqtt_init(void)
{
    wiced_result_t ret = WICED_SUCCESS;
    mqtt_object = (wiced_mqtt_object_t) malloc( WICED_MQTT_OBJECT_MEMORY_SIZE_REQUIREMENT );
    wiced_mqtt_init( mqtt_object );
    WPRINT_APP_INFO( ( "Resolving IP address of MQTT broker...\n" ) );
    wiced_hostname_lookup( MQTT_BROKER_ADDRESS, &broker_address, 10000, WICED_STA_INTERFACE );
    WPRINT_APP_INFO(("Resolved Broker IP: %u.%u.%u.%u\n\n", (uint8_t)(GET_IPV4_ADDRESS(broker_address) >> 24),
                    (uint8_t)(GET_IPV4_ADDRESS(broker_address) >> 16),
                    (uint8_t)(GET_IPV4_ADDRESS(broker_address) >> 8),
                    (uint8_t)(GET_IPV4_ADDRESS(broker_address) >> 0)));
    ret = mqtt_conn_open( mqtt_object,&broker_address, WICED_STA_INTERFACE, callbacks, NULL );
    if(ret != WICED_SUCCESS){
        printf("MQTT Init Fail\r\n");
    }
    else{
        printf("MQTT Init Success\r\n");
    }
    mqtt_app_subscribe( mqtt_object, WICED_SUBTOPIC , WICED_MQTT_QOS_DELIVER_AT_MOST_ONCE );
    if(ret != WICED_SUCCESS){
        printf("MQTT subscribe Fail\r\n");
    }
    else{
        printf("MQTT subscribe Success\r\n");
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

/*
 * Call back function to handle connection events.
 */
wiced_result_t mqtt_connection_event_cb( wiced_mqtt_object_t mqtt_object, wiced_mqtt_event_info_t *event )
{
    switch ( event->type )
    {

        case WICED_MQTT_EVENT_TYPE_CONNECT_REQ_STATUS:
        case WICED_MQTT_EVENT_TYPE_DISCONNECTED:
        case WICED_MQTT_EVENT_TYPE_PUBLISHED:
        case WICED_MQTT_EVENT_TYPE_SUBSCRIBED:
        case WICED_MQTT_EVENT_TYPE_UNSUBSCRIBED:
        {
            expected_event = event->type;
            wiced_rtos_set_semaphore( &semaphore );
        }
            break;
        case WICED_MQTT_EVENT_TYPE_PUBLISH_MSG_RECEIVED:
        {
            wiced_mqtt_topic_msg_t msg = event->data.pub_recvd;
            //WPRINT_APP_INFO(( "[MQTT] Received %.*s  for TOPIC : %.*s\n\n", (int) msg.data_len, msg.data, (int) msg.topic_len, msg.topic ));
            control_decoder(msg.data);
        }
            break;
        default:
            break;
    }
    return WICED_SUCCESS;
}

/*
 * Call back function to handle channel events.
 *
 * For each event:
 *  - The call back will set the expected_event global to the received event.
 *  - The call back will set the event semaphore to run any blocking thread functions waiting on this event
 *  - Some events will also log other global variables required for extra processing.
 *
 * A thread function will probably be waiting for the received event. Once the event is received and the
 * semaphore is set, the thread function will check for the received event and make sure it matches what
 * it is expecting.
 *
 * Note:  This mechanism is not thread safe as we are using a non protected global variable for read/write.
 * However as this snip is a single controlled thread, there is no risc of racing conditions. It is
 * however not recommended for multi-threaded applications.
 */

/*
 * A blocking call to an expected event.
 */
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
            return WICED_ERROR;
        }
    }
    return WICED_SUCCESS;
}

/*
 * Open a connection and wait for WICED_MQTT_TIMEOUT period to receive a connection open OK event
 */
wiced_result_t mqtt_conn_open( wiced_mqtt_object_t mqtt_obj, wiced_ip_address_t *address, wiced_interface_t interface, wiced_mqtt_callback_t callback, wiced_mqtt_security_t *security )
{
    wiced_mqtt_pkt_connect_t conninfo;
    wiced_result_t ret = WICED_SUCCESS;

    memset( &conninfo, 0, sizeof( conninfo ) );

    conninfo.port_number = 1883;                   /* set to 0 indicates library to use default settings */
    conninfo.mqtt_version = WICED_MQTT_PROTOCOL_VER4;
    conninfo.clean_session = 1;
    conninfo.client_id = (uint8_t*) CLIENT_ID;
    conninfo.keep_alive = 1;
    conninfo.password = NULL;
    conninfo.username = NULL;
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

/*
 * Close a connection and wait for 5 seconds to receive a connection close OK event
 */
wiced_result_t mqtt_conn_close( wiced_mqtt_object_t mqtt_obj )
{
    if ( wiced_mqtt_disconnect( mqtt_obj ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }
    if ( mqtt_wait_for( WICED_MQTT_EVENT_TYPE_DISCONNECTED, WICED_MQTT_TIMEOUT ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }
    return WICED_SUCCESS;
}

/*
 * Subscribe to WICED_TOPIC and wait for 5 seconds to receive an ACM.
 */
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

/*
 * Unsubscribe from WICED_TOPIC and wait for 10 seconds to receive an ACM.
 */
wiced_result_t mqtt_app_unsubscribe( wiced_mqtt_object_t mqtt_obj, char *topic )
{
    wiced_mqtt_msgid_t pktid;
    pktid = wiced_mqtt_unsubscribe( mqtt_obj, topic );

    if ( pktid == 0 )
    {
        return WICED_ERROR;
    }
    if ( mqtt_wait_for( WICED_MQTT_EVENT_TYPE_UNSUBSCRIBED, WICED_MQTT_TIMEOUT*2 ) != WICED_SUCCESS )
    {
        return WICED_ERROR;
    }
    return WICED_SUCCESS;
}
/*
 * Publish (send) WICED_MESSAGE_STR to WICED_TOPIC and wait for 5 seconds to receive a PUBCOMP (as it is QoS=2).
 */
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
void mqtt_send(uint8_t *buf)
{
    wiced_mqtt_publish(mqtt_object,WICED_PUBTOPIC,buf,strlen(buf),0);
    printf("mqtt send %s\r\n",buf);
}
