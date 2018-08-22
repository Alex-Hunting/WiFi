#include "stdio.h"
#include "RTC.h"

/*****************************************
*��������    U2Putchar
*�������ܣ�  ����2����һ���ֽ�
*����������  ��
*��������ֵ����
******************************************/
void U2Putchar(u8 data)
{
  USART2->DR = data;   //д������
	while((USART2->SR&0X40) == 0); //�ȴ����ͽ���
}

/*****************************************
*��������    U2Putstr
*�������ܣ�  ����2�����ַ���
*����������  ��
*��������ֵ����
******************************************/
void U2Putstr(char *sp)
{
  while(*sp)    //����Ƿ����
	{
		U2Putchar(*sp);  //��USART3�з���1�ֽ�
		sp++;  //��ַ�Լ�
	}
}

/*****************************************
*��������    RTC_Configuartion
*�������ܣ�  ����RTC
*����������  ��
*��������ֵ����
******************************************/
void RTC_Configuartion(void)
{
	/*ʹ��PWR��BKPʱ��*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP, ENABLE);
  /*ʹ�ܶԺ󱸼Ĵ����ķ���*/
	PWR_BackupAccessCmd(ENABLE);
	/*��λBKP�Ĵ���*/
  BKP_DeInit();
  /*ʹ��LSE*/
	RCC_LSEConfig(RCC_LSE_ON);
  /*�ȴ��������*/
	while(RCC_GetFlagStatus(RCC_FLAG_LSERDY) != SET);
	/*��RTCʱ������ΪLSE��32.768KHZ�ľ���*/
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	/*ʹ��RTC Clock*/
	RCC_RTCCLKCmd(ENABLE);
	/*�ȴ�ͬ��*/
	RTC_WaitForSynchro();
	/*�ȴ���RTC�Ĵ�������д�������*/
	RTC_WaitForLastTask();
	/*ʹ��RTC�ж�*/
	//RTC_ITConfig(RTC_IT_SEC, ENABLE);
	/*�ȴ���RTC�Ĵ�������д�������*/
	RTC_WaitForLastTask();
	/*����RTC����ģʽ*/
	RTC_EnterConfigMode();
	/*����Ԥ����ֵ��RTCʱ������Ϊ1S*/
	RTC_SetPrescaler(32767);//  RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1)
	/*�ȴ���RTC�Ĵ�������д�������*/
	RTC_WaitForLastTask();
	/*�˳�RTC����ģʽ*/
	RTC_ExitConfigMode();
	printf("RTC Configuartion is...OK\r\n");
}

/*****************************************
*��������    NVIC_Configuartion
*�������ܣ�  ����RTC_NVIC
*����������  ��
*��������ֵ����
******************************************/
void NVIC_Configuartion(void)
{
	
}

/*****************************************
*��������    RTC_Init
*�������ܣ�  ��ʼ��RTC
*����������  ��
*��������ֵ����
******************************************/
void RTC_Init(void)
{
	if(BKP_ReadBackupRegister(BKP_DR1)!=0x1016) //�жϺ󱸼Ĵ����Ƿ���֮ǰд���ֵ
	{
		RTC_Configuartion();//����RTC
    RTC_SetCounter(0); //�Ĵ���ֵ����
    BKP_WriteBackupRegister(BKP_DR1, 0x1016);//��ִ�еĺ󱸼Ĵ�����д������
  }
	else
	{
		RTC_WaitForSynchro();//�ȴ�RTC�Ĵ���ͬ�����
    RTC_ITConfig(RTC_IT_SEC, ENABLE);//ʹ��RTC�ж�
    RTC_WaitForLastTask();//�ȴ���RTC�Ĵ�������д�������
	}
	RCC_ClearFlag();
	//NVIC_Configuartion();
}

/*****************************************
*��������    Get_Time
*�������ܣ�  ��ȡRTC�е�ʱ��
*����������  ��
*��������ֵ����
******************************************/
void Get_Time(void)
{
	int hour, minute, second, timeCount;
  char str[20];
	timeCount  = RTC_GetCounter() % 86400; //�õ������Ѿ��߹�������
	hour = timeCount / 3600 + 8; //�õ�����Сʱ���й� UTC+8
	minute = timeCount % 3600 / 60;//�õ�������
	second = timeCount % 60;//�õ�����
	printf("RTC Time is: %d:%d:%d\r\n", hour, minute, second);//��ӡ���
	sprintf(str, "RTC is:%d:%d:%d", hour, minute, second);//��ʽ���ַ���
	U2Putstr(str);//����2�����������
}

/*****************************************
*��������    Set_Time
*�������ܣ�  ����RTC�е�ʱ��
*����������  ��
*��������ֵ����
******************************************/
void Set_Time(int second)
{
	/*ʹ��PWR��BKPʱ��*/
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR|RCC_APB1Periph_BKP, ENABLE);
  /*ʹ�ܶԺ󱸼Ĵ����ķ���*/
	PWR_BackupAccessCmd(ENABLE);
	/*����RTC�Ĵ���ֵ*/
	RTC_SetCounter(second);
	/*�ȴ���RTC�Ĵ�������д�������*/
	RTC_WaitForLastTask();
}
