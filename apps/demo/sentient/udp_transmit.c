#include "wiced.h"
#include "udp_transmit.h"
#include "wifi_paser.h"

#define UDP_MAX_DATA_LENGTH         1000
#define UDP_RX_TIMEOUT              1
#define UDP_TX_INTERVAL             1
#define UDP_RX_INTERVAL             1
#define UDP_RECV_PORT               50008
#define UDP_SEND_PORT               50006
#define UDP_TARGET_IS_BROADCAST
#define GET_UDP_RESPONSE

#ifdef UDP_TARGET_IS_BROADCAST
#define UDP_TARGET_IP MAKE_IPV4_ADDRESS(192,168,0,255)
#else
#define UDP_TARGET_IP MAKE_IPV4_ADDRESS(192,168,0,2)
#endif


wiced_udp_socket_t  udp_socket;
wiced_timed_event_t udp_tx_event;

wiced_thread_t *udp_rx_thread;
void udp_start(wiced_interface_t interface)
{
    //wiced_rtos_create_thread(udp_rx_thread,5,"rx",rx_udp_packet,1024,NULL);
    //wiced_rtos_thread_join(udp_rx_thread);
    if (wiced_udp_create_socket(&udp_socket, UDP_RECV_PORT, interface) != WICED_SUCCESS)
    {
        WPRINT_APP_INFO( ("UDP send socket creation failed\n") );
    }
    else
    {
        WPRINT_APP_INFO( ("UDP send socket creation success\n") );
    }
}
wiced_result_t udp_packet_send (char* buffer, uint16_t buffer_length)
{
    wiced_packet_t*          packet;
    char*                    tx_data;
    uint16_t                 available_data_length;
    const wiced_ip_address_t INITIALISER_IPV4_ADDRESS( target_ip_addr, UDP_TARGET_IP );

    /* Create the UDP packet. Memory for the TX data is automatically allocated */
    if ( wiced_packet_create_udp( &udp_socket, buffer_length, &packet, (uint8_t**) &tx_data, &available_data_length ) != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ("UDP tx packet creation failed\n") );
        return WICED_ERROR;
    }

    /* Copy buffer into tx_data which is located inside the UDP packet */
    memcpy( tx_data, buffer, buffer_length + 1 );

    /* Set the end of the data portion of the packet */
    wiced_packet_set_data_end( packet, (uint8_t*) tx_data + buffer_length );

    /* Send the UDP packet */
    if ( wiced_udp_send( &udp_socket, &target_ip_addr, UDP_SEND_PORT, packet ) != WICED_SUCCESS )
    {
        WPRINT_APP_INFO( ("UDP packet send failed\n") );
        /* Delete packet, since the send failed */
        wiced_packet_delete( packet );
    }
    else
    {
        printf( "UDP Send : %s\r\n", tx_data);
    }

    /*
     * NOTE : It is not necessary to delete the packet created above, the packet
     *        will be automatically deleted *AFTER* it has been successfully sent
     */

    return WICED_SUCCESS;
}
/*
 * Attempts to receive a UDP packet
 */
wiced_result_t rx_udp_packet(uint32_t timeout)
{
    wiced_packet_t* packet;
    char*           udp_data;
    uint16_t        data_length;
    uint16_t        available_data_length;
    /* Wait for UDP packet */
    wiced_result_t result = wiced_udp_receive( &udp_socket, &packet, timeout );

    if ( ( result == WICED_ERROR ) || ( result == WICED_TIMEOUT ) )
    {
        return result;
    }

    wiced_packet_get_data( packet, 0, (uint8_t**) &udp_data, &data_length, &available_data_length );

    if (data_length != available_data_length)
    {
        WPRINT_APP_INFO(("Fragmented packets not supported\n"));
        return WICED_ERROR;
    }

    /* Null terminate the received string */
    udp_data[ data_length ] = '\x0';

    WPRINT_APP_INFO( ("Got Data is %s\n\n", udp_data) );

    wifi_decoder(udp_data);

    /* Delete packet as it is no longer needed */
    wiced_packet_delete( packet );

    return WICED_SUCCESS;
}
