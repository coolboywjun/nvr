#ifndef __NVR_NTP_H__
#define __NVR_NTP_H__
#include <nvr_server_headers.h>
/*NTP服务器\端口号*/
#define NVR_NTP_SERVER_IP1       "cn.pool.ntp.org"	/*国家授时中心*/
#define NVR_NTP_SERVER_IP2       "time.windows.com"
#define NVR_NTP_SERVER_IP3	"time.nist.gov"
#define NVR_NTP_PORT_STR        "123"          		/*NTP专用端口号字符串*/
#define NVR_NTP_PORT            123          		/*NTP专用端口号*/

/*NTP包头数据*/
#define NVR_NTP_PCK_LEN				48
#define NVR_NTP_LI 					0
#define NVR_NTP_VN 					3
#define NVR_NTP_MODE 				3
#define NVR_NTP_STRATUM 			0
#define NVR_NTP_POLL 				4
#define NVR_NTP_PREC 				-6

/*时间参数*/
#define NVR_NTP_JAN_1970 	  	0x83aa7e80/* 1900年～1970年之间的时间秒数 */
#define NVR_NTPFRAC(x)			(4294 * (x) + ((1981 * (x)) >> 11))/*NTPFRAC宏是用于计算x*4294.967296的非精确写法，这样就避免了浮点计算*/
#define NVR_NTPUSEC(x)			(((x) >> 12) - 759 * ((((x) >> 10) + 32768) >> 16))/*USEC是上面过程的反过程，表示x/4294.967296*/

#define SUMMERTIMEMONTHBEGIN    3  //!< European Summer Time begin at March     */
#define SUMMERTIMEMONTHEND     10  //!< European Summer Time end at October     */

/*NTP时间格式*/
typedef struct _NVR_NTP_TIME
{
	unsigned int coarse;	//1900年1月1日0时0分以来的秒数
	unsigned int fine;		//微秒的4294.967296(=2^32/10^6)倍
} NVR_NTP_TIME;

/*NTP数据格式包大小为48字节,8位对齐，后续采用字符数组读取方法*/
typedef struct _NVR_NTP_PACKET
{
	unsigned char leap_ver_mode;/*分为三段:前2位为闰秒指示，一般填0;
								  中间3位为NTP协议版本号，为3;		
								  最后3位为通信模式。为3，表示是client.*/

	unsigned char startum;		//NTP服务器阶级，0表示不指定，1表示校准用原子钟。为0。
	char poll;					//表示测试间隔为2^poll_intervals秒,为4。
	char precision;				//表示本机时钟精度为2^local_precision秒。local_precision通常为负数，为-6。

	int root_delay;				//可正可负。2^16表示一秒。具体含义参见RFC1305。4~7
	int root_dispersion;		//只可为正。2^16表示一秒。具体含义参见RFC1305。8~11
	int reference_identifier;	//具体含义参见RFC1305。一般填0。12~15

	/*本地时间*/
	NVR_NTP_TIME reference_timestamp;//具体含义参见RFC1305。一般填0。16~19:20~23
	NVR_NTP_TIME originage_timestamp;//上次发出时刻 24~27:28~31
	NVR_NTP_TIME receive_timestamp;	 //接收时刻 32~35:36~39
	NVR_NTP_TIME transmit_timestamp; //发出时刻 40~43:44~47

}NVR_NTP_PACKET;

/*NTP包存放结构体*/
typedef struct _NVR_NTP_PACKET_STR
{
	NVR_NTP_PACKET ntpPacket;			//NTP数据包
	char ntpPacketBuf[NVR_NTP_PCK_LEN];	//NTP数据包缓冲区

}NVR_NTP_PACKET_STR;

//ntp时间同步控制
typedef enum _NVR_NTP_CTRL_CMD_E_
{
	NVR_NTP_START_UPDATE = 0x01,//开始更新
	
}NVR_NTP_CTRL_CMD_E;

//初始化函数
int  nvr_ntp_init(void);
void nvr_ntp_uninit(void);
//ntp时间同步控制
void nvr_ntp_update();



#endif
