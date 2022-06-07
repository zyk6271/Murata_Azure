#include "wiced.h"
wiced_result_t rx_udp_packet(uint32_t timeout);
wiced_result_t udp_packet_send (char* buffer, uint16_t buffer_length);
void udp_start(wiced_interface_t interface);
