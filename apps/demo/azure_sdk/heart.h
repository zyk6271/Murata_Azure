#ifndef HEART_H
#define HEART_H

uint32_t sta_rssi_get(void);
void wifi_status_change(uint8_t value);
void wifi_restart_release(void);
void mqtt_disconnect_callabck(void);
void mqtt_connect_callabck(void);
void wifi_disconnect_callback(void);
void wifi_connect_callback(void);
#endif
