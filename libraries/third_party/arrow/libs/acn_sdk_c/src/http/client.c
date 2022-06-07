/* Copyright (c) 2017 Arrow Electronics, Inc.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Apache License 2.0
 * which accompanies this distribution, and is available at
 * http://apache.org/licenses/LICENSE-2.0
 * Contributors: Arrow Electronics, Inc.
 */

#define MODULE_NAME "HTTP_Client"

#include "http/client.h"

#define USE_STATIC
#include <data/chunk.h>

#include <debug.h>
#include <bsd/socket.h>
#include <time/time.h>

#include <ssl/ssl.h>

#if !defined(MAX_BUFFER_SIZE)
#define MAX_BUFFER_SIZE 1024
#endif

#define CHUNK_SIZE RINGBUFFER_SIZE

uint8_t tmpbuffer[CHUNK_SIZE];
#define MAX_TMP_BUF_SIZE (sizeof(tmpbuffer)-1)

void http_session_close_set(http_client_t *cli, bool mode) {
#if defined(HTTP_SOCK_KEEP_OPEN)
    mode = false;
#endif
  cli->flags._close = mode;
}

bool http_session_close(http_client_t *cli) {
  return cli->flags._close;
}

#define CHECK_CONN_ERR(ret) \
    if ( ret < 0 ) { \
      DBG("Connection error [%d] (%d)", __LINE__, ret); \
      return ret; \
    }

#define PRTCL_ERR() \
  { \
    DBG("Protocol error"); \
    return -1; \
  }

#define client_send(cli)                    (*(cli->_w_func))((cli), NULL, 0)
#define client_send_direct(cli, buf, size)  (*(cli->_w_func))((cli), (uint8_t*)(buf), (size))
#define client_recv(cli, size)              (*(cli->_r_func))((cli), NULL, (size))

static int simple_read(void *c, uint8_t *buf, uint16_t len) {
    SSP_PARAMETER_NOT_USED(buf);
    CREATE_CHUNK(tmp, MAX_TMP_BUF_SIZE);
    http_client_t *cli = (http_client_t *)c;
    if ( len > ringbuf_capacity(cli->queue) )
        len = ringbuf_capacity(cli->queue);
    int ret = recv(cli->sock, tmp, len, 0);
    if ( ret > 0 ) {
        if ( ringbuf_push(cli->queue, (uint8_t*)tmp, ret) < 0 ) {
            FREE_CHUNK(tmp);
            return -1;
        }
    }
    HTTP_DBG("%d|%s|", ret, tmp);
    FREE_CHUNK(tmp);
    return ret;
}

static int simple_write(void *c, uint8_t *buf, uint16_t len) {
    CREATE_CHUNK(tmp, MAX_TMP_BUF_SIZE);
    http_client_t *cli = (http_client_t *)c;
    if ( !buf ) {
        if ( !len ) len = ringbuf_size(cli->queue);
        if ( ringbuf_pop(cli->queue, (uint8_t*)tmp, len) < 0 )
            return -1;
        buf = (uint8_t*)tmp;
    } else {
        if ( !len ) len = strlen((char*)buf);
    }
    HTTP_DBG("%d|%s|", buf, len);
    int ret = send(cli->sock, buf, len, 0);
    FREE_CHUNK(tmp);
    return ret;
}

static int ssl_read(void *c, uint8_t *buf, uint16_t len) {
    SSP_PARAMETER_NOT_USED(buf);
    CREATE_CHUNK(tmp, MAX_TMP_BUF_SIZE);
    http_client_t *cli = (http_client_t *)c;
    if ( len > ringbuf_capacity(cli->queue) )
        len = ringbuf_capacity(cli->queue);
    int ret = ssl_recv(cli->sock, tmp, (int)len);
    if ( ret > 0 ) {
        if ( ringbuf_push(cli->queue, (uint8_t*)tmp, ret) < 0 ) {
            FREE_CHUNK(tmp);
            return -1;
        }
    }
    HTTP_DBG("[%d]{%s}", ret, tmp);
    FREE_CHUNK(tmp);
    return ret;
}

static int ssl_write(void *c, uint8_t *buf, uint16_t len) {
    CREATE_CHUNK(tmp, MAX_TMP_BUF_SIZE);
    http_client_t *cli = (http_client_t *)c;
    if ( !buf ) {
        if ( !len ) len = ringbuf_size(cli->queue);
        if ( ringbuf_pop(cli->queue, (uint8_t*)tmp, len) < 0 )
            return -1;
        buf = (uint8_t*)tmp;
    } else {
        if ( !len ) len = strlen((char*)buf);
    }
    HTTP_DBG("[%d]|%s|", len, buf);
    int ret = ssl_send(cli->sock, (char*)buf, (int)len);
    FREE_CHUNK(tmp);
    return ret;
}

void http_client_init(http_client_t *cli) {
    cli->response_code = 0;
    if ( !cli->queue ) {
        cli->queue = (ring_buffer_t *)malloc(sizeof(ring_buffer_t));
        int ret = ringbuf_init(cli->queue, RINGBUFFER_SIZE);
        if ( ret < 0 ) {
            DBG("HTTP: ringbuffer init failed %d", ret);
        }
    }
    if ( cli->flags._new ) {
        cli->sock = -1;
        cli->timeout = DEFAULT_API_TIMEOUT;
#if defined(HTTP_CIPHER)
        cli->_r_func = simple_read;
        cli->_w_func = simple_write;
#else
        cli->_r_func = simple_read;
        cli->_w_func = simple_write;
#endif
    }
}

void http_client_free(http_client_t *cli) {
  if ( cli->flags._close ) {
    if ( cli->sock >= 0 ) {
#if defined(HTTP_CIPHER)
      ssl_close(cli->sock);
#endif
      soc_close(cli->sock);
    }
    ringbuf_free(cli->queue);
    free(cli->queue);
    cli->queue = NULL;
    cli->flags._new = 1;
  } else {
    cli->flags._new = 0;
    ringbuf_clear(cli->queue);
  }
}

#define HTTP_VERS " HTTP/1.1\r\n"
static int send_start(http_client_t *cli, http_request_t *req, ring_buffer_t *buf) {
    ringbuf_clear(buf);
    int ret = snprintf((char*)tmpbuffer, MAX_TMP_BUF_SIZE,
                       "%s %s",
                       P_VALUE(req->meth), P_VALUE(req->uri));
    if ( ret < 0 ) return ret;
    if ( ringbuf_push(cli->queue, tmpbuffer, ret) < 0 ) return -1;
    if ( req->query ) {
        char *queryString = (char*)tmpbuffer;
        strcpy(queryString, "?");
        property_map_t *query = NULL;
        arrow_linked_list_for_each(query, req->query, property_map_t) {
          if ( (int)strlen(P_VALUE(query->key)) +
               (int)strlen(P_VALUE(query->value)) + 3 >
               (int)ringbuf_capacity(cli->queue) ) break;
            strcat(queryString, P_VALUE(query->key));
            strcat(queryString, "=");
            strcat(queryString, P_VALUE(query->value));
            if ( ringbuf_capacity(buf) - sizeof(HTTP_VERS)-1 < strlen(queryString) ) break;
            if ( ringbuf_push(buf, (uint8_t*)queryString, 0) < 0 )
                break;
            strcpy(queryString, "&");
        }
    }
    ret = ringbuf_push(buf, (uint8_t*)HTTP_VERS, 0);
    if ( ret < 0 ) return ret;
    if ( (ret = client_send(cli)) < 0 ) {
        return ret;
    }
    ret = snprintf((char*)tmpbuffer, MAX_TMP_BUF_SIZE, "Host: %s:%d\r\n", P_VALUE(req->host), req->port);
    if ( ret < 0 ) return ret;
    if ( ringbuf_push(cli->queue, tmpbuffer, ret) < 0) return -1;
    if ( (ret = client_send(cli)) < 0 ) {
        return ret;
    }
    return 0;
}

static int send_header(http_client_t *cli, http_request_t *req, ring_buffer_t *buf) {
    int ret;
    ringbuf_clear(buf);
    if ( !IS_EMPTY(req->payload.buf) && req->payload.size > 0 ) {
        if ( req->is_chunked ) {
            ret = client_send_direct(cli, "Transfer-Encoding: chunked\r\n", 0);
        } else {
            ret = snprintf((char*)tmpbuffer,
                           ringbuf_capacity(cli->queue),
                           "Content-Length: %lu\r\n", (long unsigned int)req->payload.size);
            if ( ret < 0 ) return ret;
            if ( ringbuf_push(cli->queue, tmpbuffer, ret) < 0 )
                return -1;
            ret = client_send(cli);
        }
        ringbuf_clear(buf);
        if ( ret < 0 ) return ret;
        ret = snprintf((char*)tmpbuffer,
                       ringbuf_capacity(cli->queue),
                       "Content-Type: %s\r\n", P_VALUE(req->content_type.value));
        if ( ringbuf_push(cli->queue, tmpbuffer, ret) < 0 ) return -1;
        if ( ret < 0 ) return ret;
        if ( (ret = client_send(cli)) < 0 ) return ret;
    }
    property_map_t *head = NULL;
    arrow_linked_list_for_each(head, req->header, property_map_t) {
        ringbuf_clear(buf);
        ret = snprintf((char*)tmpbuffer,
                           ringbuf_capacity(cli->queue),
                           "%s: %s\r\n", P_VALUE(head->key), P_VALUE(head->value));
    	if ( ret < 0 ) return ret;
        if ( ringbuf_push(cli->queue, tmpbuffer, ret) < 0 ) return -1;
        if ( (ret = client_send(cli)) < 0 ) return ret;
    }
    return client_send_direct(cli, "\r\n", 2);
}

static int send_payload(http_client_t *cli, http_request_t *req) {
    if ( !IS_EMPTY(req->payload.buf) && req->payload.size > 0 ) {
        if ( req->is_chunked ) {
            char *data = P_VALUE(req->payload.buf);
            int len = (int)req->payload.size;
            int trData = 0;
            while ( len >= 0 ) {
                char buf[6];
                int chunk = len > CHUNK_SIZE ? CHUNK_SIZE : len;
                int ret = sprintf(buf, "%02X\r\n", chunk);
                client_send_direct(cli, buf, ret);
                if ( chunk ) client_send_direct(cli, data + trData, chunk);
                trData += chunk;
                len -= chunk;
                client_send_direct(cli, "\r\n", 2);
                if ( !chunk ) break;
            }
            return 0;
        } else {
          int ret = client_send_direct(cli, P_VALUE(req->payload.buf), req->payload.size);
          return ret;
        }
    }
    return -1;
}

static uint8_t *wait_line(http_client_t *cli, uint8_t *buf) {
  int size = 0;
  while ( size < 2 || strncmp((char*)buf + size - 2, "\r\n", 2) != 0 ) {
      // pop from buffer by one symbol
      if ( ringbuf_pop(cli->queue, buf + size, 1) == 0 ) {
          size ++;
          buf[size] = 0x0;
      } else {
          // empty buffer
          if( ringbuf_capacity(cli->queue) > LINE_CHUNK ) {
              int ret = client_recv( cli, LINE_CHUNK );
              if ( ret < 0 ) return NULL;
          } else {
              return NULL;
          }
      }
  }
  return buf + size - 2;
}

static int receive_response(http_client_t *cli, http_response_t *res) {
    ringbuf_clear(cli->queue);
    CREATE_CHUNK(tmp, RINGBUFFER_SIZE);
    uint8_t *crlf = NULL;
    do {
        crlf = wait_line(cli, (uint8_t*)tmp);
        if ( !crlf ) {
            DBG("couldn't wait end of a line");
            FREE_CHUNK(tmp);
            return -1;
        }
        if( (crlf - (uint8_t *)tmp) < 10 ) {
            // too short line: sizeof(HTTP/1.1)
            *crlf = 0x0;
            crlf = NULL;
        }
    } while ( !crlf );
    *crlf = 0x0;
    DBG("resp: {%s}", tmp);
    if( sscanf((char*)tmp, "HTTP/1.1 %4d", &res->m_httpResponseCode) != 1 ) {
        DBG("Not a correct HTTP answer : %s", tmp);
        FREE_CHUNK(tmp);
        return -1;
    }

    DBG("Response code %d", res->m_httpResponseCode);
    cli->response_code = res->m_httpResponseCode;
    FREE_CHUNK(tmp);
    return 0;
}

static int receive_headers(http_client_t *cli, http_response_t *res) {
    uint8_t *crlf = NULL;
    CREATE_CHUNK(tmp, RINGBUFFER_SIZE);
    CREATE_CHUNK(key, CHUNK_SIZE>>2);
    CREATE_CHUNK(value, CHUNK_SIZE);
    int ret = 0;
    while( ( crlf = wait_line(cli, (uint8_t*)tmp) ) ) {
        if ( crlf == (uint8_t*)tmp ) {
            HTTP_DBG("Headers read done");
            ret = 0;
            break;
        }

        int n = sscanf((char*)tmp, "%256[^:]: %256[^\r\n]", key, value);
        if ( n == 2 ) {
            HTTP_DBG("Read header : %s: %s", key, value);
            if( !strcmp(key, "Content-Length") ) {
                sscanf(value, "%8d", (int *)&res->recvContentLength);
            } else if( !strcmp(key, "Transfer-Encoding") ) {
                if( !strcmp(value, "Chunked") || !strcmp(value, "chunked") )
                    res->is_chunked = 1;
            } else if( !strcmp(key, "Content-Type") ) {
                http_response_set_content_type(res, property(value, is_stack));
            } else {
#if defined(HTTP_PARSE_HEADER)
                http_response_add_header(res,
                                         p_stack(key),
                                         p_stack(value));
#endif
            }
        } else {
            DBG("Could not parse header");
            ret = -1;
            goto recv_header_end;
        }
    }
recv_header_end:
    FREE_CHUNK(tmp);
    FREE_CHUNK(key);
    FREE_CHUNK(value);
    return ret;
}

static int get_chunked_payload_size(http_client_t *cli, http_response_t *res) {
    // find the \r\n in the payload
    // next string shoud start at HEX chunk size
    SSP_PARAMETER_NOT_USED(res);
    CREATE_CHUNK(tmp, RINGBUFFER_SIZE);
    int chunk_len = 0;
    uint8_t *crlf = wait_line(cli, (uint8_t*)tmp);
    if ( !crlf ) return -1; // no \r\n - wrong string
    int ret = sscanf((char*)tmp, "%4x\r\n", (unsigned int*)&chunk_len);
    if ( ret != 1 ) {
        // couldn't read a chunk size - fail
        chunk_len = 0;
        FREE_CHUNK(tmp);
        return -1;
    }
    HTTP_DBG("detect chunk %d", chunk_len);
    FREE_CHUNK(tmp);
    return chunk_len;
}

static int receive_payload(http_client_t *cli, http_response_t *res) {
    int chunk_len = 0;
    int no_data_error = 0;
    do {
        if ( res->is_chunked ) {
            chunk_len = get_chunked_payload_size(cli, res);
        } else {
            chunk_len = res->recvContentLength;
            DBG("Con-Len %d", res->recvContentLength);
        }
        if ( !chunk_len || chunk_len < 0 ) break;
        while ( chunk_len ) {
            uint32_t need_to_read = (chunk_len < CHUNK_SIZE-10) ? chunk_len : CHUNK_SIZE-10;
            HTTP_DBG("need to read %d", need_to_read);
            while ( (int)ringbuf_size(cli->queue) < (int)need_to_read ) {
                HTTP_DBG("get chunk add %d", need_to_read-ringbuf_size(cli->queue));
                int ret = client_recv(cli, need_to_read-ringbuf_size(cli->queue));
                if ( ret < 0 ) {
                    // ret < 0 - error
                    DBG("No data");
                    if ( no_data_error ++ > 2) return -1;
                }
            }
            HTTP_DBG("add payload{%d:s}", need_to_read);//, buf);
            if ( ringbuf_pop(cli->queue, tmpbuffer, need_to_read) < 0 ) return -1;
            if ( http_response_add_payload(res, p_stack(tmpbuffer), need_to_read) < 0 ) {
                ringbuf_clear(cli->queue);
                DBG("Payload is failed");
                return -1;
            }
            chunk_len -= need_to_read;
            HTTP_DBG("%d %d", chunk_len, need_to_read);
        }
        if ( !res->is_chunked ) break;
        else {
          uint8_t *crlf = wait_line(cli, tmpbuffer);
          if ( !crlf ) {
              DBG("No new line");
          }
        }
    } while(1);

    HTTP_DBG("body{%s}", P_VALUE(res->payload.buf));
    return 0;
}

int http_client_do(http_client_t *cli, http_request_t *req, http_response_t *res) {
    int ret;
    http_response_init(res, &req->_response_payload_meth);
    if ( cli->sock < 0 ) {
        DBG("new TCP connection");
        ret = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
        if ( ret < 0 ) return ret;
        ringbuf_clear(cli->queue);
        cli->sock = ret;
        // resolve the host
    	struct sockaddr_in serv;
    	struct hostent *serv_resolve;
        serv_resolve = gethostbyname(P_VALUE(req->host));
    	if (serv_resolve == NULL) {
    		DBG("ERROR, no such host");
    		return -1;
    	}
    	memset(&serv, 0, sizeof(serv));
    	serv.sin_family = PF_INET;
    	bcopy((char *)serv_resolve->h_addr,
    			(char *)&serv.sin_addr.s_addr,
				(uint32_t)serv_resolve->h_length);
    	serv.sin_port = htons(req->port);

        // set timeout
    	struct timeval tv;
    	tv.tv_sec =     (time_t)        ( cli->timeout / 1000 );
    	tv.tv_usec =    (suseconds_t)   (( cli->timeout % 1000 ) * 1000);
    	setsockopt(cli->sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof(struct timeval));

    	ret = connect(cli->sock, (struct sockaddr*)&serv, sizeof(serv));
    	if ( ret < 0 ) {
    		DBG("connect fail");
    		soc_close(cli->sock);
    		cli->sock = -1;
    		return -1;
    	}
    	HTTP_DBG("connect done");
    	if ( req->is_cipher ) {
    		if ( ssl_connect(cli->sock) < 0 ) {
    			HTTP_DBG("SSL connect fail");
    			ssl_close(cli->sock);
    			soc_close(cli->sock);
    			cli->sock = -1;
    			return -1;
    		}
    		cli->_r_func = ssl_read;
    		cli->_w_func = ssl_write;
    	} else {
    		cli->_r_func = simple_read;
    		cli->_w_func = simple_write;
    	}
    }

    if ( send_start(cli, req, cli->queue) < 0 ) {
        DBG("send start fail");
        return -1;
    }

    if ( send_header(cli, req, cli->queue) < 0 ) {
        DBG("send header fail");
        return -1;
    }

    if ( !IS_EMPTY(req->payload.buf) ) {
        if ( send_payload(cli, req) < 0 ) {
            DBG("send payload fail");
            return -1;
        }
    }

    HTTP_DBG("Receiving response");

    ringbuf_clear(cli->queue);

    ret = receive_response(cli, res);
    if ( ret < 0 ) {
        DBG("Receiving error (%d)", ret);
        return -1;
    }

    HTTP_DBG("Reading headers");
    res->header = NULL;
    memset(&res->content_type, 0x0, sizeof(property_map_t));
    memset(&res->payload, 0x0, sizeof(http_payload_t));
    res->is_chunked = 0;
    res->processed_payload_chunk = 0;

    //Now let's get a headers
    ret = receive_headers(cli, res);
    if ( ret < 0 ) {
        DBG("Receiving headers error (%d)", ret);
        return -1;
    }

    ret = receive_payload(cli, res);
    if ( ret < 0 ) {
        DBG("Receiving payload error (%d)", ret);
        return -1;
    }
    return 0;
}
