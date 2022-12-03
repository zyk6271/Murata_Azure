#include "wiced.h"
#include "uart_core.h"
#include "heart.h"

static wiced_thread_t wifi_watch;
static wiced_thread_t mqtt_watch;

wiced_semaphore_t wifi_restart_sem;
wiced_semaphore_t mqtt_reconnect_sem;
wiced_event_flags_t wifi_heart_event;

uint8_t wifi_status;

uint8_t link_status;
uint8_t mqtt_status;

#define KEEP_ALIVE_ID_NFD          0
#define KEEP_ALIVE_PERIOD_NFD_MSEC 30000

#define MAX_KEEP_ALIVE_PACKET_SIZE 512

static uint8_t  keep_alive_packet_buffer[MAX_KEEP_ALIVE_PACKET_SIZE];

#define EVENT_WIFI_RECONNECT        1<<0
#define EVENT_WIFI_CONNECTED        1<<1
#define EVENT_WIFI_DISCONNECTED     1<<2

void print_keep_alive_info( wiced_keep_alive_packet_t* packet_info )
{
    int i;
    WPRINT_APP_INFO( ( "Keep alive ID: %d\n", packet_info->keep_alive_id ) );
    WPRINT_APP_INFO( ( "Repeat period: %lu ms\n", packet_info->period_msec ) );
    WPRINT_APP_INFO( ( "Packet length: %d bytes\n", packet_info->packet_length ) );
    WPRINT_APP_INFO( ( "Packet bytes : ") );
    for ( i = 1; i <= packet_info->packet_length; i++ )
    {
        WPRINT_APP_INFO( ( "%02X", packet_info->packet[i] ) );
        if ( i % 4 == 0 )
        {
            WPRINT_APP_INFO( ( ":" ) );
        }
    }
    WPRINT_APP_INFO( ( "\n\n" ) );
}
void keep_alive(void)
{
    wiced_result_t            status;
    wiced_keep_alive_packet_t keep_alive_packet_info;

    /* Setup a Null function data frame keep alive */
    keep_alive_packet_info.keep_alive_id = KEEP_ALIVE_ID_NFD,
    keep_alive_packet_info.period_msec   = KEEP_ALIVE_PERIOD_NFD_MSEC,
    keep_alive_packet_info.packet_length = 0;

    status = wiced_wifi_add_keep_alive( &keep_alive_packet_info );
    switch ( status )
    {
        case WICED_SUCCESS:
        {
            WPRINT_APP_INFO( ( "\r\nAdded:\n") );
            WPRINT_APP_INFO( ( "  - Null Function Data frame with repeat period of %d milliseconds \n", KEEP_ALIVE_PERIOD_NFD_MSEC ) );
            break;
        }
        case WICED_TIMEOUT:
        {
            WPRINT_APP_INFO( ( "Timeout: Adding Null Function Data frame keep alive packet\n" ) );
            break;
        }
        default:
            WPRINT_APP_INFO( ( "Error[%d]: Adding Null Function Data frame keep alive packet\n", status ) );
            break;
    }
    /* Get Null Function Data Frame keep alive packet info */
    keep_alive_packet_info.keep_alive_id = KEEP_ALIVE_ID_NFD;
    keep_alive_packet_info.packet_length = MAX_KEEP_ALIVE_PACKET_SIZE;
    keep_alive_packet_info.packet        = &keep_alive_packet_buffer[0];

    status = wiced_wifi_get_keep_alive( &keep_alive_packet_info );
    if ( status == WICED_SUCCESS )
    {
        print_keep_alive_info( &keep_alive_packet_info );
    }
    else
    {
        WPRINT_APP_INFO( ( "ERROR[%d]: Get keep alive packet failed for ID:%d\n", status, KEEP_ALIVE_ID_NFD) );
    }
}
void mqtt_disconnect_callabck(void)
{
    printf("mqtt_disconnect_callabck\n");
    wifi_status_change(2);
    wiced_rtos_set_semaphore(&mqtt_reconnect_sem);
}
void mqtt_connect_callabck(void)
{
    printf("mqtt_connect_callabck\n");
    wifi_status_change(3);
}
void wifi_disconnect_callback(void)
{
    wiced_rtos_set_event_flags(&wifi_heart_event,EVENT_WIFI_DISCONNECTED);
}
void wifi_connect_callback(void)
{
    wiced_rtos_set_event_flags(&wifi_heart_event,EVENT_WIFI_CONNECTED);
}
void mqtt_watch_callback( uint32_t arg )
{
    printf("mqtt_watch created\r\n");
    while ( 1 )
    {
        wiced_rtos_get_semaphore( &mqtt_reconnect_sem, WICED_WAIT_FOREVER );
        if(link_status)
        {
            printf("mqtt_connect_azure\n");
            mqtt_connect_azure();
        }
    }
}
void mqtt_watch_init(void)
{
    wiced_rtos_init_semaphore( &mqtt_reconnect_sem );
    wiced_rtos_create_thread( &mqtt_watch, 7, "mqtt_watch", mqtt_watch_callback, 8192, 0 );
}

void wifi_watch_callback( uint32_t arg )
{
    uint32_t events;
    while ( 1 )
    {
        wiced_rtos_wait_for_event_flags(&wifi_heart_event,0xFFFF, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,WICED_WAIT_FOREVER);
       if(events & EVENT_WIFI_CONNECTED)
        {
            printf("wifi_connect_callback\n");
            link_status = 1;
            link_up_callback();
        }
        else if(events & EVENT_WIFI_DISCONNECTED)
        {
            printf("wifi_disconnect_callback\n");
            mqtt_stop();
            link_status = 0;
            wifi_status_change(1);
            wiced_wifi_disable_keep_alive( 0 );
            wiced_network_down(WICED_STA_INTERFACE);
            printf("wiced_network_down\r\n");
            wiced_network_up( WICED_STA_INTERFACE, WICED_USE_EXTERNAL_DHCP_SERVER, NULL );
        }
    }
}
void wifi_watch_init(void)
{
    wiced_rtos_init_event_flags(&wifi_heart_event);
    wiced_rtos_init_semaphore( &wifi_restart_sem );
    wiced_rtos_create_thread( &wifi_watch, 7, "wifi_watch", wifi_watch_callback, 8192, 0 );
}
void wifi_status_change(uint8_t value)
{
    wifi_status = value;

    uint8_t send_len = 0;
    send_len = set_wifi_uart_byte(send_len,value);
    wifi_uart_write_frame(WIFI_STATE_CMD, MCU_TX_VER, send_len);
}
void ap_status_change(uint8_t value)
{
    if(value)
    {
        uint8_t send_len = 0;
        send_len = set_wifi_uart_byte(send_len,5);
        wifi_uart_write_frame(WIFI_STATE_CMD, MCU_TX_VER, send_len);
    }
    else
    {
        uint8_t send_len = 0;
        send_len = set_wifi_uart_byte(send_len,4);
        wifi_uart_write_frame(WIFI_STATE_CMD, MCU_TX_VER, send_len);
    }

}
uint8_t wifi_status_get(void)
{
    return wifi_status;
}
