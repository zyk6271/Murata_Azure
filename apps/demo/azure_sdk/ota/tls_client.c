/*
 *  Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 *  SPDX-License-Identifier: Apache-2.0
 *
 *  Licensed under the Apache License, Version 2.0 (the "License"); you may
 *  not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 *  WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 *
 *  This file is part of mbed TLS (https://tls.mbed.org)
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tls_client.h"
#include "tls_certificate.h"

#include "nxd_bsd.h"
#include "wiced_tcpip.h"

#if !defined(MBEDTLS_CONFIG_FILE)
#include "mbedtls/config.h"
#else
#include MBEDTLS_CONFIG_FILE
#endif

#define LOG_I
#define LOG_D   printf
#define LOG_E
#define tls_free free
#define RT_NULL NULL
/* RT-Thread error code definitions */
#define RT_EOK                          0               /**< There is no error */
#define RT_ERROR                        1               /**< A generic error happens */
#define RT_ETIMEOUT                     2               /**< Timed out */
#define RT_EFULL                        3               /**< The resource is full */
#define RT_EEMPTY                       4               /**< The resource is empty */
#define RT_ENOMEM                       5               /**< No memory */
#define RT_ENOSYS                       6               /**< No system */
#define RT_EBUSY                        7               /**< Busy */
#define RT_EIO                          8               /**< IO error */
#define RT_EINTR                        9               /**< Interrupted system call */
#define RT_EINVAL                       10              /**< Invalid argument */

#define host_name "cs110032001e4afc55e.blob.core.windows.net"

/* Keepalive will be sent every 2 seconds */
#define TCP_SERVER_KEEP_ALIVE_INTERVAL      (2)
/* Retry 10 times */
#define TCP_SERVER_KEEP_ALIVE_PROBES        (5)
/* Initiate keepalive check after 5 seconds of silence on a tcp socket */
#define TCP_SERVER_KEEP_ALIVE_TIME          (5)
#define TCP_SILENCE_DELAY                   (30)


static void _ssl_debug(void *ctx, int level, const char *file, int line, const char *str)
{
    ((void) level);

    LOG_D("%s:%04d: %s", file, line, str);
}

static int mbedtls_ssl_certificate_verify(MbedTLSSession *session)
{
    return RT_EOK;
}

int mbedtls_client_init(MbedTLSSession *session, void *entropy, size_t entropyLen)
{
    int ret = 0;

    wiced_result_t result;

    session->server_fd.fd = -1;

    if ( ( result = wiced_tcp_create_socket( &session->socket, WICED_STA_INTERFACE ) ) != WICED_SUCCESS )
    {
        return result;
    }

    wiced_tls_init_root_ca_certificates( mbedtls_root_certificate, mbedtls_root_certificate_len );

    wiced_tls_init_context( &session->tls_context, NULL,  host_name );

    wiced_tcp_enable_tls( &session->socket.socket, &session->tls_context );

    return RT_EOK;
}

int mbedtls_client_close(MbedTLSSession *session)
{
    if (session == RT_NULL)
    {
        return -RT_ERROR;
    }

    session->server_fd.fd = -1;

    wiced_tls_deinit_context( &session->tls_context );

    wiced_tcp_disconnect( &session->socket );

    wiced_tcp_delete_socket( &session->socket );

    wiced_tls_deinit_root_ca_certificates( );

    if (session->buffer)
    {
        tls_free(session->buffer);
    }

    if (session->host)
    {
        tls_free(session->host);
    }

    if(session->port)
    {
        tls_free(session->port);
    }

    tls_free(session);
    session = RT_NULL;
    
    return RT_EOK;
}

int mbedtls_client_context(MbedTLSSession *session)
{
    return RT_EOK;
}

int mbedtls_client_connect(MbedTLSSession *session)
{
    #ifndef WICED_DISABLE_TLS
    wiced_result_t result;
    #endif /* ifndef WICED_DISABLE_TLS */
    int ret = 0;
    wiced_ip_address_t        address;

    ret = wiced_hostname_lookup( host_name, &address, 5000, WICED_STA_INTERFACE );

    result = wiced_tcp_connect( &session->socket, &address, 443, 5000 );
    if ( result != WICED_SUCCESS )
    {
        return ( result );
    }
    else
    {
        session->server_fd.fd = 0;
        LOG_I("Connected %s:%s success,result is %d\r\n...", session->host, session->port,result);
    }

    return RT_EOK;
}

int mbedtls_client_read(MbedTLSSession *session, unsigned char *buf , size_t len)
{
    int ret = 0;
    unsigned char *recv_buf;

    if (session == RT_NULL || buf == RT_NULL)
    {
        return -RT_ERROR;
    }

    if( session->tls_context.context.defragmentation_buffer != NULL)
    {
        tls_host_free_defragmentation_buffer(session->tls_context.context.defragmentation_buffer);
        session->tls_context.context.defragmentation_buffer                 = NULL;
        session->tls_context.context.defragmentation_buffer_bytes_processed = 0;
        session->tls_context.context.defragmentation_buffer_bytes_received  = 0;
        session->tls_context.context.defragmentation_buffer_length          = 0;
        session->tls_context.context.defragmentation_buffer_bytes_skipped   = 0;
    }

    if ( session->tls_context.context.received_packet != NULL )
    {
        wiced_packet_delete((wiced_packet_t*) session->tls_context.context.received_packet);
        session->tls_context.context.received_packet = NULL;
    }

    session->tls_context.context.read_timeout = 3000;
    session->tls_context.context.packet_receive_option = TLS_RECEIVE_PACKET_IF_NEEDED;

    ret = mbedtls_ssl_user_read(&session->tls_context.context, &buf, len);
    if (ret != WICED_SUCCESS)
    {
        if ( session->tls_context.context.received_packet != NULL )
        {
            wiced_packet_delete((wiced_packet_t*) session->tls_context.context.received_packet);
        }
        session->tls_context.context.received_packet = NULL;
        session->tls_context.context.in_msg = NULL;
        session->tls_context.context.in_msglen = 0;
        session->tls_context.context.in_len = NULL;
        session->tls_context.context.in_offt = NULL;
        LOG_E("mbedtls_client_read data error, return -0x%x", -ret);
        free(recv_buf);
        return -1;
    }
    return len;
}
int mbedtls_client_write(MbedTLSSession *session, const unsigned char *buf , size_t len)
{
    int ret = 0;

    if (session == RT_NULL || buf == RT_NULL)
    {
        return -RT_ERROR;
    }

    if ( ( ret = wiced_tcp_send_buffer( &session->socket, buf, len ) != WICED_SUCCESS ) )
    {
        session->server_fd.fd = -1;
        wiced_tcp_disconnect( &session->socket );
        wiced_tcp_delete_socket( &session->socket );
        LOG_E("mbedtls_client_write data error, return -0x%x", -ret);
        return -RT_ERROR;
    }
    return len;
}
