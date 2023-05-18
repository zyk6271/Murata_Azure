#include "wiced.h"
#include "http_server.h"
#include "resources.h"
#include "sntp.h"
#include "cJSON.h"
#include "http_callback.h"
#include "uart.h"

#define SERVER_SENT_EVENT_INTERVAL_MS  ( 1 * SECONDS )
#define MAX_SOCKETS                    ( 5 )

const wiced_ip_setting_t ip_settings =
{
    INITIALISER_IPV4_ADDRESS( .ip_address, MAKE_IPV4_ADDRESS(192,168, 4 ,1) ),
    INITIALISER_IPV4_ADDRESS( .netmask,    MAKE_IPV4_ADDRESS(255,255,255,0) ),
    INITIALISER_IPV4_ADDRESS( .gateway,    MAKE_IPV4_ADDRESS(192,168, 4 ,1) ),
};

wiced_http_server_t syr_server;
uint8_t http_status;

START_OF_HTTP_PAGE_DATABASE(web_pages)
{    "/rsa/set/rst/*",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_rst_set_callback, 0 },},
{    "/rsa/set/def/*",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_def_set_callback, 0 },},
{    "/rsa/set/ras/*",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_ras_set_callback, 0 },},
{    "/rsa/get/cnd",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_cnd_get_callback, 0 },},
{    "/rsa/get/net",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_net_get_callback, 0 },},
{    "/rsa/get/bat",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_bat_get_callback, 0 },},
{    "/rsa/get/sup",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_sup_get_callback, 0 },},
{    "/rsa/get/rse",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_rse_get_callback, 0 },},
{    "/rsa/set/rse/*",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_rse_set_callback, 0 },},
{    "/rsa/get/rsa",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_rsa_get_callback, 0 },},
{    "/rsa/set/rsa/*",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_rsa_set_callback, 0 },},
{    "/rsa/get/rsi",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_rsi_get_callback, 0 },},
{    "/rsa/set/rsi/*",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_rsi_set_callback, 0 },},
{    "/rsa/get/rsd",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_rsd_get_callback, 0 },},
{    "/rsa/set/rsd/*",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_rsd_set_callback, 0 },},
{    "/rsa/get/cnf",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_cnf_get_callback, 0 },},
{    "/rsa/set/cnf/*",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_cnf_set_callback, 0 },},
{    "/rsa/get/cnl",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_cnl_get_callback, 0 },},
{    "/rsa/set/cnl/*",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_cnl_set_callback, 0 },},
{    "/rsa/get/sse",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_sse_get_callback, 0 },},
{    "/rsa/set/sse/*",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_sse_set_callback, 0 },},
{    "/rsa/get/ssa",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_ssa_get_callback, 0 },},
{    "/rsa/set/ssa/*",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_ssa_set_callback, 0 },},
{    "/rsa/get/ssd",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_ssd_get_callback, 0 },},
{    "/rsa/set/ssd/*",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_ssd_set_callback, 0 },},
{    "/rsa/get/lng",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_lng_get_callback, 0 },},
{    "/rsa/set/lng/*",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_lng_set_callback, 0 },},
{    "/rsa/get/ver",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_ver_get_callback, 0 },},
{    "/rsa/get/ver2",                            "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_ver2_get_callback, 0 },},
{    "/rsa/get/srn",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_srn_get_callback, 0 },},
{    "/rsa/get/com",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_com_get_callback, 0 },},
{    "/rsa/set/com/*",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_com_set_callback, 0 },},
{    "/rsa/get/coa",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_coa_get_callback, 0 },},
{    "/rsa/set/coa/*",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_coa_set_callback, 0 },},
{    "/rsa/get/cod",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_cod_get_callback, 0 },},
{    "/rsa/set/cod/*",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_cod_set_callback, 0 },},
{    "/rsa/get/coe",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_coe_get_callback, 0 },},
{    "/rsa/set/coe/*",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_coe_set_callback, 0 },},
{    "/rsa/get/wfc",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_wfc_get_callback, 0 },},
{    "/rsa/set/wfc/*",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_wfc_set_callback, 0 },},
{    "/rsa/get/wfk",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_wfk_get_callback, 0 },},
{    "/rsa/set/wfk/*",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_wfk_set_callback, 0 },},
{    "/rsa/set/azc",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_azc_set_callback, 0 },},
{    "/rsa/get/azc",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_azc_get_callback, 0 },},
{    "/rsa/get/vlv",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_vlv_get_callback, 0 },},
{    "/rsa/get/wfs",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_wfs_get_callback, 0 },},
{    "/rsa/get/alm",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_alm_get_callback, 0 },},
{    "/rsa/get/aln",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_aln_get_callback, 0 },},
{    "/rsa/get/alw",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_alw_get_callback, 0 },},
{    "/rsa/get/ala",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_ala_get_callback, 0 },},
{    "/rsa/get/not",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_not_get_callback, 0 },},
{    "/rsa/get/wrn",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_wrn_get_callback, 0 },},
{    "/rsa/get/apt",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_apt_get_callback, 0 },},
{    "/rsa/set/apt/*",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_apt_set_callback, 0 },},
{    "/rsa/get/emr",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_emr_get_callback, 0 },},
{    "/rsa/set/emr/*",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_emr_set_callback, 0 },},
{    "/rsa/get/rcp",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_rcp_get_callback, 0 },},
{    "/rsa/set/rcp/*",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_rcp_set_callback, 0 },},
{    "/rsa/get/wti",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_wti_get_callback, 0 },},
{    "/rsa/set/wti/*",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_wti_set_callback, 0 },},
{    "/rsa/get/wfr",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_wfr_get_callback, 0 },},
{    "/rsa/get/rtc",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_rtc_get_callback, 0 },},
{    "/rsa/set/rtc/*",                           "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_rtc_set_callback, 0 },},
{    "/rsa/get/*",                               "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_nsc_get_callback, 0 },},
{    "/rsa/set/*/*",                             "application/json",         WICED_DYNAMIC_URL_CONTENT,     .url_content.dynamic_data  = { http_nsc_set_callback, 0 },},
END_OF_HTTP_PAGE_DATABASE();

void http_control( uint8_t value )
{
    extern uint8_t wifi_configured;
    ap_status_change(value);
    if(value != http_status)
    {
        http_status = value;
        if(value)
        {
            if(wifi_configured)
            {
                wifi_status_change(1);
            }
            wiced_network_up(WICED_AP_INTERFACE, WICED_USE_INTERNAL_DHCP_SERVER, &ip_settings);
            wiced_http_server_start( &syr_server, 5333, MAX_SOCKETS, web_pages, WICED_AP_INTERFACE, DEFAULT_URL_PROCESSOR_STACK_SIZE );
        }
        else
        {
            if(wifi_configured)
            {
                wifi_status_change(0);
            }
            wiced_http_server_stop(&syr_server);
            wiced_network_down(WICED_AP_INTERFACE);
        }
    }

}
