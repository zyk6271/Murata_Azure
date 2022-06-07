#include "wiced.h"

#define PING_TIMEOUT_MS          2000
#define PING_PERIOD_MS           10000
#define BING_Default MAKE_IPV4_ADDRESS(202, 89, 233, 100)

static wiced_thread_t heart_t = NULL;
static wiced_ip_address_t  ping_target_ip;
static wiced_timed_event_t ping_timed_event;

extern wiced_semaphore_t mqtt_restart_sem;

uint8_t retry_times;

static wiced_result_t send_ping( void* arg )
{
    uint32_t elapsed_ms;
    wiced_result_t status;

    //WPRINT_APP_INFO(("Ping about to be sent\n"));

    status = wiced_ping( WICED_STA_INTERFACE, &ping_target_ip, PING_TIMEOUT_MS, &elapsed_ms );

    if ( status == WICED_SUCCESS )
    {
        retry_times = 0;
        WPRINT_APP_INFO(("Ping Reply : %lu ms\n", (unsigned long)elapsed_ms ));
    }
    else if ( status == WICED_TIMEOUT )
    {
        retry_times ++;
        WPRINT_APP_INFO(("Ping timeout,time is %d\n",retry_times));
    }
    else
    {
        //WPRINT_APP_INFO(("Ping error\n"));
    }
    if(retry_times>6)
    {
        retry_times = 0;
        platform_mcu_reset();
    }
    return WICED_SUCCESS;
}

void heart_callback( uint32_t arg )
{
    uint8_t ret;
    printf("heart_thread created\n");
    while ( 1 )
    {
        wiced_rtos_get_semaphore( &mqtt_restart_sem, WICED_WAIT_FOREVER );
        do
        {
            ret = mqtt_connect_try();
            wiced_rtos_delay_milliseconds(1000);
        }
        while(ret == WICED_ERROR);
    }
}


void heart_init(void)
{
    /* The ping target is the gateway */
    SET_IPV4_ADDRESS(ping_target_ip, BING_Default);
    //wiced_ip_get_gateway_address( WICED_STA_INTERFACE, &ping_target_ip );

    /* Setup a regular ping event and setup the callback to run in the networking worker thread */
    wiced_rtos_register_timed_event( &ping_timed_event, WICED_NETWORKING_WORKER_THREAD, &send_ping, PING_PERIOD_MS, 0 );

    wiced_rtos_create_thread( &heart_t, 3, "heart_t", heart_callback, 4096, 0 );
}
