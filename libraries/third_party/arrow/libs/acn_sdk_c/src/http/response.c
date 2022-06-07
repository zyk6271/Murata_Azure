/* Copyright (c) 2017 Arrow Electronics, Inc.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Apache License 2.0
 * which accompanies this distribution, and is available at
 * http://apache.org/licenses/LICENSE-2.0
 * Contributors: Arrow Electronics, Inc.
 */

#include "http/response.h"
#include <debug.h>
#include <config.h>
#include <sys/mem.h>
#include <arrow/utf8.h>

// default functions to process payload for the http response
int default_set_payload_handler(void *r,
                            property_t buf,
                            int size) {
  http_response_t *res = (http_response_t *)r;
  res->payload.size = size;
  P_COPY(res->payload.buf, buf);
  if ( IS_EMPTY(res->payload.buf) ) {
    DBG("[http] set_payload: fail");
  }
  return 0;
}

int default_add_payload_handler(void *r,
                            property_t payload,
                            int size) {
  http_response_t *res = (http_response_t *)r;
  if ( IS_EMPTY(res->payload.buf) ) {
    res->payload.size = size;
    res->payload.buf.value = (char*)malloc(size+1);
    memcpy(res->payload.buf.value, payload.value, size);
    P_VALUE(res->payload.buf)[res->payload.size] = '\0';
    res->payload.buf.flags = is_dynamic;
    return 0;
  } else {
    switch(res->payload.buf.flags) {
      case is_dynamic:
        res->payload.buf.value = realloc(res->payload.buf.value, res->payload.size + size + 1);
        if ( IS_EMPTY(res->payload.buf) ) {
          DBG("[add payload] out of memory ERROR");
          return 0;
        }
        memcpy(res->payload.buf.value + res->payload.size, payload.value, size);
        res->payload.size += size;
        P_VALUE(res->payload.buf)[res->payload.size] = '\0';
      break;
      default:
        // error
        return -1;
    }
    if ( payload.flags == is_dynamic ) P_FREE(payload);
  }
  return 0;
}

void http_response_init(http_response_t *res, _payload_meth_t *handler) {
  memset(res, 0x00, sizeof(http_response_t));
  res->_p_meth._p_set_handler = handler->_p_set_handler;
  res->_p_meth._p_add_handler = handler->_p_add_handler;
}

void http_response_free(http_response_t *res) {
    property_free(&res->payload.buf);
    res->payload.size = 0;
    property_map_clear(&res->header);
    property_free(&res->content_type.value);
    property_free(&res->content_type.key);
}

void http_response_add_header(http_response_t *req, property_t key, property_t value) {
    property_map_add(&req->header, key, value);
}

void http_response_set_content_type(http_response_t *res, property_t value) {
  res->content_type.key = property(CONTENT_TYPE, is_const);
  property_copy(&res->content_type.value, value);
}

void http_response_set_payload(http_response_t *res, property_t payload, uint32_t size) {
  if ( ! size ) size = P_SIZE(payload);
  res->_p_meth._p_set_handler(res, payload, size);
  res->processed_payload_chunk = 1;
}

int http_response_add_payload(http_response_t *res, property_t payload, uint32_t size) {
  if ( !size ) size = P_SIZE(payload);
  int ret = 0;
  ret = res->_p_meth._p_add_handler(res, payload, size);
  if ( ret < 0 ) return -1;
  res->processed_payload_chunk ++ ;
  return 0;
}
