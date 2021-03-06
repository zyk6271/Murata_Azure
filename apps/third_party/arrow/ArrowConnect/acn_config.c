/** @file
 *
 */
#include <stdlib.h>
#include <string.h>
#include "wiced.h"
#include "http_server.h"
#include "wwd_constants.h"
#include <wiced_utilities.h>
#include <resources.h>
#include "dns_redirect.h"
#include "wiced_network.h"
#include "wiced_framework.h"
#include "wiced_wps.h"
#include "acn_config.h"

#ifdef USE_HTTPS
#include "wiced_tls.h"
#endif

/******************************************************
 *                      Macros
 ******************************************************/
#define HTTPS_PORT                  443
#define HTTP_PORT                   80
#define PIN_FIELD_NAME              "pin"

#ifdef USE_HTTPS
#define CONNECT_PROTOCOL            "https\n"
#else
#define CONNECT_PROTOCOL            "http\n"
#endif

/******************************************************
 *                    Constants
 ******************************************************/
static const wiced_wps_device_detail_t wps_details =
{
    .device_name               = PLATFORM,
    .manufacturer              = "Cypress",
    .model_name                = PLATFORM,
    .model_number              = "2.0",
    .serial_number             = "1408248",
    .device_category           = WICED_WPS_DEVICE_COMPUTER,
    .sub_category              = 7,
    .config_methods            = WPS_CONFIG_LABEL | WPS_CONFIG_VIRTUAL_PUSH_BUTTON | WPS_CONFIG_VIRTUAL_DISPLAY_PIN,
    .authentication_type_flags = WPS_OPEN_AUTHENTICATION | WPS_WPA_PSK_AUTHENTICATION | WPS_WPA2_PSK_AUTHENTICATION,
    .encryption_type_flags     = WPS_NO_ENCRYPTION | WPS_MIXED_ENCRYPTION,
};

/******************************************************
 *               Variable Definitions
 ******************************************************/
static const wiced_ip_setting_t device_init_ip_settings =
{
    INITIALISER_IPV4_ADDRESS( .ip_address, MAKE_IPV4_ADDRESS( 192,168,  0,  1 ) ),
    INITIALISER_IPV4_ADDRESS( .netmask,    MAKE_IPV4_ADDRESS( 255,255,255,  0 ) ),
    INITIALISER_IPV4_ADDRESS( .gateway,    MAKE_IPV4_ADDRESS( 192,168,  0,  1 ) ),
};

#ifdef USE_HTTPS
static wiced_https_server_t*     http_server;
static wiced_tls_identity_t      tls_identity;

#else
static wiced_http_server_t*      http_server;
#endif

static wiced_bool_t                 config_use_wps;
static char                         config_wps_pin[9];
static wiced_semaphore_t            acn_config_semaphore;

/******************************************************
 *                 Type Definitions
 ******************************************************/
typedef struct
{
    wiced_http_response_stream_t* stream;
    wiced_semaphore_t           semaphore;
    uint32_t                    result_count;
} process_scan_data_t;

/******************************************************
 *               Static Function Declarations
 ******************************************************/

static int32_t        process_app_settings_page ( const char* url_parameters, const char* url_query_string, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_data );
static int32_t        process_wps_go            ( const char* url_parameters, const char* url_query_string, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_data );
static int32_t        process_scan              ( const char* url_parameters, const char* url_query_string, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_data );
static int32_t        process_connect           ( const char* url_parameters, const char* url_query_string, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_data );
static wiced_result_t scan_handler              ( wiced_scan_handler_result_t* malloced_scan_result );

/******************************************************
 *               Variable Definitions
 ******************************************************/
/**
 * URL Handler List
 */
START_OF_HTTP_PAGE_DATABASE(config_http_page_database)
    ROOT_HTTP_PAGE_REDIRECT("/config/scan_page_outer.html"),
    { "/images/cypresslogo.png",         "image/png",                         WICED_RESOURCE_URL_CONTENT,   .url_content.resource_data  = &resources_images_DIR_cypresslogo_png,        },
    { "/images/cypresslogo_line.png",    "image/png",                         WICED_RESOURCE_URL_CONTENT,   .url_content.resource_data  = &resources_images_DIR_cypresslogo_line_png,   },
    { "/wpad.dat",                       "application/x-ns-proxy-autoconfig", WICED_RESOURCE_URL_CONTENT,   .url_content.resource_data  = &resources_scripts_DIR_wpad_dat,              },
    { "/scan_results.txt",               "text/plain",                        WICED_DYNAMIC_URL_CONTENT,    .url_content.dynamic_data   = {process_scan,                  0 }           },
    { "/images/64_0bars.png",            "image/png",                         WICED_RESOURCE_URL_CONTENT,   .url_content.resource_data  = &resources_images_DIR_64_0bars_png,           },
    { "/images/64_1bars.png",            "image/png",                         WICED_RESOURCE_URL_CONTENT,   .url_content.resource_data  = &resources_images_DIR_64_1bars_png,           },
    { "/images/64_2bars.png",            "image/png",                         WICED_RESOURCE_URL_CONTENT,   .url_content.resource_data  = &resources_images_DIR_64_2bars_png,           },
    { "/images/64_3bars.png",            "image/png",                         WICED_RESOURCE_URL_CONTENT,   .url_content.resource_data  = &resources_images_DIR_64_3bars_png,           },
    { "/images/64_4bars.png",            "image/png",                         WICED_RESOURCE_URL_CONTENT,   .url_content.resource_data  = &resources_images_DIR_64_4bars_png,           },
    { "/images/64_5bars.png",            "image/png",                         WICED_RESOURCE_URL_CONTENT,   .url_content.resource_data  = &resources_images_DIR_64_5bars_png,           },
    { "/images/tick.png",                "image/png",                         WICED_RESOURCE_URL_CONTENT,   .url_content.resource_data  = &resources_images_DIR_tick_png,               },
    { "/images/cross.png",               "image/png",                         WICED_RESOURCE_URL_CONTENT,   .url_content.resource_data  = &resources_images_DIR_cross_png,              },
    { "/images/lock.png",                "image/png",                         WICED_RESOURCE_URL_CONTENT,   .url_content.resource_data  = &resources_images_DIR_lock_png,               },
    { "/images/progress.gif",            "image/gif",                         WICED_RESOURCE_URL_CONTENT,   .url_content.resource_data  = &resources_images_DIR_progress_gif,           },
    { "/config/device_settings.html",    "text/html",                         WICED_DYNAMIC_URL_CONTENT,    .url_content.dynamic_data   = {process_app_settings_page,     0 } },
    { "/config/scan_page_outer.html",    "text/html",                         WICED_RESOURCE_URL_CONTENT,   .url_content.resource_data  = &resources_config_DIR_scan_page_outer_html,   },
    { "/scripts/general_ajax_script.js", "application/javascript",            WICED_RESOURCE_URL_CONTENT,   .url_content.resource_data  = &resources_scripts_DIR_general_ajax_script_js,},
    { "/images/wps_icon.png",            "image/png",                         WICED_RESOURCE_URL_CONTENT,   .url_content.resource_data  = &resources_images_DIR_wps_icon_png,           },
    { "/images/scan_icon.png",           "image/png",                         WICED_RESOURCE_URL_CONTENT,   .url_content.resource_data  = &resources_images_DIR_scan_icon_png,          },
    { "/images/favicon.ico",             "image/vnd.microsoft.icon",          WICED_RESOURCE_URL_CONTENT,   .url_content.resource_data  = &resources_images_DIR_favicon_ico,            },
    { "/images/cypresslogo.png",         "image/png",                         WICED_RESOURCE_URL_CONTENT,   .url_content.resource_data  = &resources_images_DIR_cypresslogo_png,        },
    { "/images/cypresslogo_line.png",    "image/png",                         WICED_RESOURCE_URL_CONTENT,   .url_content.resource_data  = &resources_images_DIR_cypresslogo_line_png,   },
    { "/styles/buttons.css",             "text/css",                          WICED_RESOURCE_URL_CONTENT,   .url_content.resource_data  = &resources_styles_DIR_buttons_css,            },
    { "/connect",                        "text/html",                         WICED_DYNAMIC_URL_CONTENT,    .url_content.dynamic_data   = {process_connect,               0 }           },
    { "/wps_go",                         "text/html",                         WICED_DYNAMIC_URL_CONTENT,    .url_content.dynamic_data   = {process_wps_go,                0 }           },
    /* Add more pages here */
END_OF_HTTP_PAGE_DATABASE();

/******************************************************
 *             Static Function Definitions
 ******************************************************/
static int32_t process_app_settings_page( const char* url_parameters, const char* url_query_string, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_data )
{
    UNUSED_PARAMETER( url_parameters );
    UNUSED_PARAMETER( url_query_string );
    UNUSED_PARAMETER( arg );
    UNUSED_PARAMETER( http_data );

    wiced_http_response_stream_write_resource( stream, &resources_config_DIR_scan_page_outer_html );

    return 0;
}

static wiced_result_t scan_handler( wiced_scan_handler_result_t* malloced_scan_result )
{
    process_scan_data_t* scan_data = (process_scan_data_t*)malloced_scan_result->user_data;

    malloc_transfer_to_curr_thread( malloced_scan_result );

    /* Check if scan is not finished */
    if ( malloced_scan_result->status == WICED_SCAN_INCOMPLETE )
    {
        char temp_buffer[70];
        char* temp_ptr;
        const char* const_temp_ptr;
        uint16_t temp_length;
        int i;

        wiced_http_response_stream_t* stream = scan_data->stream;

        /* Result ID */
        temp_length = unsigned_to_decimal_string( scan_data->result_count, temp_buffer, 1, 10 );
        temp_buffer[temp_length++] = '\n';
        scan_data->result_count++;
        wiced_http_response_stream_write( stream, temp_buffer, temp_length );

        /* SSID */
        temp_ptr = temp_buffer;
        for( i = 0; i < malloced_scan_result->ap_details.SSID.length; i++)
        {
            temp_ptr = string_append_two_digit_hex_byte( temp_ptr, malloced_scan_result->ap_details.SSID.value[i] );
        }
        *temp_ptr = '\n';
        temp_ptr++;

        wiced_http_response_stream_write(stream, temp_buffer, (uint32_t)( temp_ptr - temp_buffer ) );

        /* Security */
        temp_length = unsigned_to_decimal_string( malloced_scan_result->ap_details.security, temp_buffer, 1, 10 );
        temp_buffer[temp_length++] = '\n';
        wiced_http_response_stream_write( stream, temp_buffer, temp_length );

        const_temp_ptr = (   malloced_scan_result->ap_details.security == WICED_SECURITY_OPEN )? "OPEN\n" :
                         ( ( malloced_scan_result->ap_details.security & WEP_ENABLED   ) != 0 )? "WEP\n"  :
                         ( ( malloced_scan_result->ap_details.security & WPA_SECURITY  ) != 0 )? "WPA\n"  :
                         ( ( malloced_scan_result->ap_details.security & WPA2_SECURITY ) != 0 )? "WPA2\n" : "UNKNOWN\n";
        wiced_http_response_stream_write(stream, const_temp_ptr, strlen(const_temp_ptr) );

        /* RSSI */
        temp_length = signed_to_decimal_string( malloced_scan_result->ap_details.signal_strength, temp_buffer, 1, 11 );
        temp_buffer[temp_length++] = '\n';
        wiced_http_response_stream_write( stream, temp_buffer, temp_length );

        /* Channel */
        temp_length = unsigned_to_decimal_string( malloced_scan_result->ap_details.channel, temp_buffer, 1, 10 );
        temp_buffer[temp_length++] = '\n';
        wiced_http_response_stream_write( stream, temp_buffer, temp_length );

        /* BSSID */
        temp_ptr = temp_buffer;
        temp_ptr = string_append_two_digit_hex_byte( temp_ptr, malloced_scan_result->ap_details.BSSID.octet[0] );
        temp_ptr = string_append_two_digit_hex_byte( temp_ptr, malloced_scan_result->ap_details.BSSID.octet[1] );
        temp_ptr = string_append_two_digit_hex_byte( temp_ptr, malloced_scan_result->ap_details.BSSID.octet[2] );
        temp_ptr = string_append_two_digit_hex_byte( temp_ptr, malloced_scan_result->ap_details.BSSID.octet[3] );
        temp_ptr = string_append_two_digit_hex_byte( temp_ptr, malloced_scan_result->ap_details.BSSID.octet[4] );
        temp_ptr = string_append_two_digit_hex_byte( temp_ptr, malloced_scan_result->ap_details.BSSID.octet[5] );
        *temp_ptr = '\n';
        temp_ptr++;
        wiced_http_response_stream_write( stream, temp_buffer, (uint32_t)(temp_ptr - temp_buffer) );

        /* Remembered */
        temp_length = unsigned_to_decimal_string( 0, temp_buffer, 1, 10 );  /* TODO: add support for remembered APs */
        temp_buffer[temp_length++] = '\n';
        wiced_http_response_stream_write( stream, temp_buffer, temp_length );
    }
    else
    {
        wiced_rtos_set_semaphore( &scan_data->semaphore );
    }

    free(malloced_scan_result);

    return WICED_SUCCESS;
}

static int32_t process_scan( const char* url_parameters, const char* url_query_string, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_data )
{
    process_scan_data_t scan_data;

    UNUSED_PARAMETER( url_parameters );
    UNUSED_PARAMETER( arg );
    UNUSED_PARAMETER( url_query_string );
    UNUSED_PARAMETER( http_data );

    scan_data.stream = stream;
    scan_data.result_count = 0;

    /* Initialise the semaphore that will tell us when the scan is complete */
    wiced_rtos_init_semaphore(&scan_data.semaphore);

    wiced_http_response_stream_write( stream, CONNECT_PROTOCOL, sizeof( CONNECT_PROTOCOL ) - 1 );

    /* Start the scan */
    wiced_wifi_scan_networks( scan_handler, &scan_data );

    /* Wait until scan is complete */
    wiced_rtos_get_semaphore(&scan_data.semaphore, WICED_WAIT_FOREVER);

    /* Clean up */
    wiced_rtos_deinit_semaphore(&scan_data.semaphore);

    return 0;
}


static int32_t process_wps_go( const char* url_parameters, const char* url_query_string, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_data )
{
    unsigned int url_parameters_len;

    UNUSED_PARAMETER( stream );
    UNUSED_PARAMETER( arg );
    UNUSED_PARAMETER( url_query_string );
    UNUSED_PARAMETER( http_data );

    url_parameters_len = strlen(url_parameters);

    /* client has signalled to start client mode via WPS. */
    config_use_wps = WICED_TRUE;

    /* Check if config method is PIN */
    if ( ( strlen( PIN_FIELD_NAME ) + 1 < url_parameters_len ) &&
         ( 0 == strncmp( url_parameters, PIN_FIELD_NAME "=", strlen( PIN_FIELD_NAME ) + 1 ) ) )
    {
        unsigned int pinlen = 0;

        url_parameters += strlen( PIN_FIELD_NAME ) + 1;

        /* Find length of pin */
        while ( ( url_parameters[pinlen] != '&'    ) &&
                ( url_parameters[pinlen] != '\n'   ) &&
                ( url_parameters[pinlen] != '\x00' ) &&
                ( url_parameters_len > 0 ) )
        {
            pinlen++;
            url_parameters_len--;
        }
        memcpy( config_wps_pin, url_parameters, pinlen );
        config_wps_pin[pinlen] = '\x00';
    }
    else
    {
        config_wps_pin[0] = '\x00';
    }

    /* Config has been set. Continue the main thread */
    wiced_rtos_set_semaphore(&acn_config_semaphore);
    return 1;
}


/**
 * URL handler for signaling web server shutdown
 *
 * The reception of this web server request indicates that the client wants to
 * start the appliance, after shutting down the access point, DHCP server and web server
 * Decodes the URL parameters into the connection configuration buffer, then signals
 * for the web server to shut down
 *
 * @param  socket  : a handle for the TCP socket over which the data will be sent
 * @param  url_parameters     : a byte array containing any parameters included in the URL
 * @param  url_parameters_len : size of the url_parameters byte array in bytes
 */

static int32_t process_connect( const char* url_parameters, const char* url_query_string, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_data )
{
    /* This is the first part of the platform_dct_wifi_config_t structure */
    struct
    {
        wiced_bool_t             device_configured;
        wiced_config_ap_entry_t  ap_entry;
    } temp_config;

    UNUSED_PARAMETER( stream );
    UNUSED_PARAMETER( arg );
    UNUSED_PARAMETER( url_parameters );
    UNUSED_PARAMETER( http_data );

    memset( &temp_config, 0, sizeof(temp_config) );

    /* First, parse AP details */
    while (url_query_string[0] == 'a' && url_query_string[3] == '=')
    {
        uint8_t ap_index;
        const char* end_of_value;

        /* Extract the AP index and check validity */
        ap_index = (uint8_t)( url_query_string[2] - '0' );
        if (ap_index >= CONFIG_AP_LIST_SIZE)
        {
            return -1;
        }

        /* Find the end of the value */
        end_of_value = &url_query_string[4];
        while( (*end_of_value != '&') && (*end_of_value != '\x00') && (*end_of_value != '\n') )
        {
            ++end_of_value;
        }

        /* Parse either the SSID or PSK*/
        if ( url_query_string[1] == 's' )
        {
            memcpy( temp_config.ap_entry.details.SSID.value, &url_query_string[4], (size_t) ( end_of_value - &url_query_string[4] ) );
            temp_config.ap_entry.details.SSID.length = (uint8_t) ( end_of_value - &url_query_string[4] );
            temp_config.ap_entry.details.SSID.value[temp_config.ap_entry.details.SSID.length] = 0;
        }
        else if (url_query_string[1] == 'p')
        {
            temp_config.ap_entry.security_key_length = (uint8_t) ( end_of_value - &url_query_string[4] );
            memcpy( temp_config.ap_entry.security_key, &url_query_string[4], temp_config.ap_entry.security_key_length);
            temp_config.ap_entry.security_key[temp_config.ap_entry.security_key_length] = 0;
        }
        else if (url_query_string[1] == 't')
        {
            temp_config.ap_entry.details.security = (wiced_security_t) atoi( &url_query_string[4] );
        }
        else
        {
            return -1;
        }
        url_query_string = end_of_value + 1;
    }

    /* Save updated config details */
    temp_config.device_configured = WICED_TRUE;
    wiced_dct_write( &temp_config, DCT_WIFI_CONFIG_SECTION, 0, sizeof(temp_config) );

    /* Config has been set. Continue the main thread */
    wiced_rtos_set_semaphore(&acn_config_semaphore);
    return 0;
}

/******************************************************
 *               Function Definitions
 ******************************************************/
wiced_result_t acn_configure_device(void)
{
    wiced_bool_t             device_configured;
    wiced_result_t           result;
    wiced_config_soft_ap_t*  config_ap;
    acn_config_dct_t*        acn_dct_ptr;

    result = wiced_dct_read_lock( (void**) &acn_dct_ptr, WICED_FALSE, DCT_APP_SECTION, 0, sizeof( acn_config_dct_t ) );
    if ( result != WICED_SUCCESS )
    {
        WPRINT_APP_INFO(("Unable to lock DCT to read certificate\n"));
        return WICED_ERROR;
    }
    device_configured = acn_dct_ptr->is_configured;
    wiced_dct_read_unlock( acn_dct_ptr, WICED_FALSE );

    if ( device_configured != WICED_TRUE )
    {
        dns_redirector_t dns_redirector;

        /* Initialize variables */
        config_use_wps = WICED_FALSE;

        wiced_rtos_init_semaphore(&acn_config_semaphore);

        /* Prepare the HTTP server */
        http_server = malloc(sizeof(*http_server));
        memset( http_server, 0, sizeof(*http_server) );

        /* Start the AP */
        wiced_network_up( WICED_CONFIG_INTERFACE, WICED_USE_INTERNAL_DHCP_SERVER, &device_init_ip_settings );

        result= wiced_dct_read_lock( (void**) &config_ap, WICED_FALSE, DCT_WIFI_CONFIG_SECTION, OFFSETOF(platform_dct_wifi_config_t, config_ap_settings), sizeof(wiced_config_soft_ap_t) );
        if ( result != WICED_SUCCESS )
        {
            return WICED_ERROR;
        }

        WPRINT_APP_INFO(("*******************************************************\n"));
        WPRINT_APP_INFO(("Config SSID : %s\n", config_ap->SSID.value));
        WPRINT_APP_INFO(("Password    : %s\n", config_ap->security_key));
        WPRINT_APP_INFO(("IP Address  : %u.%u.%u.%u\n", (unsigned char) ( ( device_init_ip_settings.ip_address.ip.v4 >> 24 ) & 0xff ),
                                                        (unsigned char) ( ( device_init_ip_settings.ip_address.ip.v4 >> 16 ) & 0xff ),
                                                        (unsigned char) ( ( device_init_ip_settings.ip_address.ip.v4 >> 8 ) & 0xff ),
                                                        (unsigned char) ( ( device_init_ip_settings.ip_address.ip.v4 >> 0 ) & 0xff ) ) );
        WPRINT_APP_INFO(("*******************************************************\n"));

        wiced_dct_read_unlock( config_ap, WICED_FALSE );

        /* Start the DNS redirect server */
        wiced_dns_redirector_start( &dns_redirector, WICED_CONFIG_INTERFACE );

        /* Start the HTTP server */
#ifdef USE_HTTPS
        {
            /* Lock the DCT to allow us to access the certificate and key */
            result = wiced_dct_read_lock( (void**) &dct_security, WICED_FALSE, DCT_SECURITY_SECTION, 0, sizeof( *dct_security ) );
            if ( result != WICED_SUCCESS )
            {
                WPRINT_APP_INFO(("Unable to lock DCT to read certificate\n"));
                return result;
            }

            /* Setup TLS identity */
            result = wiced_tls_init_identity( &tls_identity, dct_security->private_key, (uint8_t*)dct_security->certificate, strlen(dct_security->certificate) );
            if ( result != WICED_SUCCESS )
            {
                WPRINT_APP_INFO(( "Unable to initialize TLS identity. Error = [%d]\n", result ));
                return result;
            }

            wiced_https_server_start(http_server, HTTPS_PORT, 10, config_http_page_database, &tls_identity, WICED_CONFIG_INTERFACE, DEFAULT_URL_PROCESSOR_STACK_SIZE );

            /* Finished accessing the certificates */
            wiced_dct_read_unlock( dct_security, WICED_FALSE );
        }
#else
        wiced_http_server_start(http_server, HTTP_PORT, 10, config_http_page_database, WICED_CONFIG_INTERFACE, DEFAULT_URL_PROCESSOR_STACK_SIZE );
#endif
        /* Wait for configuration to complete */
        wiced_rtos_get_semaphore(&acn_config_semaphore, WICED_WAIT_FOREVER);

        /* Cleanup HTTP server */
#ifdef USE_HTTPS
        wiced_https_server_stop(http_server);
#else
        wiced_http_server_stop(http_server);
#endif
        free( http_server );

        wiced_rtos_deinit_semaphore(&acn_config_semaphore);

        /* Cleanup DNS server */
        wiced_dns_redirector_stop(&dns_redirector);

        /* Turn off AP */
        wiced_network_down( WICED_CONFIG_INTERFACE );

        /* Check if WPS was selected */
        if (config_use_wps == WICED_TRUE)
        {
            wiced_result_t ret;
            wiced_wps_credential_t* wps_credentials = MALLOC_OBJECT("wps",wiced_wps_credential_t);

            if (config_wps_pin[0] == '\x00')
            {
                ret = wiced_wps_enrollee(WICED_WPS_PBC_MODE, &wps_details, "00000000", wps_credentials, 1);
            }
            else
            {
                ret = wiced_wps_enrollee(WICED_WPS_PIN_MODE, &wps_details, config_wps_pin, wps_credentials, 1);
            }

            if (ret == WICED_SUCCESS)
            {
                /* Write received credentials into DCT */
                struct
                {
                    wiced_bool_t             device_configured;
                    wiced_config_ap_entry_t  ap_entry;
                } temp_config;
                memset(&temp_config, 0, sizeof(temp_config));
                memcpy(&temp_config.ap_entry.details.SSID,     &wps_credentials->ssid, sizeof(wiced_ssid_t));
                memcpy(&temp_config.ap_entry.details.security, &wps_credentials->security, sizeof(wiced_security_t));
                memcpy(temp_config.ap_entry.security_key,       wps_credentials->passphrase, wps_credentials->passphrase_length);
                temp_config.ap_entry.security_key_length = wps_credentials->passphrase_length;
                temp_config.device_configured = WICED_TRUE;
                wiced_dct_write( &temp_config, DCT_WIFI_CONFIG_SECTION, 0, sizeof(temp_config) );
            }
            else
            {
                /* TODO: WPS failed.. Do something */
            }

            free(wps_credentials);
        }

        device_configured = WICED_TRUE;
        wiced_dct_write( &device_configured, DCT_APP_SECTION, 0, sizeof(device_configured) );

        result = WICED_SUCCESS;
    }
    else
    {
        result = WICED_ALREADY_INITIALIZED;
    }

    return result;
}

