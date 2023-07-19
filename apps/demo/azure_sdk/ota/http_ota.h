#define HTTP_OTA_BUFF_LEN           4096
#define GET_HEADER_BUFSZ            512

#define Retry_Count                 5
#define Retry_Time                  3000

#define OTA_Searching               1<<0
#define Murata_Downloading          1<<1
#define Murata_Download_Done        1<<2
#define Murata_Download_Error       1<<3
#define Murata_No_Upadate           1<<4
#define ST_Downloading              1<<5
#define ST_Download_Done            1<<6
#define ST_Download_Error           1<<7
#define ST_No_Upadate               1<<8
#define Network_Fail                1<<10

#define EVENT_OTA_MURATA_VER_START      1<<0
#define EVENT_OTA_MURATA_BIN_START      1<<1
#define EVENT_OTA_ST_VER_START          1<<2
#define EVENT_OTA_ST_BIN_START          1<<3
