/* Copyright (c) 2017 Arrow Electronics, Inc.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Apache License 2.0
 * which accompanies this distribution, and is available at
 * http://apache.org/licenses/LICENSE-2.0
 * Contributors: Arrow Electronics, Inc.
 */

#include "arrow/device.h"
#include <sys/mem.h>
#include <config.h>

void arrow_device_init(arrow_device_t *dev) {
    dev->info = NULL;
    dev->prop = NULL;
    P_CLEAR(dev->name);
    P_CLEAR(dev->type);
    P_CLEAR(dev->uid);
    P_CLEAR(dev->gateway_hid);
    P_CLEAR(dev->hid);
    property_init(&dev->softwareName);
    property_init(&dev->softwareVersion);
#if defined(__IBM__)
    P_CLEAR(dev->eid);
#endif
}

void arrow_device_free(arrow_device_t *dev) {
  P_FREE(dev->name);
  P_FREE(dev->type);
  P_FREE(dev->uid);
  P_FREE(dev->gateway_hid);
  P_FREE(dev->hid);
  property_free(&dev->softwareName);
  property_free(&dev->softwareVersion);
  if ( dev->info ) json_delete(dev->info);
  if ( dev->prop ) json_delete(dev->prop);
#if defined(__IBM__)
  P_FREE(dev->eid);
#endif
}

void arrow_device_add_info(arrow_device_t *dev, const char *key, const char *value) {
  if ( !dev->info) dev->info = json_mkobject();
  json_append_member(dev->info, key, json_mkstring(value));
}

void arrow_device_add_property(arrow_device_t *dev, const char *key, const char *value) {
  if ( !dev->prop ) dev->prop = json_mkobject();
  json_append_member(dev->prop, key, json_mkstring(value));
}

char *arrow_device_serialize(arrow_device_t *dev) {
  JsonNode *_main = json_mkobject();
  json_append_member(_main, "name", json_mkstring(P_VALUE(dev->name)));
  json_append_member(_main, "type", json_mkstring(P_VALUE(dev->type)));
  json_append_member(_main, "uid", json_mkstring(P_VALUE(dev->uid)));
  json_append_member(_main, "gatewayHid", json_mkstring(P_VALUE(dev->gateway_hid)));
  json_append_member(_main, "softwareName", json_mkstring(P_VALUE(dev->softwareName)));
  json_append_member(_main, "softwareName", json_mkstring(P_VALUE(dev->softwareVersion)));
  if ( dev->info ) json_append_member(_main, "info", dev->info);
  if ( dev->prop ) json_append_member(_main, "properties", dev->prop);
  char *dev_str = json_encode(_main);
  if ( dev->info ) json_remove_from(_main, dev->info);
  if ( dev->prop ) json_remove_from(_main, dev->prop);
  json_delete(_main);
  return dev_str;
}

int arrow_device_parse(arrow_device_t *dev, const char *str) {
    JsonNode *_main = json_decode(str);
    if ( !_main ) return -1;
    JsonNode *hid = json_find_member(_main, "hid");
    if ( !hid || hid->tag != JSON_STRING ) return -1;
    P_COPY(dev->hid, p_stack(hid->string_));
#if defined(__IBM__)
    JsonNode *eid = json_find_member(_main, "externalId");
    if ( !eid || eid->tag != JSON_STRING ) return -1;
    P_COPY(dev->eid, p_stack(eid->string_));
#endif
    json_delete(_main);
    return 0;
}

int arrow_prepare_device(arrow_gateway_t *gateway, arrow_device_t *device) {
  arrow_device_init(device);
  P_COPY(device->gateway_hid, p_const(P_VALUE(gateway->hid)) ); // FIXME weak pointer
  P_COPY(device->name, p_const(DEVICE_NAME));
  P_COPY(device->type, p_const(DEVICE_TYPE));
  property_copy(&device->softwareName, p_const(DEVICE_SOFTWARE_NAME));
  property_copy(&device->softwareVersion, p_const(DEVICE_SOFTWARE_VERSION));
  if ( IS_EMPTY(gateway->uid) ) return -1;
  char *uid = (char*)malloc(P_SIZE(gateway->uid)+sizeof(DEVICE_UID_SUFFIX)+2);
  strcpy(uid, P_VALUE(gateway->uid) );
  strcat(uid, "-");
  strcat(uid, DEVICE_UID_SUFFIX);
  P_COPY(device->uid, p_heap(uid));
  return 0;
}
