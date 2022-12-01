#include "wiced.h"
#include "uart.h"
#include "uart_core.h"
#include "system.h"

wiced_ring_buffer_t rx_buffer;
wiced_thread_t uart_recv_t;
wiced_thread_t uart_serv_t;

wiced_event_flags_t Config_EventHandler;
wiced_event_flags_t Info_EventHandler;
wiced_event_flags_t C2D_EventHandler;
wiced_event_flags_t TEM_EventHandler;

DEFINE_RING_BUFFER_DATA( uint8_t, rx_data, 1024)

wiced_uart_config_t uart_config =
{
    .baud_rate    = 115200,
    .data_width   = DATA_WIDTH_8BIT,
    .parity       = NO_PARITY,
    .stop_bits    = STOP_BITS_1,
    .flow_control = FLOW_CONTROL_DISABLED,
};
void uart_recv_callback( uint32_t arg )
{
    char value;
    uint32_t expected_data_size = 1;

    while ( 1 )
    {
        wiced_uart_receive_bytes( Control_UART, &value, &expected_data_size, WICED_NEVER_TIMEOUT );
        uart_receive_input(value);
        expected_data_size = 1;
    }
}
void uart_serv_callback( uint32_t arg )
{
    product_info_request();
    while(1)
    {
        wifi_uart_service();
        wiced_rtos_delay_milliseconds(10);
    }
}
void uart_init(void)
{
    wiced_rtos_init_event_flags(&Config_EventHandler);
    wiced_rtos_init_event_flags(&Info_EventHandler);
    wiced_rtos_init_event_flags(&C2D_EventHandler);
    wiced_rtos_init_event_flags(&TEM_EventHandler);
    ring_buffer_init(&rx_buffer, rx_data, 1024 );
    wiced_uart_init( Control_UART, &uart_config, &rx_buffer );
    wifi_protocol_init();
    wiced_rtos_create_thread( &uart_recv_t, 6, "uart_recv", uart_recv_callback, 2048, 0 );
    wiced_rtos_create_thread( &uart_serv_t, 7, "uart_serv", uart_serv_callback, 4096, 0 );
}
