#include "wiced.h"
#include "http_server.h"
#include "resources.h"
#include "sntp.h"
#include "cJSON.h"
#include "http_callback.h"
#include "uart.h"

#define SERVER_SENT_EVENT_INTERVAL_MS  ( 1 * SECONDS )
#define MAX_SOCKETS                    ( 5 )

wiced_http_server_t syr_server;

START_OF_HTTP_PAGE_DATABASE(web_pages)
{    "/api/RST",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_rst_callback, 0 },},
{    "/api/DEF",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_def_callback, 0 },},
{    "/api/RAS",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_ras_callback, 0 },},
{    "/api/CND",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_cnd_callback, 0 },},
{    "/api/NET",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_net_callback, 0 },},
{    "/api/BAT",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_bat_callback, 0 },},
{    "/api/ALA",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_ala_callback, 0 },},
{    "/api/ALR",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_alr_callback, 0 },},
{    "/api/SUP",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_sup_callback, 0 },},
{    "/api/RSE",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_rse_callback, 0 },},
{    "/api/RSA",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_rsa_callback, 0 },},
{    "/api/RSI",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_rsi_callback, 0 },},
{    "/api/RSD",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_rsd_callback, 0 },},
{    "/api/CNF",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_cnf_callback, 0 },},
{    "/api/CNL",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_cnl_callback, 0 },},
{    "/api/SSE",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_sse_callback, 0 },},
{    "/api/SSA",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_ssa_callback, 0 },},
{    "/api/SSD",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_ssd_callback, 0 },},
{    "/api/LNG",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_lng_callback, 0 },},
{    "/api/SRN",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_srn_callback, 0 },},
{    "/api/VER",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_ver_callback, 0 },},
{    "/api/COM",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_com_callback, 0 },},
{    "/api/COA",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_coa_callback, 0 },},
{    "/api/COD",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_cod_callback, 0 },},
{    "/api/COE",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_coe_callback, 0 },},

{    "/api/WFK",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_wfk_callback, 0 },},
{    "/api/WFC",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_wfc_callback, 0 },},
{    "/api/WFS",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_wfs_callback, 0 },},
{    "/api/WSR",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_wsr_callback, 0 },},
END_OF_HTTP_PAGE_DATABASE();

void http_start( void )
{
    wiced_http_server_start( &syr_server, 5333, MAX_SOCKETS, web_pages, WICED_AP_INTERFACE, DEFAULT_URL_PROCESSOR_STACK_SIZE );
}
