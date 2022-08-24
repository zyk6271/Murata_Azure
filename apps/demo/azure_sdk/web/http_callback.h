#ifndef HTTP_CALLBACK_H
#define HTTP_CALLBACK_H

#define SUCCESS_CODE 0
#define TIMEOUT_CODE 1
#define ERRREQS_CODE 2
#define ERRDATA_CODE 3

int32_t wifi_callback( const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body );
int32_t http_rst_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_def_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_ras_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, waiced_http_message_body_t* http_message_body);
int32_t http_cnd_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_net_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_bat_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_ala_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_alr_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_sup_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_rse_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_rse_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_rsa_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_rsa_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_rsi_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_rsi_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_rsd_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_rsd_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_cnf_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_cnf_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_cnl_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_cnl_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_sse_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_sse_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_ssa_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_ssa_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_ssd_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_ssd_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_lng_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_lng_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_ver_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_srn_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_com_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_com_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_coa_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_coa_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_cod_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_cod_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_coe_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_coe_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_wfc_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_wfc_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_wfs_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_wfs_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_wfk_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_wfk_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_wsr_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_azc_get_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);
int32_t http_azc_set_callback(const char* url_path, const char* url_parameters, wiced_http_response_stream_t* stream, void* arg, wiced_http_message_body_t* http_message_body);

#endif
