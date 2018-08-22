#include "stdio.h"
#include "RTC.h"

/*****************************************
*函数名：    U2Putchar
*函数功能：  串口2发送一个字节
*函数参数：  无
*函数返回值：无
******************************************/
void U2Putchar(u8 data)
{
  USART2->DR = data;   //写入数据
	while((USART2->SR&0X40) == 0); //等待发送结束
}

/*****************************************
*函数名：    U2Putstr
*函数功能：  串口2发送字符串
*函数参数：  无
*函数返回值：无
******************************************/
void U2Putstr(char *sp)
{
  while(*sp)    //检查是否结束
	{
		U2Putchar(*sp);  //向USART3中发送1字节
		sp++;  //地址自加
	}
}

/*****************************************
*函数名：    RTC_Configuartion
*函数功能：  配置RTC
*函数参数：  无
*函数返回值：无
******************************************/
void RTC_Configuartion(void)
{
	/*使能PWR和BKP时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP, ENABLE);
  /*使能对后备寄存器的访问*/
	PWR_BackupAccessCmd(ENABLE);
	/*复位BKP寄存器*/
  BKP_DeInit();
  /*使能LSE*/
	RCC_LSEConfig(RCC_LSE_ON);
  /*等待启动完成*/
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) != SET);
	/*将RTC时钟设置为LSE，32.768KHZ的晶振*/
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	/*使能RTC Clock*/
	RCC_RTCCLKCmd(ENABLE);
	/*等待同步*/
	RTC_WaitForSynchro();
	/*等待对RTC寄存器最后的写操作完成*/
	RTC_WaitForLastTask();
	/*使能RTC中断*/
	//RTC_ITConfig(RTC_IT_SEC, ENABLE);
	/*等待对RTC寄存器最后的写操作完成*/
	RTC_WaitForLastTask();
	/*进入RTC配置模式*/
	RTC_EnterConfigMode();
	/*配置预分配值，RTC时钟周期为1S*/
	RTC_SetPrescaler(32767);//  RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1)
	/*等待对RTC寄存器最后的写操作完成*/
	RTC_WaitForLastTask();
	/*退出RTC配置模式*/
	RTC_ExitConfigMode();
	printf("RTC Configuartion is...OK\r\n");
}

/*****************************************
*函数名：    NVIC_Configuartion
*函数功能：  配置RTC_NVIC
*函数参数：  无
*函数返回值：无
******************************************/
void NVIC_Configuartion(void)
{
	
}

/*****************************************
*函数名：    RTC_Init
*函数功能：  初始化RTC
*函数参数：  无
*函数返回值：无
******************************************/
void RTC_Init(void)
{
	if(BKP_ReadBackupRegister(BKP_DR1)!=0x1016) //判断后备寄存器是否是之前写入的值
	{
		RTC_Configuartion();//配置RTC
    RTC_SetCounter(0); //寄存器值清零
    BKP_WriteBackupRegister(BKP_DR1, 0x1016);//向执行的后备寄存器中写入数据
  }
	else
	{
		RTC_WaitForSynchro();//等待RTC寄存器同步完成
    RTC_ITConfig(RTC_IT_SEC, ENABLE);//使能RTC中断
    RTC_WaitForLastTask();//等待对RTC寄存器最后的写操作完成
	}
	RCC_ClearFlag();
	//NVIC_Configuartion();
}

/*****************************************
*函数名：    Get_Time
*函数功能：  获取RTC中的时间
*函数参数：  无
*函数返回值：无
******************************************/
void Get_Time(void)
{
	int hour, minute, second, timeCount;
  char str[20];
	timeCount  = RTC_GetCounter() % 86400; //得到当天已经走过的秒数
	hour = timeCount / 3600 + 8; //得到格林小时，中国 UTC+8
	minute = timeCount % 3600 / 60;//得到分钟数
	second = timeCount % 60;//得到秒数
	printf("RTC Time is: %d:%d:%d\r\n", hour, minute, second);//打印输出
	sprintf(str, "RTC is:%d:%d:%d", hour, minute, second);//格式化字符串
	U2Putstr(str);//串口2（蓝牙）输出
}

/*****************************************
*函数名：    Set_Time
*函数功能：  设置RTC中的时间
*函数参数：  无
*函数返回值：无
******************************************/
void Set_Time(int second)
{
	/*使能PWR和BKP时钟*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP, ENABLE);
  /*使能对后备寄存器的访问*/
	PWR_BackupAccessCmd(ENABLE);
	/*设置RTC寄存器值*/
	RTC_SetCounter(second);
	/*等待对RTC寄存器最后的写操作完成*/
	RTC_WaitForLastTask();
}
