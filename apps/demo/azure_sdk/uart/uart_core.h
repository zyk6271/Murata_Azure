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
#define         WIFI_TEST_CMD                   0x15                            //wifi功能测试
#define         GET_WIFI_STATUS_CMD             0x16                            //获取wifi状态
#define         FACTORY_SET_CMD                 0x17                            //恢复出厂设置
#define         INFORM_DEV_DEV_DEL_CMD          0x18                            //报告移除状态
#define         DEVICE_REBOOT_CMD               0x19                            //重启设备
#define         WIFI_AP_ENABLE_CMD              0x1a                            //开启关闭热点
#define         TELEMETRY_CONTROL_CMD           0x1b                            //数据遥测控制
#define         UPDATE_CONTROL_CMD              0x1c                            //查询子设备列表
#define         UPDATE_START_CMD                0x1d                            //网关启动升级
#define         UPDATE_TRANS_CMD                0x1e                            //网关升级传输中
#define         UPDATE_PROGRSS_CMD              0x1f                            //WIFI升级进度
//=============================================================================
//数据帧类型
//=============================================================================
#define         RAS_SET_CMD                  0x03
#define         RAS_GET_CMD                  0x04
#define         RAS_PUT_CMD                  0x05
#define         CND_GET_CMD                  0x06
#define         NET_GET_CMD                  0x07
#define         BAT_GET_CMD                  0x08
#define         ALA_GET_CMD                  0x09
#define         ALA_SET_CMD                  0x0A
#define         ALR_GET_CMD                  0x0B
#define         ALR_SET_CMD                  0x0C
#define         RSE_SET_CMD                  0x0D
#define         RSE_GET_CMD                  0x0E
#define         RSE_PUT_CMD                  0x0F
#define         RSA_SET_CMD                  0x10
#define         RSA_GET_CMD                  0x11
#define         RSA_PUT_CMD                  0x12
#define         RSI_SET_CMD                  0x13
#define         RSI_GET_CMD                  0x14
#define         RSI_PUT_CMD                  0x15
#define         RSD_SET_CMD                  0x16
#define         RSD_GET_CMD                  0x17
#define         RSD_PUT_CMD                  0x18
#define         CNF_SET_CMD                  0x19
#define         CNF_GET_CMD                  0x1A
#define         CNF_PUT_CMD                  0x1B
#define         CNL_SET_CMD                  0x1C
#define         CNL_GET_CMD                  0x1D
#define         CNL_PUT_CMD                  0x1E
#define         SSE_SET_CMD                  0x1F
#define         SSE_GET_CMD                  0x20
#define         SSE_PUT_CMD                  0x21
#define         SSA_SET_CMD                  0x22
#define         SSA_GET_CMD                  0x23
#define         SSA_PUT_CMD                  0x24
#define         SSD_SET_CMD                  0x25
#define         SSD_GET_CMD                  0x26
#define         SSD_PUT_CMD                  0x27
#define         LNG_SET_CMD                  0x28
#define         LNG_GET_CMD                  0x29
#define         LNG_PUT_CMD                  0x2A
#define         SRN_GET_CMD                  0x2B
#define         SUP_GET_CMD                  0x2C
#define         SUP_PUT_CMD                  0x2D
#define         VER_GET_CMD                  0x2E
#define         COM_SET_CMD                  0x2F
#define         COM_GET_CMD                  0x30
#define         COM_PUT_CMD                  0x31
#define         COA_SET_CMD                  0x32
#define         COA_GET_CMD                  0x33
#define         COA_PUT_CMD                  0x34
#define         COD_SET_CMD                  0x35
#define         COD_GET_CMD                  0x36
#define         COD_PUT_CMD                  0x37
#define         COE_SET_CMD                  0x38
#define         COE_GET_CMD                  0x39
#define         COE_PUT_CMD                  0x3A
#define         CND_PUT_CMD                  0x3B
#define         EMR_SET_CMD                  0x3C
#define         EMR_GET_CMD                  0x3D
#define         RCP_SET_CMD                  0x3E
#define         RCP_GET_CMD                  0x3F

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
