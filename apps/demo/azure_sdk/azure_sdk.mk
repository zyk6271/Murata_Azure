#
# Copyright 2020, Cypress Semiconductor Corporation or a subsidiary of 
 # Cypress Semiconductor Corporation. All Rights Reserved.
 # This software, including source code, documentation and related
 # materials ("Software"), is owned by Cypress Semiconductor Corporation
 # or one of its subsidiaries ("Cypress") and is protected by and subject to
 # worldwide patent protection (United States and foreign),
 # United States copyright laws and international treaty provisions.
 # Therefore, you may use this Software only as provided in the license
 # agreement accompanying the software package from which you
 # obtained this Software ("EULA").
 # If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
 # non-transferable license to copy, modify, and compile the Software
 # source code solely for use in connection with Cypress's
 # integrated circuit products. Any reproduction, modification, translation,
 # compilation, or representation of this Software except as specified
 # above is prohibited without the express written permission of Cypress.
 #
 # Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND,
 # EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED
 # WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress
 # reserves the right to make changes to the Software without notice. Cypress
 # does not assume any liability arising out of the application or use of the
 # Software or any product or circuit described in the Software. Cypress does
 # not authorize its products for use in any products where a malfunction or
 # failure of the Cypress product may reasonably be expected to result in
 # significant property damage, injury or death ("High Risk Product"). By
 # including Cypress's product in a High Risk Product, the manufacturer
 # of such system or application assumes all risk of such use and in doing
 # so agrees to indemnify Cypress against all liability.
#

NAME := azure_sdk

AZURE_SDK_DIR = ../../../apps/demo/azure_sdk/azure

GLOBAL_INCLUDES += \
                    azure \
                    ntp \
                    cloud \
                    web \
                    uart \
                    ota \
                    config \
                   $(AZURE_SDK_DIR) \
                   $(AZURE_SDK_DIR)/iot \
                   $(AZURE_SDK_DIR)/core \
                   
$(NAME)_SOURCES :=  azure_sdk.c \
                    heart.c \
                    storage.c \
                    web/http_server.c \
                    web/http_callback.c \
                    web/http_api.c \
                    uart/uart.c \
                    uart/uart_core.c \
                    uart/system.c \
                    ota/http_ota.c \
                    ota/webclient.c \
                    cloud/c2d.c \
                    cloud/telemetry.c \
                    cloud/twin_upload.c \
					cloud/iot_sample_common.c \
					cloud/mqtt_handle.c \
					cloud/twin_parse.c \
					ntp/ntp.c \
					azure/azure_server.c \
					azure/core/az_base64.c \
					azure/core/az_context.c \
					azure/core/az_http_pipeline.c \
					azure/core/az_http_policy.c \
					azure/core/az_http_policy_logging.c \
					azure/core/az_http_policy_retry.c \
					azure/core/az_http_request.c \
					azure/core/az_http_response.c \
					azure/core/az_json_reader.c \
					azure/core/az_json_token.c \
					azure/core/az_json_writer.c \
					azure/core/az_log.c \
					azure/core/az_precondition.c \
					azure/core/az_span.c \
					azure/iot/az_iot_common.c \
					azure/iot/az_iot_hub_client.c \
					azure/iot/az_iot_hub_client_c2d.c \
					azure/iot/az_iot_hub_client_commands.c \
					azure/iot/az_iot_hub_client_methods.c \
					azure/iot/az_iot_hub_client_properties.c \
					azure/iot/az_iot_hub_client_sas.c \
					azure/iot/az_iot_hub_client_telemetry.c \
					azure/iot/az_iot_hub_client_twin.c \
					azure/iot/az_iot_provisioning_client.c \
					azure/iot/az_iot_provisioning_client_sas.c \

GLOBAL_INCLUDES +=  azure \

$(NAME)_COMPONENTS := protocols/MQTT	\
					  utilities/cJSON 	\
					  daemons/HTTP_server \
					  protocols/SNTP	

$(NAME)_RESOURCES  := apps/azure_iot_hub/rootca.cer \
                      apps/azure_iot_hub/client.cer \
                      apps/azure_iot_hub/privkey.cer\
                      apps/http_server_sent_events/main.html \
                      images/cypresslogo.png \
                      images/cypresslogo_line.png \
                      images/favicon.ico
                      
WIFI_CONFIG_DCT_H := wifi_config_dct.h


GLOBAL_DEFINES += MAC_ADDRESS_SET_BY_HOST 
