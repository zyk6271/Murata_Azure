#ifndef MQTT_HANDLE_H
#define MQTT_HANDLE_H

void mqtt_init(void);
void mqtt_print_status( wiced_result_t result, const char * ok_message, const char * error_message );
wiced_result_t mqtt_connection_event_cb( wiced_mqtt_object_t mqtt_object, wiced_mqtt_event_info_t *event );
wiced_result_t mqtt_wait_for( wiced_mqtt_event_type_t event, uint32_t timeout );
wiced_result_t mqtt_conn_open( wiced_mqtt_object_t mqtt_obj, wiced_ip_address_t *address, wiced_interface_t interface, wiced_mqtt_callback_t callback, wiced_mqtt_security_t *security );
wiced_result_t mqtt_app_subscribe( wiced_mqtt_object_t mqtt_obj, char *topic, uint8_t qos );
wiced_result_t mqtt_app_publish( wiced_mqtt_object_t mqtt_obj, uint8_t qos, char *topic, uint8_t *data, uint32_t data_len );

#endif
