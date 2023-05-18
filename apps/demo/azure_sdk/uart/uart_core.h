#define Control_UART 0

#define WIFI_DATA_PROCESS_LMT           1024             //单包1024byte
#define WIFI_UART_RECV_BUF_LMT          512              //串口数据接收缓存区大小,如MCU的RAM不够,可缩小
#define WIFIR_UART_SEND_BUF_LMT         2048              //根据用户DP数据大小量定，用户可根据实际情况修改
//=============================================================================
/*定义常量*/
//=============================================================================
#ifndef TRUE
#define         TRUE                1
#endif

#ifndef FALSE
#define         FALSE               0
#endif

#ifndef NULL
#define         NULL                ((void *) 0)
#endif

#ifndef SUCCESS
#define         SUCCESS             1
#endif

#ifndef ERROR
#define         ERROR               0
#endif

#ifndef INVALID
#define         INVALID             0xFF
#endif

#ifndef ENABLE
#define         ENABLE                1
#endif
//
#ifndef DISABLE
#define         DISABLE               0
#endif

#ifndef WORD_SWAP
#define WORD_SWAP(X)    (((X << 8) | (X >> 8))&0xFFFF)
#endif
//=============================================================================
//帧的字节命令
//=============================================================================
#define MCU_RX_VER              0x00                                            //接收的协议版本号
#define MCU_TX_VER              0x00                                            //发送的协议版本号
#define PROTOCOL_HEAD           0x07                                            //固定协议头长度
#define FRAME_FIRST             0x55
#define FRAME_SECOND            0xaa
//=============================================================================
//帧的字节命令
//=============================================================================
#define MCU_RX_VER              0x00                                            //接收的协议版本号
#define MCU_TX_VER              0x00                                            //发送的协议版本号
#define PROTOCOL_HEAD           0x07                                            //固定协议头长度
#define FRAME_FIRST             0x55
#define FRAME_SECOND            0xaa
//=============================================================================
//dp数据点类型
//=============================================================================
#define         DP_TYPE_RAW                     0x00        //RAW型
#define         DP_TYPE_BOOL                    0x01        //布尔型
#define         DP_TYPE_VALUE                   0x02        //数值型
#define         DP_TYPE_STRING                  0x03        //字符串型
#define         DP_TYPE_ENUM                    0x04        //枚举型
#define         DP_TYPE_FAULT                   0x05        //故障型
//=============================================================================
//帧的字节顺序
//=============================================================================
#define         HEAD_FIRST                      0
#define         HEAD_SECOND                     1
#define         PROTOCOL_VERSION                2
#define         FRAME_TYPE                      3
#define         LENGTH_HIGH                     4
#define         LENGTH_LOW                      5
#define         DATA_START                      6
//=============================================================================
//数据帧类型
//=============================================================================
#define         PRODUCT_INFO_CMD                0x01                            //产品信息
#define         WORK_MODE_CMD                   0x02                            //查询MCU 设定的模块工作模式
#define         WIFI_STATE_CMD                  0x03                            //wifi工作状态
#define         WIFI_RESET_CMD                  0x04                            //重置wifi
#define         WIFI_MODE_CMD                   0x05                            //选择smartconfig/AP模式
#define         PERMIT_SUBDEVICE_NETIN_CMD      0x06                            //允许子设备入网
#define         CLOSE_SUBDEVICE_NETIN_CMD       0x07                            //关闭子设备入网
#define         SUBDEVICE_ADD_CMD               0x08                            //子设备添加
#define         SUBDEVICE_DELETTE_CMD           0x09                            //子设备删除
#define         HEART_CHECK_CMD                 0x0a                            //心跳检测
#define         STATE_QUERY_CMD                 0x0b                            //状态查询（用于同步设子设备状态）
#define         DATA_ISSUED_CMD                 0x0c                            //命令下发
#define         STATE_UPLOAD_CMD                0x0d                            //状态上报
#define         GROUP_SUBDEV_ADD_CMD            0x0e                            //群组子设备加入
#define         GROUP_SUBDEV_DEL_CMD            0x0f                            //群组子设备删除
#define         GET_ONLINE_TIME_CMD             0x10                            //获取系统时间(格林威治时间)
#define         GET_LOCAL_TIME_CMD              0x11                            //获取本地时间
#define         BATCH_ADD_DEV_CMD               0x12                            //批量添加设备
#define         ADD_DEV_RESULT_CMD              0x13                            //返回添加设备结果
#define         CTRL_GROUP_DOWNLOAD_CMD         0x14                            //控制群组指令下发
#define         WFC_CONTROL_CMD                 0x15                            //配网
#define         GET_WIFI_STATUS_CMD             0x16                            //获取wifi状态
#define         FACTORY_SET_CMD                 0x17                            //恢复出厂设置
#define         DP_UPLOAD_CMD                   0x18                            //DP主动上报
#define         DEVICE_REBOOT_CMD               0x19                            //重启设备
#define         WIFI_AP_CONTROL_CMD             0x1a                            //开启关闭热点
#define         TELEMETRY_CONTROL_CMD           0x1b                            //数据遥测控制
#define         UPDATE_CONTROL_CMD              0x1c                            //查询子设备列表
#define         UPDATE_START_CMD                0x1d                            //网关启动升级
#define         UPDATE_TRANS_CMD                0x1e                            //网关升级传输中
#define         UPDATE_PROGRSS_CMD              0x1f                            //WIFI升级进度
#define         ALARM_CONTROL_CMD               0x20                            //报警上传控制
#define         EVENT_CONTROL_CMD               0x21                            //事件上传控制

//=============================================================================
//数据帧类型
//=============================================================================
#define         RAS_SET_CMD                  0x01
#define         RAS_GET_CMD                  0x02
#define         CND_GET_CMD                  0x03
#define         NET_GET_CMD                  0x04
#define         BAT_GET_CMD                  0x05
#define         RSE_SET_CMD                  0x06
#define         RSE_GET_CMD                  0x07
#define         RSA_SET_CMD                  0x08
#define         RSA_GET_CMD                  0x09
#define         RSI_SET_CMD                  0x0A
#define         RSI_GET_CMD                  0x0B
#define         RSD_SET_CMD                  0x0C
#define         RSD_GET_CMD                  0x0D
#define         CNF_SET_CMD                  0x0E
#define         CNF_GET_CMD                  0x0F
#define         CNL_SET_CMD                  0x10
#define         CNL_GET_CMD                  0x11
#define         SSE_SET_CMD                  0x12
#define         SSE_GET_CMD                  0x13
#define         SSA_SET_CMD                  0x14
#define         SSA_GET_CMD                  0x15
#define         SSD_SET_CMD                  0x16
#define         SSD_GET_CMD                  0x17
#define         LNG_SET_CMD                  0x18
#define         LNG_GET_CMD                  0x19
#define         SRN_GET_CMD                  0x1A
#define         SUP_GET_CMD                  0x1B
#define         VER_GET_CMD                  0x1C
#define         COM_SET_CMD                  0x1D
#define         COM_GET_CMD                  0x1E
#define         COA_SET_CMD                  0x1F
#define         COA_GET_CMD                  0x20
#define         COD_SET_CMD                  0x21
#define         COD_GET_CMD                  0x22
#define         COE_SET_CMD                  0x23
#define         COE_GET_CMD                  0x24
#define         CND_PUT_CMD                  0x25
#define         EMR_SET_CMD                  0x26
#define         EMR_GET_CMD                  0x27
#define         RCP_SET_CMD                  0x28
#define         RCP_GET_CMD                  0x29
#define         VLV_GET_CMD                  0x2A
#define         ALA_GET_CMD                  0x2B
#define         NOT_GET_CMD                  0x2C
#define         WRN_GET_CMD                  0x2D
#define         ALM_GET_CMD                  0x2E
#define         ALW_GET_CMD                  0x2F
#define         ALN_GET_CMD                  0x30
#define         APT_GET_CMD                  0x31
#define         APT_SET_CMD                  0x32
#define         WAD_GET_CMD                  0x33
#define         WAD_SET_CMD                  0x34
#define         WTI_SET_CMD                  0x35
#define         WTI_GET_CMD                  0x36


#define         EVENT_CONFIG_RSE_GET         1<<0
#define         EVENT_CONFIG_RSE_SET         1<<1
#define         EVENT_CONFIG_RSA_GET         1<<2
#define         EVENT_CONFIG_RSA_SET         1<<3
#define         EVENT_CONFIG_RSI_GET         1<<4
#define         EVENT_CONFIG_RSI_SET         1<<5
#define         EVENT_CONFIG_RSD_GET         1<<6
#define         EVENT_CONFIG_RSD_SET         1<<7
#define         EVENT_CONFIG_CNF_GET         1<<8
#define         EVENT_CONFIG_CNF_SET         1<<9
#define         EVENT_CONFIG_CNL_GET         1<<10
#define         EVENT_CONFIG_CNL_SET         1<<11
#define         EVENT_CONFIG_SSE_GET         1<<12
#define         EVENT_CONFIG_SSE_SET         1<<13
#define         EVENT_CONFIG_SSA_GET         1<<14
#define         EVENT_CONFIG_SSA_SET         1<<15
#define         EVENT_CONFIG_SSD_GET         1<<16
#define         EVENT_CONFIG_SSD_SET         1<<17
#define         EVENT_CONFIG_LNG_GET         1<<18
#define         EVENT_CONFIG_LNG_SET         1<<19
#define         EVENT_CONFIG_RCP_GET         1<<20
#define         EVENT_CONFIG_RCP_SET         1<<21
#define         EVENT_CONFIG_EMR_GET         1<<22
#define         EVENT_CONFIG_EMR_SET         1<<23
#define         EVENT_CONFIG_WTI_GET         1<<24
#define         EVENT_CONFIG_WTI_SET         1<<25
#define         EVENT_CONFIG_APT_GET         1<<26
#define         EVENT_CONFIG_APT_SET         1<<27
#define         EVENT_CONFIG_WAD_GET         1<<28
#define         EVENT_CONFIG_WAD_SET         1<<29

#define         EVENT_INFO_COM_GET           1<<0
#define         EVENT_INFO_COM_SET           1<<1
#define         EVENT_INFO_COA_GET           1<<2
#define         EVENT_INFO_COA_SET           1<<3
#define         EVENT_INFO_COD_GET           1<<4
#define         EVENT_INFO_COD_SET           1<<5
#define         EVENT_INFO_COE_GET           1<<6
#define         EVENT_INFO_COE_SET           1<<7
#define         EVENT_INFO_CND_GET           1<<8
#define         EVENT_INFO_SUP_GET           1<<9
#define         EVENT_INFO_VER_GET           1<<10
#define         EVENT_INFO_SRN_GET           1<<11
#define         EVENT_INFO_VLV_GET           1<<12
#define         EVENT_INFO_ALM_GET           1<<13
#define         EVENT_INFO_ALW_GET           1<<14
#define         EVENT_INFO_ALN_GET           1<<15
#define         EVENT_INFO_ALA_GET           1<<16
#define         EVENT_INFO_ALA_SET           1<<17
#define         EVENT_INFO_NOT_GET           1<<18
#define         EVENT_INFO_NOT_SET           1<<19
#define         EVENT_INFO_WRN_GET           1<<20
#define         EVENT_INFO_WRN_SET           1<<21

#define         EVENT_C2D_RST_SET            1<<0
#define         EVENT_C2D_DEF_SET            1<<1
#define         EVENT_C2D_RAS_SET            1<<2

#define         EVENT_TEM_NET_GET            1<<0
#define         EVENT_TEM_BAT_GET            1<<1
#define         EVENT_TEM_ALA_GET            1<<2
#define         EVENT_TEM_ALR_GET            1<<3

unsigned short set_wifi_uart_byte(unsigned short dest, unsigned char byte);
unsigned short set_wifi_uart_buffer(unsigned short dest, const unsigned char *src, unsigned short len);
unsigned char get_queue_total_data(void);
unsigned char get_check_sum(unsigned char *pack, unsigned short pack_len);
void wifi_uart_write_data(unsigned char *in, unsigned short len);
void wifi_uart_write_frame(unsigned char fr_type, unsigned char fr_ver, unsigned short len);
void wifi_uart_write_command_value(unsigned char command, unsigned int data);
unsigned char Queue_Read_Byte(void);
void uart_receive_input(unsigned char value);
void wifi_protocol_init(void);
void wifi_uart_service(void);
void data_handle(unsigned short offset);
