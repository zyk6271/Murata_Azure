/* Copyright (c) 2017 Arrow Electronics, Inc.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Apache License 2.0
 * which accompanies this distribution, and is available at
 * http://apache.org/licenses/LICENSE-2.0
 * Contributors: Arrow Electronics, Inc.
 */

#ifndef ACN_SDK_C_MQTT_CONFIG_H_
#define ACN_SDK_C_MQTT_CONFIG_H_

#if !defined(DEFAULT_MQTT_TIMEOUT)
# define DEFAULT_MQTT_TIMEOUT 10000
#endif

#if defined(__IBM__) || \
  ( defined(__AZURE__) || !defined(DEV_ENV) )
# define MQTT_CIPHER
#endif

#if !defined(MQTT_BUF_LEN)
#define MQTT_BUF_LEN 1200
#endif

#if defined(MQTT_CIPHER)
#  define MQTT_SCH "tls"
#  define MQTT_PORT 8883
#else
# define MQTT_SCH "tcp"
# define MQTT_PORT 1883
#endif

#if defined(__IBM__)
#  define MQTT_ADDR ".messaging.internetofthings.ibmcloud.com"
#elif defined(__AZURE__)
#  define MQTT_ADDR "pgshubdev01.azure-devices.net"
#  define VHOST "iothubowner"
#else
# if defined(DEV_ENV)
#  define MQTT_ADDR "pgsdev01.arrowconnect.io"
#  define VHOST "/themis.dev:"
# else
#  define MQTT_ADDR "mqtt-a01.arrowconnect.io"
#  define VHOST "/pegasus:"
# endif
#endif

#define ARROW_MQTT_URL MQTT_SCH "://" MQTT_ADDR ":" #MQTT_PORT

#if !defined(MQTT_QOS)
// FIXME delete it for a next version
#if defined(__nrf52832__)
#define MQTT_QOS        1
#else
#define MQTT_QOS        0
#endif
#endif
#if !defined(MQTT_RETAINED)
#define MQTT_RETAINED   0
#endif
#if !defined(MQTT_DUP)
#define MQTT_DUP        0
#endif

#endif // ACN_SDK_C_MQTT_CONFIG_H_
