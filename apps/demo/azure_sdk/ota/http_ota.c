#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

#include "wiced.h"
#include "http_ota.h"
#include "waf_platform.h"
#include "webclient.h"
#include "uart_core.h"
#include "twin_parse.h"

wiced_thread_t http_ota_t;
wiced_semaphore_t uart_ack_sem;
wiced_event_flags_t OTA_EventHandler;

uint32_t now_offset = 0;
uint32_t file_size = 0;

extern char wifi_version[];
extern syr_status device_status;
extern volatile unsigned char stop_update_flag;

void http_uart_ack(void)
{
    wiced_rtos_set_semaphore(&uart_ack_sem);
}
void ota_event_send(uint32_t value)
{
    switch(value)
    {
    case 0x1:
        wiced_rtos_set_event_flags(&OTA_EventHandler,EVENT_OTA_MURATA_START);
    break;
    case 0x2:
        wiced_rtos_set_event_flags(&OTA_EventHandler,EVENT_OTA_ST_START);
    break;
    default:break;
    }
}
void http_ota_callback( uint32_t arg )
{
    uint32_t ret,events;
    while(1)
    {
        ret = wiced_rtos_wait_for_event_flags(&OTA_EventHandler,EVENT_OTA_ST_START|EVENT_OTA_MURATA_START, &events, WICED_TRUE, WAIT_FOR_ANY_EVENT,WICED_WAIT_FOREVER);
        if(events & EVENT_OTA_MURATA_START)
        {
            murata_ota_start();
        }
        else
        {
            st_ota_start();
        }
    }
}

void http_ota_init(void)
{
    wiced_rtos_init_event_flags(&OTA_EventHandler);
    wiced_rtos_init_semaphore( &uart_ack_sem );
    wiced_rtos_create_thread( &http_ota_t, 6, "http_ota", http_ota_callback, 4096, 0 );
}
static void st_version_callback(char *buffer, int length)
{
    printf("st_version_callback get version %s,now version %s\r\n",buffer,device_status.info.ver);
    if(my_strcmp(buffer,device_status.info.ver))
    {
        st_download();
    }
    else
    {
        ota_control_send(ST_No_Upadate);
    }
}
int st_ota_start(void)
{
    int ret = RT_EOK;
    struct webclient_session* session = RT_NULL;

    session = webclient_session_create(GET_HEADER_BUFSZ);
    if (!session)
    {
        LOG_E("open uri failed.\n");
        ret = -RT_ERROR;
        goto __exit;
    }
    LOG_I("open uri success %s.\n",ST_VER_URL);
    /* get the real data length */
    ret = webclient_shard_head_function(session, ST_VER_URL, &file_size);
    if(ret < 0)
    {
        LOG_E("Request fail!\n");
        ota_control_send(Network_Fail);
        ret = -RT_ERROR;
        goto __exit;
    }
    if (file_size == 0)
    {
        LOG_E("Request file size is 0!\n");
        ret = -RT_ERROR;
        goto __exit;
    }
    else if (file_size < 0)
    {
        LOG_E("webclient GET request type is chunked.\n");
        ret = -RT_ERROR;
        goto __exit;
    }
    now_offset = 0;

    webclient_register_shard_position_function(session, st_version_callback);

    webclient_shard_position_function(session, ST_VER_URL, now_offset, file_size, 16);

    webclient_register_shard_position_function(session, RT_NULL);

__exit:
    if (session != RT_NULL)
    {
        webclient_close(session);
        session = RT_NULL;
    }
    return ret;
}
static int st_download_callback(char *buffer, int length)
{
    uint8_t ret = RT_EOK;
    uint32_t send_len = 0;
    static uint32_t chunk_count = 0;

    stop_update_flag = ENABLE;

    if(now_offset==0)
    {
        chunk_count = 0;
    }
    for(uint8_t i = 0;i < Retry_Count;i++)
    {
        send_len = 0;
        send_len = set_wifi_uart_byte(send_len,(now_offset & 0xff000000)>>24);
        send_len = set_wifi_uart_byte(send_len,(now_offset & 0xff0000)>>16);
        send_len = set_wifi_uart_byte(send_len,(now_offset & 0xff00)>>8);
        send_len = set_wifi_uart_byte(send_len,now_offset & 0xff);
        send_len = set_wifi_uart_buffer(send_len,(const unsigned char*)buffer,length);
        wifi_uart_write_frame(UPDATE_TRANS_CMD, MCU_TX_VER, send_len);

        if(wiced_rtos_get_semaphore( &uart_ack_sem, Retry_Time ) == 0)
        {
            chunk_count++;
            //printf("st_download chunk %d of size %d from offset %d \r\n", chunk_count, length, now_offset);
            now_offset += length;
            if( now_offset == file_size )
            {
                send_len = 0;
                send_len = set_wifi_uart_byte(send_len,(now_offset & 0xff000000)>>24);
                send_len = set_wifi_uart_byte(send_len,(now_offset & 0xff0000)>>16);
                send_len = set_wifi_uart_byte(send_len,(now_offset & 0xff00)>>8);
                send_len = set_wifi_uart_byte(send_len,now_offset & 0xff);
                wifi_uart_write_frame(UPDATE_TRANS_CMD, MCU_TX_VER, send_len);
                ota_control_send(ST_Downloading);
            }
            goto __exit;
        }
        else
        {
            printf("st_dowload retry %d\r\n",i+1);
        }
    }
    ret = RT_ERROR;
    printf("st_download failed\r\n");
    ota_control_send(ST_Download_Error);
__exit:
    stop_update_flag = DISABLE;
    free(buffer);
    return ret;
}
int st_download(void)
{
    int ret = RT_EOK;
    struct webclient_session* session = RT_NULL;

    session = webclient_session_create(GET_HEADER_BUFSZ);
    if (!session)
    {
        LOG_E("open uri failed.\n");
        ret = -RT_ERROR;
        goto __exit;
    }
    LOG_I("open uri success %s.\n",ST_BIN_URL);

    ret = webclient_shard_head_function(session, ST_BIN_URL, &file_size);
    if(ret < 0)
    {
        LOG_E("Request fail!\n");
        ota_control_send(Network_Fail);
        ret = -RT_ERROR;
        goto __exit;
    }
    if (file_size == 0)
    {
        LOG_E("Request file size is 0!\n");
        ret = -RT_ERROR;
        goto __exit;
    }
    else if (file_size < 0)
    {
        LOG_E("webclient GET request type is chunked.\n");
        ret = -RT_ERROR;
        goto __exit;
    }
    ota_control_send(ST_Downloading);

    unsigned short send_len = 0;
    send_len = set_wifi_uart_byte(send_len,(file_size & 0xff000000) >> 24);
    send_len = set_wifi_uart_byte(send_len,(file_size & 0xff0000) >> 16);
    send_len = set_wifi_uart_byte(send_len,(file_size & 0xff00) >> 8);
    send_len = set_wifi_uart_byte(send_len,file_size & 0xff);
    wifi_uart_write_frame(UPDATE_START_CMD, MCU_TX_VER, send_len);

    now_offset = 0;

    webclient_register_shard_position_function(session, st_download_callback);

    webclient_shard_position_function(session, ST_BIN_URL, now_offset, file_size, 512);

    webclient_register_shard_position_function(session, RT_NULL);

__exit:
    if (session != RT_NULL)
    {
        webclient_close(session);
        session = RT_NULL;
    }
    return ret;
}
static void murata_version_callback(char *buffer, int length)
{
    printf("murata_version_callback get version %s,now version %s\r\n",buffer,wifi_version);
    if(my_strcmp(buffer,wifi_version))
    {
        murata_download();
    }
    else
    {
        ota_control_send(Murata_No_Upadate);
    }
}
int murata_ota_start(void)
{
    int ret = RT_EOK;
    struct webclient_session* session = RT_NULL;

    /* create webclient session and set header response size */
    session = webclient_session_create(GET_HEADER_BUFSZ);
    if (!session)
    {
        LOG_E("open uri failed.\n");
        ret = -RT_ERROR;
        goto __exit;
    }
    LOG_I("open uri success %s.\n",MURATA_VER_URL);
    /* get the real data length */
    ret = webclient_shard_head_function(session, MURATA_VER_URL, &file_size);
    if(ret < 0)
    {
        LOG_E("Request fail!\n");
        ota_control_send(Network_Fail);
        ret = -RT_ERROR;
        goto __exit;
    }
    if (file_size == 0)
    {
        LOG_E("Request file size is 0!\n");
        ret = -RT_ERROR;
        goto __exit;
    }
    else if (file_size < 0)
    {
        LOG_E("webclient GET request type is chunked.\n");
        ret = -RT_ERROR;
        goto __exit;
    }
    now_offset = 0;

    webclient_register_shard_position_function(session, murata_version_callback);

    /* the "memory size" that you can provide in the project and uri */
    webclient_shard_position_function(session, MURATA_VER_URL, now_offset, file_size, 16);

    /* clear the handle function */
    webclient_register_shard_position_function(session, RT_NULL);

__exit:
    if (session != RT_NULL)
    {
        webclient_close(session);
        session = RT_NULL;
    }
    return ret;
}

static int murata_download_callback(char *buffer, int length)
{
    int ret = RT_EOK;
    static wiced_app_t app;
    static uint32_t chunk_count = 0;

    stop_update_flag = ENABLE;
    if (now_offset == 0)
    {
        ota_control_send(Murata_Downloading);
        chunk_count = 0;
        uint32_t current_size;
        if (wiced_framework_app_open( DCT_APP0_INDEX, &app ) != WICED_SUCCESS)
        {
            printf("murata_download failed\r\n");
            ota_control_send(Murata_Download_Error);
            goto __exit;
        }
        if (wiced_framework_app_get_size( &app, &current_size ) != WICED_SUCCESS)
        {
            printf("murata_download failed\r\n");
            ota_control_send(Murata_Download_Error);
            goto __exit;
        }
        if (wiced_framework_app_set_size( &app, file_size) != WICED_SUCCESS)
        {
            printf("murata_download failed\r\n");
            ota_control_send(Murata_Download_Error);
            goto __exit;
        }
        if (wiced_framework_app_get_size( &app, &current_size ) != WICED_SUCCESS)
        {
            printf("murata_download failed\r\n");
            ota_control_send(Murata_Download_Error);
            goto __exit;
        }
        if ( current_size < file_size )
        {
            printf("Error setting application size!!\n");
            printf("murata_download failed\r\n");
            ota_control_send(Murata_Download_Error);
            goto __exit;
        }
    }
    chunk_count++;

    if(wiced_framework_app_write_chunk( &app, (const uint8_t*)buffer, length ) != WICED_SUCCESS)
    {
        LOG_E("Firmware download failed! Partition write data error!");
        goto __exit;
    }
    //printf("murata_download chunk %d of size %d from offset %lu \r\n", chunk_count, length, now_offset);
    now_offset += length;
    uint8_t per = now_offset * 100 / file_size;
    unsigned short send_len = 0;
    send_len = set_wifi_uart_byte(send_len,per);
    wifi_uart_write_frame(UPDATE_PROGRSS_CMD, MCU_TX_VER, send_len);
    if( now_offset == file_size )
    {
        wiced_framework_app_close( &app );
        wiced_framework_set_boot ( DCT_APP0_INDEX, PLATFORM_DEFAULT_LOAD );
        ota_control_send(Murata_Download_Done);
        LOG_I("Download firmware to flash success.\n");
        LOG_I("System now restart...\n");
        chunk_count = 0;
        wiced_framework_reboot();
    }
__exit:
    stop_update_flag = DISABLE;
    free(buffer);
    return ret;

}
int murata_download(void)
{
    int ret = RT_EOK;
    struct webclient_session* session = RT_NULL;

    session = webclient_session_create(GET_HEADER_BUFSZ);
    if (!session)
    {
        LOG_E("open uri failed.\n");
        ret = -RT_ERROR;
        goto __exit;
    }
    LOG_I("open uri success %s.\n",MURATA_BIN_URL);

    ret = webclient_shard_head_function(session, MURATA_BIN_URL, &file_size);
    if(ret < 0)
    {
        LOG_E("Request fail!\n");
        ota_control_send(Network_Fail);
        ret = -RT_ERROR;
        goto __exit;
    }
    if (file_size == 0)
    {
        LOG_E("Request file size is 0!\n");
        ret = -RT_ERROR;
        goto __exit;
    }
    else if (file_size < 0)
    {
        LOG_E("webclient GET request type is chunked.\n");
        ret = -RT_ERROR;
        goto __exit;
    }
    now_offset = 0;

    webclient_register_shard_position_function(session, murata_download_callback);

    webclient_shard_position_function(session, MURATA_BIN_URL, now_offset, file_size, HTTP_OTA_BUFF_LEN);

    webclient_register_shard_position_function(session, RT_NULL);

__exit:
    if (session != RT_NULL)
    {
        webclient_close(session);
        session = RT_NULL;
    }
    return ret;
}
