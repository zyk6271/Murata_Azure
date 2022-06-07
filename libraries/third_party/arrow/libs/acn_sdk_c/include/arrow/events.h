#if !defined(ARROW_EVENTS_H_) && !defined(NO_EVENTS)
#define ARROW_EVENTS_H_

#include <json/json.h>
#include <data/linkedlist.h>

typedef struct {
  char *gateway_hid;
  char *device_hid;
  char *name;
  int encrypted;
  char *cmd;
  JsonNode *parameters;
  arrow_linked_list_head_node;
} mqtt_event_t;

void arrow_mqtt_events_init(void);
void arrow_mqtt_events_done(void);
int process_event(const char *str);
int arrow_mqtt_has_events(void);
int arrow_mqtt_event_proc(void);

#define MAX_PARAM_LINE 20

#endif // ARROW_EVENTS_H_
