#include "wiced.h"
#include "uart.h"
#include "uart_core.h"

wiced_ring_buffer_t rx_buffer;
wiced_thread_t uart_recv_t = NULL;
wiced_thread_t uart_serv_t = NULL;
EventGroupHandle_t Config_EventHandler;
EventGroupHandle_t Info_EventHandler;
EventGroupHandle_t C2D_EventHandler;
EventGroupHandle_t TEM_EventHandler;

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
    printf("uart_recv_thread created\n");
    char value;
    uint32_t expected_data_size = 1;

    while ( wiced_uart_receive_bytes( Control_UART, &value, &expected_data_size, WICED_NEVER_TIMEOUT ) == WICED_SUCCESS )
    {
        uart_receive_input(value);
        expected_data_size = 1;
    }
}
void uart_serv_callback( uint32_t arg )
{
    printf("uart_serv_thread created\n");
    while(1)
    {
        wifi_uart_service();
        wiced_rtos_delay_milliseconds(10);
    }
}
void uart_init(void)
{
    Config_EventHandler = xEventGroupCreate();
    Info_EventHandler = xEventGroupCreate();
    C2D_EventHandler = xEventGroupCreate();
    TEM_EventHandler = xEventGroupCreate();
    ring_buffer_init(&rx_buffer, rx_data, 1024 );
    wiced_uart_init( Control_UART, &uart_config, &rx_buffer );
    wifi_protocol_init();
    wiced_rtos_create_thread( &uart_recv_t, 1, "uart_recv", uart_recv_callback, 1024, 0 );
    wiced_rtos_create_thread( &uart_recv_t, 5, "uart_serv", uart_serv_callback, WICED_DEFAULT_APPLICATION_STACK_SIZE, 0 );
}
