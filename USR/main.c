#include "pbdata.h"
#include "string.h"
#include "USART.h"
#include "ESP8285.h"
#include "stdlib.h"
#include "RTC.h"

void RCC_Configuration(void);  //配置RCC
void BT_Data_Analysis(void);   //解析蓝牙接收到的数据

u8 USART2_RX_Flag;         //USART2数据接受标志
u8 USART2_RX_BUF[30];      //USART2接受缓存
extern u8 USART3_RX_Flag;         //USART3数据接受标志
u8 BT_Data[30];   //BT的缓存数据
char WIFI_SSID[20] = "SegoPet";   //WIFI名字，为了调试方便，这位置直接写了值
char WIFI_Pass[20] = "Segotech@301";   //WIFI密码，为了调试方便，这位置直接写了值
u8 RTC_TimeCheck_flag = 0;  //RTC寄存器校对标志，0表示已校对；1表示没有校对
u8 ESP8285_WIFIConnect_Flag;  //ESP8285网络连接标志，1成功；0失败

void LED_Configuration(void)
{
	GPIO_InitTypeDef    GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//使能GPIOC时钟
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//设定GPIO为Pin13
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//设置频率50MHZ
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//推挽输出
  GPIO_Init(GPIOC,&GPIO_InitStructure);//初始化GPIOC_Pin13
}

void TIM3_Configuration(void)
{
	TIM_TimeBaseInitTypeDef		TIM_BaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//TIM3时钟线使能
	
	TIM_BaseInitStructure.TIM_Period = 4999;//设置自动重载值为  (4999 + 1) = 5K
  TIM_BaseInitStructure.TIM_Prescaler = 7199;//设置预分频值  72M / (7199 + 1) = 10K
  TIM_BaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;//分频0
  TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数
	TIM_TimeBaseInit(TIM3, &TIM_BaseInitStructure);//初始化TIM3
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);//中断使能
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;//TIM3中断
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//抢占优先级0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;//相应优先级3
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//IRQ通道使能
  NVIC_Init(&NVIC_InitStructure);//初始化NVIC寄存器
	
	TIM_Cmd(TIM3, ENABLE);//使能TIM3
}

int main(void)
{ 
	USART1_Configuration(115200);  //初始化USART1，波特率为115200
	USART_ClearFlag(USART1, USART_FLAG_TC);  //清除USART1的TC状态位，否则会导致第一个字节无法发送
	USART2_Configuration(115200);  //初始化USART2，波特率为115200
	USART_ClearFlag(USART2, USART_FLAG_TC);  //清除USART2的TC状态位，否则会导致第一个字节无法发送
	U2Putstr("AT+NAMESego");//设置蓝牙名字
	//RTC_Configuartion();
	RTC_Init();//初始化RTC
	LED_Configuration();//初始化GPIOC_Pin13
	TIM3_Configuration();//初始化TIM3
	GPIO_SetBits(GPIOC, GPIO_Pin_13);//拉低GPIOC_Pin13
	ESP8285_Init();								//初始化ESP8285
	while(1)
  {
		if(USART2_RX_Flag) //检查到蓝牙数据
		{
			//printf("%s", USART2_RX_BUF);
			//memcpy(BT_Data, USART2_RX_BUF, sizeof(USART2_RX_BUF));  //将USART2_RX_BUF复制到BT_Data
			memset(USART2_RX_BUF, 0x00, sizeof(USART2_RX_BUF)); //清空USART2_RX_BUF
			BT_Data_Analysis();//解析蓝牙数据
			USART2_RX_Flag = 0;//接受标志置0
		}
		if(USART3_RX_Flag)//检查到WiFi数据
		{
			WIFI_Data_Analysis();//解析WiFi数据
		}
//		if(RTC_TimeCheck_flag) //RTC寄存器时间没有校对
//		{
//			if(ESP8285_WIFIConnect_Flag)//ESP8285联网成功
//				Get_Time();//获取网络时间
//			else//ESP8285联网未成功
//				printf("ESP8285 not connect to WIFI\r\n");
//		}
	}
}

/*****************************************
*函数名：    Get_WIFI_SSID
*函数功能：  获取WiFi名称
*函数参数：  无
*函数返回值：无
******************************************/
void Get_WIFI_SSID(void)
{
	u8 i;
	memset(WIFI_SSID, '\0', sizeof(WIFI_SSID));  //清空WIFI_SSID数组
	for(i = 2; i < 18; i++)
	{	
		if(BT_Data[i] != '#')  //判断是否至结束符
			WIFI_SSID[i - 2] = BT_Data[i]; //拷贝数据
		else
			return;   
	}
	U2Putstr("WIFI SSID Got\r\n");
}
/*****************************************
*函数名：    Get_WIFI_Pass
*函数功能：  获取WiFi密码
*函数参数：  无
*函数返回值：无
******************************************/
void Get_WIFI_Pass(void)
{
	u8 i;
	memset(WIFI_Pass, '\0', sizeof(WIFI_Pass));   //清空WIFI_Pass数组
	for(i = 2; i < 18; i++)
	{	
		if(BT_Data[i] != '#') //判断是否至结束符
			WIFI_Pass[i - 2] = BT_Data[i];  //拷贝数据
		else
			return;
	}
	U2Putstr("WIFI Password Got\r\n");
}

/*****************************************
*函数名：    BT_Data_Analysis
*函数功能：  解析蓝牙收到的数据
*函数参数：  无
*函数返回值：无
******************************************/
void BT_Data_Analysis(void)
{
	if(BT_Data[0] == '$')  //判断蓝牙数据帧头
	{
		switch (BT_Data[1])  //判断蓝牙操作位
		{
			case '1':
				ESP8285_SetMode(); break;   //修改ESP8285的模式
			case '2':
				ESP8285_Connect_Network(); break;  //ESP8285连接2.4G WiFi
			case '3':
				Connect_NTP(); break;  //ESP8285连接NTP服务器
			case '4':
				Time_Calibration(); break;  //ESP8285发送请求时间数据到NTP服务器
			case '5':
				Get_Time(); break;//获取RTC时间
			case 'S':
				Get_WIFI_SSID(); break;  //获取WiFi SSID
			case 'P':
				Get_WIFI_Pass(); break; //获取WiFi密码
		}
		memset(BT_Data, 0X00, sizeof(BT_Data));  //清空BT_Data
	}
}

