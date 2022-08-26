#include "wiced.h"
#include "uart_core.h"
#include "twin_upload.h"
#include "twin_parse.h"

volatile unsigned char wifi_data_process_buf[PROTOCOL_HEAD + WIFI_DATA_PROCESS_LMT];      //�������ݴ�����
volatile unsigned char wifi_uart_rx_buf[PROTOCOL_HEAD + WIFI_UART_RECV_BUF_LMT];          //���ڽ��ջ���
volatile unsigned char wifi_uart_tx_buf[PROTOCOL_HEAD + WIFIR_UART_SEND_BUF_LMT];         //���ڷ��ͻ���

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
 * @brief  ��src��ָ�ڴ������ǰcount���ֽ����ó��ַ�c
 * @param[out] {src} �����õ��ڴ��׵�ַ
 * @param[in] {ch} ���õ��ַ�
 * @param[in] {count} ���õ��ڴ泤��
 * @return �����õ��ڴ��׵�ַ
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
 * @brief  �ڴ濽��
 * @param[out] {dest} Ŀ���ַ
 * @param[in] {src} Դ��ַ
 * @param[in] {count} �������ݸ���
 * @return ���ݴ�������Դ��ַ
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
 * @brief  �ַ�������
 * @param[in] {dest} Ŀ���ַ
 * @param[in] {src} Դ��ַ
 * @return ���ݴ�������Դ��ַ
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
 * @brief  �ַ����Ƚ�
 * @param[in] {s1} �ַ��� 1
 * @param[in] {s2} �ַ��� 2
 * @return ��С�Ƚ�ֵ
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
 * @brief  дwifi_uart�ֽ�
 * @param[in] {dest} ��������ַƫ��
 * @param[in] {byte} д���ֽ�ֵ
 * @return д����ɺ���ܳ���
 */
unsigned short set_wifi_uart_byte(unsigned short dest, unsigned char byte)
{
    unsigned char *obj = (unsigned char *)wifi_uart_tx_buf + DATA_START + dest;

    *obj = byte;
    dest += 1;

    return dest;
}
/**
 * @brief  дwifi_uart_buffer
 * @param[in] {dest} ��������ַƫ��
 * @param[in] {src} Դ��ַ����Ҫ���͵����ݣ�
 * @param[in] {len} ��Ҫ���͵����ݳ���
 * @return д����ɺ���ܳ���
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
 * @brief  �жϴ��ڽ��ջ������Ƿ�������
 * @param  Null
 * @return �Ƿ�������  0:��/1:��
 */
unsigned char get_queue_total_data(void)
{
  if(queue_in != queue_out)
    return 1;
  else
    return 0;
}
/**
 * @brief  ����У���
 * @param[in] {pack} ����Դָ��
 * @param[in] {pack_len} ����У��ͳ���
 * @return У���
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
 * @brief  ���ڷ���һ������
 * @param[in] {in} ���ͻ���ָ��
 * @param[in] {len} ���ݷ��ͳ���
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
 * @brief  ��wifi���ڷ���ָ������
 * @param[in] {fr_type} ֡����
 * @param[in] {len} ���ݳ���
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
 * @brief  ��wifi���ڷ���һ֡����
 * @param[in] {fr_type} ֡����
 * @param[in] {fr_ver} ֡�汾
 * @param[in] {len} ���ݳ���
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
 * @brief  ��ȡ����1�ֽ�����
 * @param  Null
 * @return Read the data
 */
unsigned char Queue_Read_Byte(void)
{
  unsigned char value;

  if(queue_out != queue_in)
  {
    //������
    if(queue_out >= (unsigned char *)(wifi_uart_rx_buf + sizeof(wifi_uart_rx_buf)))
    {
      //�����Ѿ���ĩβ
      queue_out = (unsigned char *)(wifi_uart_rx_buf);
    }

    value = *queue_out ++;
  }

  return value;
}
/**
 * @brief  ���ڷ�������
 * @param[in] {value} ����Ҫ���͵�1�ֽ�����
 * @return Null
 */
void uart_transmit_output(unsigned char value)
{
    wiced_uart_transmit_bytes(Control_UART,&value,1);
}
/**
 * @brief  ���ڽ��������ݴ洦��
 * @param[in] {value} �����յ���1�ֽ�����
 * @return Null
 * @note   ��MCU���ڴ������е��øú���,�������յ���������Ϊ��������
 */
void uart_receive_input(unsigned char value)
{
    if(1 == queue_out - queue_in) {
        //���ݶ�����
    }else if((queue_in > queue_out) && ((queue_in - queue_out) >= sizeof(wifi_data_process_buf))) {
        //���ݶ�����
    }else {
        //���в���
        if(queue_in >= (unsigned char *)(wifi_uart_rx_buf + sizeof(wifi_uart_rx_buf))) {
            queue_in = (unsigned char *)(wifi_uart_rx_buf);
        }

        *queue_in ++ = value;
    }
}
/**
 * @brief  Э�鴮�ڳ�ʼ������
 * @param  Null
 * @return Null
 * @note   ��MCU��ʼ�������е��øú���
 */
void wifi_protocol_init(void)
{
    //#error " ����main���������wifi_protocol_init()���wifiЭ���ʼ��,��ɾ������"
    queue_in = (unsigned char *)wifi_uart_rx_buf;
    queue_out = (unsigned char *)wifi_uart_rx_buf;
}
/**
 * @brief  wifi�������ݴ������
 * @param  Null
 * @return Null
 * @note   ��MCU������whileѭ���е��øú���
 */
void wifi_uart_service(void)
{
    //#error "��ֱ����main������while(1){}�����wifi_uart_service(),���øú�����Ҫ���κ������ж�,��ɺ�ɾ������"
    static unsigned short rx_in = 0;
    unsigned short offset = 0;
    unsigned short rx_value_len = 0;             //����֡����

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

        //���ݽ������
        if(get_check_sum((unsigned char *)wifi_data_process_buf + offset,rx_value_len - 1) != wifi_data_process_buf[offset + rx_value_len - 1]) {
            //У�����
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
