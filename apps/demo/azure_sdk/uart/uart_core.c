#include "wiced.h"
#include "uart_core.h"
#include "twin_upload.h"
#include "twin_parse.h"
#include "system.h"

volatile unsigned char wifi_data_process_buf[PROTOCOL_HEAD + WIFI_DATA_PROCESS_LMT];      //串口数据处理缓存
volatile unsigned char wifi_uart_rx_buf[PROTOCOL_HEAD + WIFI_UART_RECV_BUF_LMT];          //串口接收缓存
volatile unsigned char wifi_uart_tx_buf[PROTOCOL_HEAD + WIFIR_UART_SEND_BUF_LMT];         //串口发送缓存

volatile unsigned char *queue_in = NULL;
volatile unsigned char *queue_out = NULL;

volatile unsigned char stop_update_flag;

typedef struct {
    unsigned char dp_id;
    unsigned char dp_type;
} DOWNLOAD_CMD_S;

const DOWNLOAD_CMD_S download_cmd[] =
{
    {RAS_SET_CMD , DP_TYPE_VALUE},
    {RAS_GET_CMD , DP_TYPE_VALUE},
    {RAS_PUT_CMD , DP_TYPE_VALUE},
    {CND_GET_CMD , DP_TYPE_VALUE},
    {NET_GET_CMD , DP_TYPE_VALUE},
    {BAT_GET_CMD , DP_TYPE_VALUE},
    {ALA_GET_CMD , DP_TYPE_VALUE},
    {ALA_SET_CMD , DP_TYPE_VALUE},
    {ALR_GET_CMD , DP_TYPE_VALUE},
    {ALR_SET_CMD , DP_TYPE_VALUE},
    {RSE_SET_CMD , DP_TYPE_VALUE},
    {RSE_GET_CMD , DP_TYPE_VALUE},
    {RSE_PUT_CMD , DP_TYPE_VALUE},
    {RSA_SET_CMD , DP_TYPE_VALUE},
    {RSA_GET_CMD , DP_TYPE_VALUE},
    {RSA_PUT_CMD , DP_TYPE_VALUE},
    {RSI_SET_CMD , DP_TYPE_VALUE},
    {RSI_GET_CMD , DP_TYPE_VALUE},
    {RSI_PUT_CMD , DP_TYPE_VALUE},
    {RSD_SET_CMD , DP_TYPE_VALUE},
    {RSD_GET_CMD , DP_TYPE_VALUE},
    {RSD_PUT_CMD , DP_TYPE_VALUE},
    {CNF_SET_CMD , DP_TYPE_VALUE},
    {CNF_GET_CMD , DP_TYPE_VALUE},
    {CNF_PUT_CMD , DP_TYPE_VALUE},
    {CNL_SET_CMD , DP_TYPE_VALUE},
    {CNL_GET_CMD , DP_TYPE_VALUE},
    {CNL_PUT_CMD , DP_TYPE_VALUE},
    {SSE_SET_CMD , DP_TYPE_VALUE},
    {SSE_GET_CMD , DP_TYPE_VALUE},
    {SSE_PUT_CMD , DP_TYPE_VALUE},
    {SSA_SET_CMD , DP_TYPE_VALUE},
    {SSA_GET_CMD , DP_TYPE_VALUE},
    {SSA_PUT_CMD , DP_TYPE_VALUE},
    {SSD_SET_CMD , DP_TYPE_VALUE},
    {SSD_GET_CMD , DP_TYPE_VALUE},
    {SSD_PUT_CMD , DP_TYPE_VALUE},
    {LNG_SET_CMD , DP_TYPE_VALUE},
    {LNG_GET_CMD , DP_TYPE_VALUE},
    {LNG_PUT_CMD , DP_TYPE_VALUE},
    {SUP_GET_CMD , DP_TYPE_VALUE},
    {SUP_PUT_CMD , DP_TYPE_VALUE},
    {COM_SET_CMD , DP_TYPE_VALUE},
    {COM_GET_CMD , DP_TYPE_VALUE},
    {COM_PUT_CMD , DP_TYPE_VALUE},
    {COA_SET_CMD , DP_TYPE_VALUE},
    {COA_GET_CMD , DP_TYPE_VALUE},
    {COA_PUT_CMD , DP_TYPE_VALUE},
    {COD_SET_CMD , DP_TYPE_VALUE},
    {COD_GET_CMD , DP_TYPE_VALUE},
    {COD_PUT_CMD , DP_TYPE_VALUE},
    {COE_SET_CMD , DP_TYPE_VALUE},
    {COE_GET_CMD , DP_TYPE_VALUE},
    {COE_PUT_CMD , DP_TYPE_VALUE},
    {CND_PUT_CMD , DP_TYPE_VALUE},
    {EMR_SET_CMD , DP_TYPE_VALUE},
    {EMR_GET_CMD , DP_TYPE_VALUE},
    {RCP_SET_CMD , DP_TYPE_VALUE},
    {RCP_GET_CMD , DP_TYPE_VALUE},
};
unsigned long my_strlen(unsigned char *str)
{
    unsigned long len = 0;
    if(str == NULL) {
        return 0;
    }

    for(len = 0; *str ++ != '\0'; ) {
        len ++;
    }

    return len;
}
/**
 * @brief  把src所指内存区域的前count个字节设置成字符c
 * @param[out] {src} 待设置的内存首地址
 * @param[in] {ch} 设置的字符
 * @param[in] {count} 设置的内存长度
 * @return 待设置的内存首地址
 */
void *my_memset(void *src,unsigned char ch,unsigned short count)
{
    unsigned char *tmp = (unsigned char *)src;

    if(src == NULL) {
        return NULL;
    }

    while(count --) {
        *tmp ++ = ch;
    }

    return src;
}
/**
 * @brief  内存拷贝
 * @param[out] {dest} 目标地址
 * @param[in] {src} 源地址
 * @param[in] {count} 拷贝数据个数
 * @return 数据处理完后的源地址
 */
void *my_memcpy(void *dest, const void *src, unsigned short count)
{
    unsigned char *pdest = (unsigned char *)dest;
    const unsigned char *psrc  = (const unsigned char *)src;
    unsigned short i;

    if(dest == NULL || src == NULL) {
        return NULL;
    }

    if((pdest <= psrc) || (pdest > psrc + count)) {
        for(i = 0; i < count; i ++) {
            pdest[i] = psrc[i];
        }
    }else {
        for(i = count; i > 0; i --) {
            pdest[i - 1] = psrc[i - 1];
        }
    }

    return dest;
}
/**
 * @brief  字符串拷贝
 * @param[in] {dest} 目标地址
 * @param[in] {src} 源地址
 * @return 数据处理完后的源地址
 */
char *my_strcpy(char *dest, const char *src)
{
    char *p = dest;

    if(dest == NULL || src == NULL) {
        return NULL;
    }

    while(*src!='\0') {
        *dest++ = *src++;
    }
    *dest = '\0';
    return p;
}
/**
 * @brief  字符串比较
 * @param[in] {s1} 字符串 1
 * @param[in] {s2} 字符串 2
 * @return 大小比较值
 * -         0:s1=s2
 * -         <0:s1<s2
 * -         >0:s1>s2
 */
int my_strcmp(char *s1 , char *s2)
{
    while( *s1 && *s2 && *s1 == *s2 ) {
        s1++;
        s2++;
    }
    return *s1 - *s2;
}

/**
 * @brief  将int类型拆分四个字节
 * @param[in] {number} 4字节原数据
 * @param[out] {value} 处理完成后4字节数据
 * @return Null
 */
void int_to_byte(unsigned long number,unsigned char value[4])
{
    value[0] = number >> 24;
    value[1] = number >> 16;
    value[2] = number >> 8;
    value[3] = number & 0xff;
}
/**
 * @brief  将4字节合并为1个32bit变量
 * @param[in] {value} 4字节数组
 * @return 合并完成后的32bit变量
 */
unsigned long byte_to_int(const unsigned char value[4])
{
    unsigned long nubmer = 0;

    nubmer = (unsigned long)value[0];
    nubmer <<= 8;
    nubmer |= (unsigned long)value[1];
    nubmer <<= 8;
    nubmer |= (unsigned long)value[2];
    nubmer <<= 8;
    nubmer |= (unsigned long)value[3];

    return nubmer;
}
/**
 * @brief  mcu获取bool型下发dp值
 * @param[in] {value} dp数据缓冲区地址
 * @param[in] {len} dp数据长度
 * @return 当前dp值
 * @note   Null
 */
unsigned char mcu_get_dp_download_bool(const unsigned char value[],unsigned short len)
{
    return(value[0]);
}
/**
 * @brief  mcu获取value型下发dp值
 * @param[in] {value} dp数据缓冲区地址
 * @param[in] {len} dp数据长度
 * @return 当前dp值
 * @note   Null
 */
unsigned long mcu_get_dp_download_value(const unsigned char value[],unsigned short len)
{
    return(byte_to_int(value));
}
/**
 * @brief  写wifi_uart字节
 * @param[in] {dest} 缓存区地址偏移
 * @param[in] {byte} 写入字节值
 * @return 写入完成后的总长度
 */
unsigned short set_wifi_uart_byte(unsigned short dest, unsigned char byte)
{
    unsigned char *obj = (unsigned char *)wifi_uart_tx_buf + DATA_START + dest;

    *obj = byte;
    dest += 1;

    return dest;
}
/**
 * @brief  写wifi_uart_buffer
 * @param[in] {dest} 缓存区地址偏移
 * @param[in] {src} 源地址（需要发送的数据）
 * @param[in] {len} 需要发送的数据长度
 * @return 写入完成后的总长度
 */
unsigned short set_wifi_uart_buffer(unsigned short dest, const unsigned char *src, unsigned short len)
{
    if(NULL == src) {
        return dest;
    }
    unsigned char *obj = (unsigned char *)wifi_uart_tx_buf + DATA_START + dest;

    my_memcpy(obj,src,len);

    dest += len;
    return dest;
}
/**
 * @brief  判断串口接收缓存中是否有数据
 * @param  Null
 * @return 是否有数据  0:无/1:有
 */
unsigned char get_queue_total_data(void)
{
  if(queue_in != queue_out)
    return 1;
  else
    return 0;
}
/**
 * @brief  计算校验和
 * @param[in] {pack} 数据源指针
 * @param[in] {pack_len} 计算校验和长度
 * @return 校验和
 */
unsigned char get_check_sum(unsigned char *pack, unsigned short pack_len)
{
    unsigned short i;
    unsigned char check_sum = 0;

    for(i = 0; i < pack_len; i ++) {
        check_sum += *pack ++;
    }

    return check_sum;
}
/**
 * @brief  串口发送一段数据
 * @param[in] {in} 发送缓存指针
 * @param[in] {len} 数据发送长度
 * @return Null
 */
void wifi_uart_write_data(unsigned char *in, unsigned short len)
{
    if((NULL == in) || (0 == len)) {
        return;
    }

    while(len --) {
        uart_transmit_output(*in);
        in ++;
    }
}
/**
 * @brief  value型dp数据上传
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值
 * @param[in] {sub_id_buf} 存放子设备id的首地址
 * @param[in] {sub_id_len} 子设备id长度
 * @return Null
 * @note   Null
 */
unsigned char mcu_dp_value_update(unsigned char dpid,unsigned long value)
{
    unsigned short send_len = 0;

    if(stop_update_flag == ENABLE)
        return SUCCESS;

    send_len = set_wifi_uart_byte(send_len,dpid);
    send_len = set_wifi_uart_byte(send_len,DP_TYPE_VALUE);
    //
    send_len = set_wifi_uart_byte(send_len,0);
    send_len = set_wifi_uart_byte(send_len,4);
    //
    send_len = set_wifi_uart_byte(send_len,value >> 24);
    send_len = set_wifi_uart_byte(send_len,value >> 16);
    send_len = set_wifi_uart_byte(send_len,value >> 8);
    send_len = set_wifi_uart_byte(send_len,value & 0xff);

    wifi_uart_write_frame(DATA_ISSUED_CMD, MCU_TX_VER, send_len);

    return SUCCESS;
}
/**
 * @brief  string型dp数据上传
 * @param[in] {dpid} dpid号
 * @param[in] {value} 当前dp值指针
 * @param[in] {len} 数据长度
 * @param[in] {sub_id_buf} 存放子设备id的首地址
 * @param[in] {sub_id_len} 子设备id长度
 * @return Null
 * @note   Null
 */
unsigned char mcu_dp_string_update(unsigned char dpid,const unsigned char value[],unsigned short len)
{
    unsigned short send_len = 0;

    if(stop_update_flag == ENABLE)
        return SUCCESS;
    //
    send_len = set_wifi_uart_byte(send_len,dpid);
    send_len = set_wifi_uart_byte(send_len,DP_TYPE_STRING);
    //
    send_len = set_wifi_uart_byte(send_len,len / 0x100);
    send_len = set_wifi_uart_byte(send_len,len % 0x100);
    //
    send_len = set_wifi_uart_buffer(send_len,(unsigned char *)value,len);

    wifi_uart_write_frame(STATE_UPLOAD_CMD, MCU_TX_VER, send_len);

    return SUCCESS;
}
/**
 * @brief  向wifi串口发送指定数据
 * @param[in] {fr_type} 帧类型
 * @param[in] {len} 数据长度
 * @return Null
 */
void wifi_uart_write_command_value(unsigned char command, unsigned int data)
{
    mcu_dp_value_update(command,data);
}
/**
 * @brief  向wifi串口发送一帧数据
 * @param[in] {fr_type} 帧类型
 * @param[in] {fr_ver} 帧版本
 * @param[in] {len} 数据长度
 * @return Null
 */
void wifi_uart_write_frame(unsigned char fr_type, unsigned char fr_ver, unsigned short len)
{
    unsigned char check_sum = 0;

    wifi_uart_tx_buf[HEAD_FIRST] = 0x55;
    wifi_uart_tx_buf[HEAD_SECOND] = 0xaa;
    wifi_uart_tx_buf[PROTOCOL_VERSION] = fr_ver;
    wifi_uart_tx_buf[FRAME_TYPE] = fr_type;
    wifi_uart_tx_buf[LENGTH_HIGH] = len >> 8;
    wifi_uart_tx_buf[LENGTH_LOW] = len & 0xff;

    len += PROTOCOL_HEAD;
    check_sum = get_check_sum((unsigned char *)wifi_uart_tx_buf, len - 1);
    wifi_uart_tx_buf[len - 1] = check_sum;

    wifi_uart_write_data((unsigned char *)wifi_uart_tx_buf, len);
}

/**
 * @brief  读取队列1字节数据
 * @param  Null
 * @return Read the data
 */
unsigned char Queue_Read_Byte(void)
{
  unsigned char value;

  if(queue_out != queue_in)
  {
    //有数据
    if(queue_out >= (unsigned char *)(wifi_uart_rx_buf + sizeof(wifi_uart_rx_buf)))
    {
      //数据已经到末尾
      queue_out = (unsigned char *)(wifi_uart_rx_buf);
    }

    value = *queue_out ++;
  }

  return value;
}
/**
 * @brief  串口发送数据
 * @param[in] {value} 串口要发送的1字节数据
 * @return Null
 */
void uart_transmit_output(unsigned char value)
{
    wiced_uart_transmit_bytes(Control_UART,&value,1);
}
/**
 * @brief  串口接收数据暂存处理
 * @param[in] {value} 串口收到的1字节数据
 * @return Null
 * @note   在MCU串口处理函数中调用该函数,并将接收到的数据作为参数传入
 */
void uart_receive_input(unsigned char value)
{
    if(1 == queue_out - queue_in) {
        //数据队列满
    }else if((queue_in > queue_out) && ((queue_in - queue_out) >= sizeof(wifi_data_process_buf))) {
        //数据队列满
    }else {
        //队列不满
        if(queue_in >= (unsigned char *)(wifi_uart_rx_buf + sizeof(wifi_uart_rx_buf))) {
            queue_in = (unsigned char *)(wifi_uart_rx_buf);
        }

        *queue_in ++ = value;
    }
}
/**
 * @brief  协议串口初始化函数
 * @param  Null
 * @return Null
 * @note   在MCU初始化代码中调用该函数
 */
void wifi_protocol_init(void)
{
    //#error " 请在main函数中添加wifi_protocol_init()完成wifi协议初始化,并删除该行"
    queue_in = (unsigned char *)wifi_uart_rx_buf;
    queue_out = (unsigned char *)wifi_uart_rx_buf;
}
/**
 * @brief  wifi串口数据处理服务
 * @param  Null
 * @return Null
 * @note   在MCU主函数while循环中调用该函数
 */
void wifi_uart_service(void)
{
    //#error "请直接在main函数的while(1){}中添加wifi_uart_service(),调用该函数不要加任何条件判断,完成后删除该行"
    static unsigned short rx_in = 0;
    unsigned short offset = 0;
    unsigned short rx_value_len = 0;             //数据帧长度

    while((rx_in < sizeof(wifi_data_process_buf)) && get_queue_total_data() > 0) {
        wifi_data_process_buf[rx_in ++] = Queue_Read_Byte();
    }

    if(rx_in < PROTOCOL_HEAD)
        return;

    while((rx_in - offset) >= PROTOCOL_HEAD) {
        if(wifi_data_process_buf[offset + HEAD_FIRST] != FRAME_FIRST) {
            offset ++;
            continue;
        }

        if(wifi_data_process_buf[offset + HEAD_SECOND] != FRAME_SECOND) {
            offset ++;
            continue;
        }

//        if(wifi_data_process_buf[offset + PROTOCOL_VERSION] != MCU_RX_VER) {
//            offset += 2;
//            continue;
//        }

        rx_value_len = wifi_data_process_buf[offset + LENGTH_HIGH] * 0x100;
        rx_value_len += (wifi_data_process_buf[offset + LENGTH_LOW] + PROTOCOL_HEAD);
        if(rx_value_len > sizeof(wifi_data_process_buf)) {
            offset += 3;
            continue;
        }

        if((rx_in - offset) < rx_value_len) {
            break;
        }

        if(get_check_sum((unsigned char *)wifi_data_process_buf + offset,rx_value_len - 1) != wifi_data_process_buf[offset + rx_value_len - 1]) {
            //校验出错
            printf("crc error (crc:0x%X  but data:0x%X)\r\n",get_check_sum((unsigned char *)wifi_data_process_buf + offset,rx_value_len - 1),wifi_data_process_buf[offset + rx_value_len - 1]);
            offset += 3;
            continue;
        }
        data_handle(offset);
        offset += rx_value_len;
    }

    rx_in -= offset;
    if(rx_in > 0) {
        my_memcpy((char *)wifi_data_process_buf,(const char *)wifi_data_process_buf + offset,rx_in);
    }
}
/**
 * @brief  获取所有dp命令总和
 * @param[in] Null
 * @return 下发命令总和
 * @note   该函数用户不能修改
 */
unsigned char get_download_cmd_total(void)
{
    return(sizeof(download_cmd) / sizeof(download_cmd[0]));
}
/**
 * @brief  获取制定DPID在数组中的序号
 * @param[in] {dpid} dpid
 * @return dp序号
 */
unsigned char get_dowmload_dpid_index(unsigned char dpid)
{
    unsigned char index;
    unsigned char total = get_download_cmd_total();

    for(index = 0; index < total; index ++) {
        if(download_cmd[index].dp_id == dpid) {
            break;
        }
    }

    return index;
}
static unsigned char data_point_handle(const unsigned char value[])
{
    //这边是数据下发函数，请根据实际情况调用
    unsigned char dp_id,index;
    unsigned char dp_type;
    unsigned char ret;
    unsigned short dp_len;

    dp_id = value[0];
    dp_type = value[1];
    dp_len = (value[2] <<8) + value[3];

    index = get_dowmload_dpid_index(dp_id);

    if(dp_type != download_cmd[index].dp_type) {
        //错误提示
        return 0;
    }else {
        ret = dp_download_handle(dp_id, value + 4, dp_len);
    }

    return ret;
}

void data_handle(unsigned short offset)
{
    unsigned short dp_len;

    unsigned short i,total_len;
    unsigned char cmd_type = wifi_data_process_buf[offset + FRAME_TYPE];
    switch(cmd_type) {
        case FACTORY_SET_CMD:                                //恢复出厂
            factory_set_request();
        break;
        case PRODUCT_INFO_CMD:                                //产品信息
            total_len = (wifi_data_process_buf[offset + LENGTH_HIGH] << 8) | wifi_data_process_buf[offset + LENGTH_LOW];
            product_info_parse((unsigned char *)wifi_data_process_buf + offset + DATA_START,total_len);
        break;
        case TELEMETRY_CONTROL_CMD:                           //开启遥测
            total_len = (wifi_data_process_buf[offset + LENGTH_HIGH] << 8) | wifi_data_process_buf[offset + LENGTH_LOW];
            telemetry_upload((unsigned char *)wifi_data_process_buf + offset + DATA_START,total_len);
        break;
        case DATA_ISSUED_CMD:                                 //命令下发
            total_len = (wifi_data_process_buf[offset + LENGTH_HIGH] << 8) | wifi_data_process_buf[offset + LENGTH_LOW];

            for(i = 0 ;i < total_len; )
            {
                dp_len = (wifi_data_process_buf[offset + DATA_START + i + 2] << 8) | wifi_data_process_buf[offset + DATA_START + i + 3];

                data_point_handle((unsigned char *)wifi_data_process_buf + offset + DATA_START + i);

                i += (dp_len + 4);
            }
        break;
        case WIFI_AP_ENABLE_CMD:                                 //AP控制
            total_len = (wifi_data_process_buf[offset + LENGTH_HIGH] << 8) | wifi_data_process_buf[offset + LENGTH_LOW];
            wifi_ap_enable_control(wifi_data_process_buf[offset + DATA_START]);
        break;
        case UPDATE_CONTROL_CMD:                                 //升级控制
            total_len = (wifi_data_process_buf[offset + LENGTH_HIGH] << 8) | wifi_data_process_buf[offset + LENGTH_LOW];
            if(total_len==1)
            {
                ota_control_parse(wifi_data_process_buf[offset + DATA_START]);
            }
        break;
        case UPDATE_TRANS_CMD:                                //升级传输
            total_len = (wifi_data_process_buf[offset + LENGTH_HIGH] << 8) | wifi_data_process_buf[offset + LENGTH_LOW];
            if(total_len==0)
            {
                http_uart_ack();
            }
            break;
        default:break;
    }
}
