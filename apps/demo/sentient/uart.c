#include "wiced.h"
#include "control_paser.h"
#include "uart.h"
#define RX_BUFFER_SIZE    64
wiced_uart_config_t uart_config =
{
    .baud_rate    = 115200,
    .data_width   = DATA_WIDTH_8BIT,
    .parity       = NO_PARITY,
    .stop_bits    = STOP_BITS_1,
    .flow_control = FLOW_CONTROL_DISABLED,
};

wiced_ring_buffer_t rx_buffer;
DEFINE_RING_BUFFER_DATA( uint8_t, rx_data, RX_BUFFER_SIZE)

#define Control_UART 0

char first = 0x55;
char end = 0xAA;

void Control_Uart_Init(void)
{

    /* Initialise ring buffer */
    ring_buffer_init(&rx_buffer, rx_data, RX_BUFFER_SIZE );

    /* Initialise UART. A ring buffer is used to hold received characters */
    wiced_uart_init( Control_UART, &uart_config, &rx_buffer );

    /* Send a test string to the terminal */
    //wiced_uart_transmit_bytes( Control_UART, TEST_STR, sizeof( TEST_STR ) - 1 );

    printf("UART Init Success\r\n");
    /* Wait for user input. If received, echo it back to the terminal */
}
void uart_send(uint8_t command,uint8_t value)
{
    wiced_uart_transmit_bytes( Control_UART, &first, 1);
    wiced_uart_transmit_bytes( Control_UART, &command, 1);
    wiced_uart_transmit_bytes( Control_UART, &value, 1);
    wiced_uart_transmit_bytes( Control_UART, &end, 1);
}
void uart_parse(uint8_t *buf)
{
    for(uint8_t i = 0;i<5;i++)
    {
        printf("Control_Uart_Work %d is %X\r\n",i,buf[i]);
    }
    if(buf[0]==0x55 && buf[4] == 0xAA)
    {
        control_encode(buf[1],(buf[2]<<8)+buf[3]);
    }
}
void Control_Uart_Work(uint32_t timeout)
{
    char buf[10];
    uint32_t expected_data_size = 5;
    while ( wiced_uart_receive_bytes( Control_UART, buf, &expected_data_size, timeout ) == WICED_SUCCESS )
    {
        expected_data_size = 5;
        uart_parse(buf);
    }
}

