#include "wiced.h"
#include "uart_core.h"
#include "twin_upload.h"
#include "twin_parse.h"

volatile unsigned char wifi_data_process_buf[PROTOCOL_HEAD + WIFI_DATA_PROCESS_LMT];      //串口数据处理缓存
volatile unsigned char wifi_uart_rx_buf[PROTOCOL_HEAD + WIFI_UART_RECV_BUF_LMT];          //串口接收缓存
volatile unsigned char wifi_uart_tx_buf[PROTOCOL_HEAD + WIFIR_UART_SEND_BUF_LMT];         //串口发送缓存

volatile unsigned char *queue_in = NULL;
volatile unsigned char *queue_out = NULL;

extern uint8_t ap_flag;
extern syr_status device_status;
extern EventGroupHandle_t Config_EventHandler;
extern EventGroupHandle_t Info_EventHandler;
extern EventGroupHandle_t TEM_EventHandler;
extern EventGroupHandle_t C2D_EventHandler;

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
 * @brief  向wifi串口发送指定数据
 * @param[in] {fr_type} 帧类型
 * @param[in] {len} 数据长度
 * @return Null
 */
void wifi_uart_write_command_value(unsigned char command, unsigned int data)
{
    unsigned char check_sum = 0;

    wifi_uart_tx_buf[HEAD_FIRST] = 0x55;
    wifi_uart_tx_buf[HEAD_SECOND] = 0xaa;
    wifi_uart_tx_buf[PROTOCOL_VERSION] = 0;
    wifi_uart_tx_buf[FRAME_TYPE] = command;
    wifi_uart_tx_buf[LENGTH_HIGH] = 0x00;
    wifi_uart_tx_buf[LENGTH_LOW] = 0x04;
    wifi_uart_tx_buf[DATA_START] = (data >> 24) & 0xFF;
    wifi_uart_tx_buf[DATA_START+1] = (data >> 16) & 0xFF;
    wifi_uart_tx_buf[DATA_START+2] = (data >> 8) & 0xFF;
    wifi_uart_tx_buf[DATA_START+3] = data & 0xff;

    uint8_t len = PROTOCOL_HEAD + 4;
    check_sum = get_check_sum((unsigned char *)wifi_uart_tx_buf, len - 1);
    wifi_uart_tx_buf[len - 1] = check_sum;
    //
    wifi_uart_write_data((unsigned char *)wifi_uart_tx_buf, len);
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
    //
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

        if(wifi_data_process_buf[offset + PROTOCOL_VERSION] != MCU_RX_VER) {
            offset += 2;
            continue;
        }

        rx_value_len = wifi_data_process_buf[offset + LENGTH_HIGH] * 0x100;
        rx_value_len += (wifi_data_process_buf[offset + LENGTH_LOW] + PROTOCOL_HEAD);
        if(rx_value_len > sizeof(wifi_data_process_buf)) {
            offset += 3;
            continue;
        }

        if((rx_in - offset) < rx_value_len) {
            break;
        }

        //数据接收完成
        if(get_check_sum((unsigned char *)wifi_data_process_buf + offset,rx_value_len - 1) != wifi_data_process_buf[offset + rx_value_len - 1]) {
            //校验出错
            printf("crc error (crc:0x%X  but data:0x%X)\r\n",get_check_sum((unsigned char *)wifi_data_process_buf + offset,rx_value_len - 1),wifi_data_process_buf[offset + rx_value_len - 1]);
            offset += 3;
            continue;
        }
        if(rx_value_len - PROTOCOL_HEAD)
        {
            printf("Command is %02X,Len is %d,Data is ",wifi_data_process_buf[offset + FRAME_TYPE],rx_value_len - PROTOCOL_HEAD);
            for(uint8_t i=0;i<rx_value_len - PROTOCOL_HEAD;i++)
            {
                printf("%02X ",wifi_data_process_buf[offset + DATA_START + i]);
            }
            printf("\r\n");
        }
        else
        {
            printf("Command is %02X,Len is 0\r\n",wifi_data_process_buf[offset + FRAME_TYPE]);
        }

        data_handle(offset,rx_value_len - PROTOCOL_HEAD);
        offset += rx_value_len;
    }//end while

    rx_in -= offset;
    if(rx_in > 0) {
        my_memcpy((char *)wifi_data_process_buf,(const char *)wifi_data_process_buf + offset,rx_in);
    }
}
void data_handle(unsigned short offset,uint32_t value_len)
{
    uint32_t value = 0;
    unsigned char cmd_type = wifi_data_process_buf[offset + FRAME_TYPE];
    if(value_len)
    {
        value = wifi_data_process_buf[offset + DATA_START] * 0x1000000;
        value += wifi_data_process_buf[offset + DATA_START + 1] * 0x10000;
        value += wifi_data_process_buf[offset + DATA_START + 2] * 0x100;
        value += wifi_data_process_buf[offset + DATA_START + 3];
    }
    switch(cmd_type)
    {
    case RSE_GET_CMD:
        device_status.config.rse = value;
        xEventGroupSetBits(Config_EventHandler,EVENT_CONFIG_RSE_GET);
        break;
    case RSE_SET_CMD:
        device_status.config.rse = value;
        xEventGroupSetBits(Config_EventHandler,EVENT_CONFIG_RSE_SET);
        break;
    case RSA_GET_CMD:
        device_status.config.rsa = value;
        xEventGroupSetBits(Config_EventHandler,EVENT_CONFIG_RSA_GET);
        break;
    case RSA_SET_CMD:
        device_status.config.rsa = value;
        xEventGroupSetBits(Config_EventHandler,EVENT_CONFIG_RSA_SET);
        break;
    case RSI_GET_CMD:
        device_status.config.rsi = value;
        xEventGroupSetBits(Config_EventHandler,EVENT_CONFIG_RSI_GET);
        break;
    case RSI_SET_CMD:
        device_status.config.rsi = value;
        xEventGroupSetBits(Config_EventHandler,EVENT_CONFIG_RSI_SET);
        break;
    case RSD_GET_CMD:
        device_status.config.rsd = value;
        xEventGroupSetBits(Config_EventHandler,EVENT_CONFIG_RSD_GET);
        break;
    case RSD_SET_CMD:
        device_status.config.rsd = value;
        xEventGroupSetBits(Config_EventHandler,EVENT_CONFIG_RSD_SET);
        break;
    case CNF_GET_CMD:
        device_status.config.cnf = value;
        xEventGroupSetBits(Config_EventHandler,EVENT_CONFIG_CNF_GET);
        break;
    case CNF_SET_CMD:
        device_status.config.cnf = value;
        xEventGroupSetBits(Config_EventHandler,EVENT_CONFIG_CNF_SET);
        break;
    case CNL_GET_CMD:
        device_status.config.cnl = value;
        xEventGroupSetBits(Config_EventHandler,EVENT_CONFIG_CNL_GET);
        break;
    case CNL_SET_CMD:
        device_status.config.cnl = value;
        xEventGroupSetBits(Config_EventHandler,EVENT_CONFIG_CNL_SET);
        break;
    case SSE_GET_CMD:
        device_status.config.sse = value;
        xEventGroupSetBits(Config_EventHandler,EVENT_CONFIG_SSE_GET);
        break;
    case SSE_SET_CMD:
        device_status.config.sse = value;
        xEventGroupSetBits(Config_EventHandler,EVENT_CONFIG_SSE_SET);
        break;
    case SSA_GET_CMD:
        device_status.config.ssa = value;
        xEventGroupSetBits(Config_EventHandler,EVENT_CONFIG_SSA_GET);
        break;
    case SSA_SET_CMD:
        device_status.config.ssa = value;
        xEventGroupSetBits(Config_EventHandler,EVENT_CONFIG_SSA_SET);
        break;
    case SSD_GET_CMD:
        device_status.config.ssd = value;
        xEventGroupSetBits(Config_EventHandler,EVENT_CONFIG_SSD_GET);
        break;
    case SSD_SET_CMD:
        device_status.config.ssd = value;
        xEventGroupSetBits(Config_EventHandler,EVENT_CONFIG_SSD_SET);
        break;
    case LNG_GET_CMD:
        device_status.config.lng = value;
        xEventGroupSetBits(Config_EventHandler,EVENT_CONFIG_LNG_GET);
        break;
    case LNG_SET_CMD:
        device_status.config.lng = value;
        xEventGroupSetBits(Config_EventHandler,EVENT_CONFIG_LNG_SET);
        break;
    case COM_GET_CMD:
        device_status.info.com = value;
        xEventGroupSetBits(Info_EventHandler,EVENT_INFO_COM_GET);
        break;
    case COM_SET_CMD:
        device_status.info.com = value;
        xEventGroupSetBits(Info_EventHandler,EVENT_INFO_COM_SET);
        break;
    case COA_GET_CMD:
        device_status.info.coa = value;
        xEventGroupSetBits(Info_EventHandler,EVENT_INFO_COA_GET);
        break;
    case COA_SET_CMD:
        device_status.info.coa = value;
        xEventGroupSetBits(Info_EventHandler,EVENT_INFO_COA_SET);
        break;
    case COD_GET_CMD:
        device_status.info.cod = value;
        xEventGroupSetBits(Info_EventHandler,EVENT_INFO_COD_GET);
        break;
    case COD_SET_CMD:
        device_status.info.cod = value;
        xEventGroupSetBits(Info_EventHandler,EVENT_INFO_COD_SET);
        break;
    case COE_GET_CMD:
        device_status.info.coe = value;
        xEventGroupSetBits(Info_EventHandler,EVENT_INFO_COE_GET);
        break;
    case COE_SET_CMD:
        device_status.info.coe = value;
        xEventGroupSetBits(Info_EventHandler,EVENT_INFO_COE_SET);
        break;
    case CND_GET_CMD:
        device_status.info.cnd = value;
        xEventGroupSetBits(Info_EventHandler,EVENT_INFO_CND_GET);
        break;
    case SUP_GET_CMD:
        device_status.info.sup = value;
        xEventGroupSetBits(Info_EventHandler,EVENT_INFO_SUP_GET);
        break;
    case VER_GET_CMD:
        //device_status.info.ver = value;
        xEventGroupSetBits(Info_EventHandler,EVENT_INFO_VER_GET);
        break;
    case SRN_GET_CMD:
        //device_status.info.srn = value;
        xEventGroupSetBits(Info_EventHandler,EVENT_INFO_SRN_GET);
        break;
    case NET_GET_CMD:
        device_status.tem.net = value;
        xEventGroupSetBits(TEM_EventHandler,EVENT_TEM_NET_GET);
        break;
    case BAT_GET_CMD:
        device_status.tem.bat = value;
        xEventGroupSetBits(TEM_EventHandler,EVENT_TEM_BAT_GET);
        break;
    case ALA_GET_CMD:
        device_status.tem.ala = value;
        xEventGroupSetBits(TEM_EventHandler,EVENT_TEM_ALA_GET);
        break;
    case ALR_GET_CMD:
        device_status.tem.alr = value;
        xEventGroupSetBits(TEM_EventHandler,EVENT_TEM_ALR_GET);
        break;
    case RST_SET_CMD:
        device_status.c2d.rst = value;
        xEventGroupSetBits(C2D_EventHandler,EVENT_C2D_RST_SET);
        break;
    case DEF_SET_CMD:
        device_status.c2d.def = value;
        xEventGroupSetBits(C2D_EventHandler,EVENT_C2D_DEF_SET);
        break;
    case RAS_SET_CMD:
        device_status.c2d.ras = value;
        xEventGroupSetBits(C2D_EventHandler,EVENT_C2D_RAS_SET);
        break;
    case WST_GET_CMD:
        wifi_status_get();
        break;
    default:
        break;
    }
    if(!ap_flag)
    {
        switch(cmd_type)
        {
        case RSE_PUT_CMD:
            config_single_upload(RSE_PUT_CMD,value);
            device_status.config.rse = value;
            break;
        case RSA_PUT_CMD:
            config_single_upload(RSA_PUT_CMD,value);
            device_status.config.rsa = value;
            break;
        case RSI_PUT_CMD:
            config_single_upload(RSI_PUT_CMD,value);
            device_status.config.rsi = value;
            break;
        case RSD_PUT_CMD:
            config_single_upload(RSD_PUT_CMD,value);
            device_status.config.rsd = value;
            break;
        case CNF_PUT_CMD:
            config_single_upload(CNF_PUT_CMD,value);
            device_status.config.cnf = value;
            break;
        case CNL_PUT_CMD:
            config_single_upload(CNL_PUT_CMD,value);
            device_status.config.cnl = value;
            break;
        case SSE_PUT_CMD:
            config_single_upload(SSE_PUT_CMD,value);
            device_status.config.sse = value;
            break;
        case SSA_PUT_CMD:
            config_single_upload(SSA_PUT_CMD,value);
            device_status.config.ssa = value;
            break;
        case SSD_PUT_CMD:
            config_single_upload(SSD_PUT_CMD,value);
            device_status.config.ssd = value;
            break;
        case LNG_PUT_CMD:
            config_single_upload(LNG_PUT_CMD,value);
            device_status.config.lng = value;
            break;
        case SUP_PUT_CMD:
            info_single_upload(SUP_PUT_CMD,value);
            device_status.info.sup = value;
            break;
        case COM_PUT_CMD:
            info_single_upload(COM_PUT_CMD,value);
            device_status.info.com = value;
            break;
        case COA_PUT_CMD:
            info_single_upload(COA_PUT_CMD,value);
            device_status.info.coa = value;
            break;
        case COD_PUT_CMD:
            info_single_upload(COD_PUT_CMD,value);
            device_status.info.cod = value;
            break;
        case COE_PUT_CMD:
            info_single_upload(COE_PUT_CMD,value);
            device_status.info.coe = value;
            break;
        case CND_PUT_CMD:
            info_single_upload(CND_PUT_CMD,value);
            device_status.info.cnd = value;
            break;
        case WFA_SET_CMD:
            set_factory();
            platform_mcu_reset();
            break;
        default:
            break;
        }
    }
}
