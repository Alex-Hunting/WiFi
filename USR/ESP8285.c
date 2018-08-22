#include "USART.h"
#include "ESP8285.h"
#include "pbdata.h"
#include "string.h"
#include "stdio.h"
#include "RTC.h"

#define AT          "AT\r\n"    //测试ESP8285是否连接
#define CWMODE      "AT+CWMODE=3\r\n"   //切换wifi模式至模式3，STA+AP模式
#define RST         "AT+RST\r\n"        //重启
#define CIFSR       "AT+CIFSR\r\n"      //查询ip
#define WINDOWSNTP  "AT+CIPSTART=\"UDP\",\"time.windows.com\",123\r\n"        //连接Windows的NTP，国内建议不要使用，偶尔会被拦截
#define CHINANTP    "AT+CIPSTART=\"UDP\",\"cn.ntp.org.cn\",123\r\n"  //中国NTP，推荐国内使用,其他请参考：http://ntp.org.cn/pool.php
#define CIPMODE0    "AT+CIPMODE=0\r\n"      //单链接
#define CIPSEND48   "AT+CIPSEND=48\r\n"      //发送48字节数据
#define CIPMODE1    "AT+CIPMODE=1\r\n"      //透传模式
#define CIPSEND     "AT+CIPSEND\r\n"        //发送
#define CIPSTATUS   "AT+CIPSTATUS\r\n"      //网络状态查询

extern char WIFI_SSID[20];    //WIFI名字
extern char WIFI_Pass[20];    //WIFI密码
extern u8 USART3_RX_Flag;     //USART3数据接受标志
extern u8 USART3_RX_BUF[128]; //USART3接受缓存
extern u8 USART3_MAX_BUF;   //设置WiFi接受数据的大小
extern u8 ESP8285_WIFIConnect_Flag;  //ESP8285网络连接标志，1成功；0失败
u8 state;   //ESP8285返回状态

/*****************************************
*函数名：    ESP8285_Log
*函数功能：  串口打印ESP8285状态输出
*函数参数：  char *str:当前操作名称；u8 state:返回的状态
*函数返回值：无
******************************************/
void ESP8285_Log(char *str, u8 state)
{
  if(state == 1)   //AT指令发送成功，ESP8285响应OK
	{
		printf("%s is...OK\r\n", str);
	}
	else if(state == 2)  //AT指令发送成功，ESP8285响应ERROR
	{
		printf("%s is...Error\r\n", str);
	}
	else if(state == 3)  //ESP8285超时未响应
	{
		printf("%s is...Time Out\r\n", str);
	}
}

/*****************************************
*函数名：    U3Putchar
*函数功能：  串口3发送1字节
*函数参数：  u8 data
*函数返回值：无
******************************************/
void U3Putchar(u8 data)
{
  USART3->DR = data;   //写入数据
	while((USART3->SR&0X40) == 0); //等待发送结束
}

/*****************************************
*函数名：    U3Putstr
*函数功能：  串口3发送字符串
*函数参数：  无
*函数返回值：无
******************************************/
void U3Putstr(char *sp)
{
  while(*sp)    //检查是否结束
	{
		U3Putchar(*sp);  //向USART3中发送1字节
		sp++;  //地址自加
	}
}

/*****************************************
*函数名：    ESP8285_CmdSend
*函数功能：  ESP8285发送命令
*函数参数：  char *strAT指令，char *result成功后返回的结果，int timeout等待时间
*函数返回值：成功1，失败2，超时3
******************************************/
u8 ESP8285_CmdSend(char *str, char *result, int timeout)
{	
	state = 0;  //设置状态位为0
	U3Putstr(str);  //发送AT指令
	while(timeout--)  //等待响应
	{
		if(strstr((const char*)USART3_RX_BUF, (const char*)result) != NULL)  //查询result是否为USART3_RX_BUF的子集，是返回首地址，否返回NULL
		{
			memset(USART3_RX_BUF, 0, sizeof(USART3_RX_BUF));	  //清空USART3_RX_BUF
			state = 1;   //状态位置1
			return 1;  //返回1
		}
		else if(strstr((const char*)USART3_RX_BUF, "ERROR") != NULL)   //查询ERROR是否为USART3_RX_BUF的子集，是返回首地址，否返回NULL
		{
			memset(USART3_RX_BUF, 0, sizeof(USART3_RX_BUF));//清空USART3_RX_BUF
			state = 2;//状态位置2
			return 2;//返回2
		}
		delay_ms(2); //延时2MS
	}
	memset(USART3_RX_BUF, 0, sizeof(USART3_RX_BUF));//清空USART3_RX_BUF
	state = 3;//状态位置3
	return 3;//返回3
}
/*****************************************
*函数名：    ESP8285_Init
*函数功能：  初始化ESP8285
*函数参数：  无
*函数返回值：无
******************************************/
u8 Time_Send_Buf[48];
void ESP8285_Init(void)
{
	USART3_Configuration(115200);     //初始化串口3波特率为115200
	USART_ClearFlag(USART3, USART_FLAG_TC); //清除USART3的TC状态位，否则会导致第一个字节无法发送
//	U3Putstr("AT+RESTORE\r\n");    //模块回复出厂设置
	delay_ms(10);//需要测试，主要用于等待ESP8285启动
	memset(Time_Send_Buf, 0X00, sizeof(Time_Send_Buf)); //清空Time_Send_Buf
	Time_Send_Buf[0] = 0XE3;  //设置Time_Send_Buf第一个字节为  0XE3
	ESP8285_Log("Check ESP8285", ESP8285_CmdSend(AT, "OK", 10));   //检查ESP8285是否成功连接
	if(state == 1)
		printf("ESP8285 Init...OK\r\n");
	else
		printf("ESP8285 Init...Error\r\n");
}

/*****************************************
*函数名：    ESP8285_Init
*函数功能：  初始化ESP8285
*函数参数：  无
*函数返回值：无
******************************************/
void ESP8285_SetMode(void)
{
	U2Putstr("Change mode...");
	ESP8285_Log("Change mode", ESP8285_CmdSend(CWMODE, "OK", 10));   //设置ESP8285为模式3，AP和Station模式
	if(state == 1)   //判断设置状态
	{
		U2Putstr("Restart...");
		state = ESP8285_CmdSend(RST, "OK", 10);
		if(state == 1)
		{
			printf("Restart OK\r\n");
			U2Putstr("Restart OK");
		}
		//ESP8285_Log("ESP8285 Restart", ESP8285_CmdSend(RST, "OK", 10));  //重启ESP8285，更改模式后需要重启
	}
	//U3Putstr("AT+CWMODE=3\r\n");//更改WiFi为模式3，AP和Station模式，该指令需要重启生效
	//delay_ms(50);
	//U3Putstr("AT+RST\r\n"); //发送重启AT指令，不清楚为什么不能检测到WIFI返回的OK，但是模块确实重启，上面的方法可以检测
	//delay_ms(10);
}

/*可以工作，但没有使用，AT指令建议使用ESP8285_CmdSend发送
void ESP8255_QueryIP(void)
{
	U3Putstr("AT+CIFSR\r\n");//查询模块IP
}

void ESP8285_QueryMode(void)
{
	U3Putstr("AT+CWMODE?\r\n");//查询模块当前工作模式
}
void ESP8285_RESTORE(void)
{
	U3Putstr("AT+RESTORE\r\n");//恢复出厂设置
}*/

/*****************************************
*函数名：    Connect_Network
*函数功能：  ESP8285连接2.4G WIFI
*函数参数：  无
*函数返回值：无
******************************************/
char Connect_Network[50];
void ESP8285_Connect_Network(void)
{
//	u8 i;
	memset(Connect_Network, '\0',sizeof(Connect_Network));  //清空Connect_Network
	strcpy(Connect_Network, "AT+CWJAP=\"");//复制"AT+CWJAP=\""到Connect_Network
	strcat(Connect_Network, WIFI_SSID);   //复制WIFI_SSID到Connect_Network末尾
	strcat(Connect_Network, "\",\"");     //复制"\",\""到Connect_Network末尾
	strcat(Connect_Network, WIFI_Pass);   //复制WIFI_Pass到Connect_Network末尾
	strcat(Connect_Network, "\"\r\n");    //复制"\"\r\n"到Connect_Network末尾
/*这样也能发送，只是不好检查ESP8285返回的状态
	for(i = 0; i <= 50; i++)
	{
		if(Connect_Network != '\0')
		{	
			U3Putchar(Connect_Network[i]);
		}
	}
	*/
	printf("WIFI Connect...\r\n");
	U2Putstr("WIFI Connect...");
	state = ESP8285_CmdSend(Connect_Network, "OK", 3000);
	if(state)//WIFI连接成功
	{
		ESP8285_WIFIConnect_Flag = 1;//WIFI连接标志置1
		printf("Connect WIFI is...OK\r\n");
		U2Putstr("WIFI is OK!");
	}
	else//WIFI连接失败
	{
		printf("Connect WIFI is...Error\r\n");
		U2Putstr("WIFI is Error!");
	}
	//ESP8285_Log("Connect WIFI is", ESP8285_CmdSend(Connect_Network, "OK", 3000));  //ESP8285连接至WiFi，只能连2.4G
}

/*****************************************
*函数名：    Connect_NTP
*函数功能：  ESP8285连接到NTP服务器
*函数参数：  无
*函数返回值：无
******************************************/
void Connect_NTP(void)
{
//	U3Putstr("AT+CIPMUX=0\r\n");//设置ESP8285为单路连接
//	delay_ms(5);
//	U3Putstr("AT+CIPSTART=\"UDP\",\"time.windows.com\",123\r\n");	//连接到NTP服务器
	ESP8285_Log("Set CIPMUX=0", ESP8285_CmdSend(CIPMODE0, "OK", 10));//设置ESP8285为单路连接
	if(state == 1)
		{
			ESP8285_Log("Connect NTP", ESP8285_CmdSend(CHINANTP, "OK", 1000));//连接到NTP服务器
			U2Putstr("Connect NTP OK");
		}
}

/*****************************************
*函数名：    Time_Calibration
*函数功能：  获取网络时间
*函数参数：  无
*函数返回值：无
******************************************/
void Time_Calibration(void)
{
	u8 i;
	int timeout = 200;
	//U2Putstr("Get NTP time...");//串口2发送数据，测试使用
	ESP8285_Log("Set CIPSEND=48", ESP8285_CmdSend(CIPSEND48, "OK", 10));  //设定单路链接长度为48字节
	//U3Putstr("AT+CIPSEND=48\r\n");  //设定单路链接长度为48字节
	while(timeout--) //等待模块响应
	{
		if(state == 1) //模块响应成功
		{
			USART3_MAX_BUF = 48;   //设置WiFi接受数据的大小，用于中断接受结束判断
			USART_ClearFlag(USART3, USART_FLAG_TC); //清除USART3的TC状态位，防止第一个字节无法发送
			for(i = 0; i < 48; i++)
			{
				USART3->DR = Time_Send_Buf[i];   //发送Time_Send_Buf
				while((USART3->SR&0X40) == 0); //等待发送结束
			}	
			timeout = 0;  //设置timeout为0，退出while循环
		}
		delay_us(10); //延时10us
	}
}

/*****************************************
*函数名：    WIFI_Data_Analysis
*函数功能：  分析接收到的ESP8285数据
*函数参数：  无
*函数返回值：无
******************************************/
unsigned int secTemp;  //当前时间与1900年1月1日0时0分0秒之间的时间差（单位：秒）
void WIFI_Data_Analysis(void)
{
	USART3_RX_Flag = 0;  //接受标志置0
	
	/*NTP返回数据为：+IPD数据，   +的ASCII码为0X2B，I为0X49，P为0X50，D为0X44*/
	if(USART3_RX_BUF[0] == 0x2B && USART3_RX_BUF[1] == 0x49)  //判断是否为时间数据，
		{
			/*计算时间，由2个16进制数组成*/
			secTemp = (u8)USART3_RX_BUF[40];
			secTemp <<= 8;
			secTemp |= (u8)USART3_RX_BUF[41];
			secTemp <<= 8;
			secTemp |= (u8)USART3_RX_BUF[42];
			secTemp <<= 8;
			secTemp |= USART3_RX_BUF[43];
			USART3_MAX_BUF = 128;   //设置WiFi接受数据的大小，获取时间前修改成了48

			Time_Analysis();//解析时间
		}
		/*当接收到到的数据不是时间数据时，可在此处理，其他位置的代码也要改*/
		/*
		else if(.....)
		{
		}
		*/
}

/*****************************************
*函数名：    ESP8285_Init
*函数功能：  初始化ESP8285
*函数参数：  无
*函数返回值：无
******************************************/
u8 time_flag = 0;//测试使用
int Time_Analysis(void)
{
	char str[20];//USART2输出字符串
	int hour, minute, second;
	secTemp = secTemp % 86400; //得到当天已经走过的秒数
	/*测试使用*/
	if(time_flag == 0)//仅上电后第一次获取NTP时间时，给RTC赋值
	{
		U2Putstr("Set RCC");
		Set_Time(secTemp);
		time_flag = 1;
	}
	/*************************/
	//Set_Time(secTemp);
	hour = secTemp / 3600 + 8; //得到格林小时，中国 UTC+8
	minute = secTemp % 3600 / 60;//得到分钟数
	second = secTemp % 60;//得到秒数
	//printf("NTP Time is: %d:%d:%d\r\n", hour, minute, second);//打印输出
	/*以下为测试使用*/
	sprintf(str, "NTP is:%d:%d:%d", hour, minute, second);  //格式化字符串
	U2Putstr(str);//串口2，蓝牙发送
	
	return secTemp;
}
