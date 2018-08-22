#include "USART.h"
#include "ESP8285.h"
#include "pbdata.h"
#include "string.h"
#include "stdio.h"
#include "RTC.h"

#define AT          "AT\r\n"    //����ESP8285�Ƿ�����
#define CWMODE      "AT+CWMODE=3\r\n"   //�л�wifiģʽ��ģʽ3��STA+APģʽ
#define RST         "AT+RST\r\n"        //����
#define CIFSR       "AT+CIFSR\r\n"      //��ѯip
#define WINDOWSNTP  "AT+CIPSTART=\"UDP\",\"time.windows.com\",123\r\n"        //����Windows��NTP�����ڽ��鲻Ҫʹ�ã�ż���ᱻ����
#define CHINANTP    "AT+CIPSTART=\"UDP\",\"cn.ntp.org.cn\",123\r\n"  //�й�NTP���Ƽ�����ʹ��,������ο���http://ntp.org.cn/pool.php
#define CIPMODE0    "AT+CIPMODE=0\r\n"      //������
#define CIPSEND48   "AT+CIPSEND=48\r\n"      //����48�ֽ�����
#define CIPMODE1    "AT+CIPMODE=1\r\n"      //͸��ģʽ
#define CIPSEND     "AT+CIPSEND\r\n"        //����
#define CIPSTATUS   "AT+CIPSTATUS\r\n"      //����״̬��ѯ

extern char WIFI_SSID[20];    //WIFI����
extern char WIFI_Pass[20];    //WIFI����
extern u8 USART3_RX_Flag;     //USART3���ݽ��ܱ�־
extern u8 USART3_RX_BUF[128]; //USART3���ܻ���
extern u8 USART3_MAX_BUF;   //����WiFi�������ݵĴ�С
extern u8 ESP8285_WIFIConnect_Flag;  //ESP8285�������ӱ�־��1�ɹ���0ʧ��
u8 state;   //ESP8285����״̬

/*****************************************
*��������    ESP8285_Log
*�������ܣ�  ���ڴ�ӡESP8285״̬���
*����������  char *str:��ǰ�������ƣ�u8 state:���ص�״̬
*��������ֵ����
******************************************/
void ESP8285_Log(char *str, u8 state)
{
  if(state == 1)   //ATָ��ͳɹ���ESP8285��ӦOK
	{
		printf("%s is...OK\r\n", str);
	}
	else if(state == 2)  //ATָ��ͳɹ���ESP8285��ӦERROR
	{
		printf("%s is...Error\r\n", str);
	}
	else if(state == 3)  //ESP8285��ʱδ��Ӧ
	{
		printf("%s is...Time Out\r\n", str);
	}
}

/*****************************************
*��������    U3Putchar
*�������ܣ�  ����3����1�ֽ�
*����������  u8 data
*��������ֵ����
******************************************/
void U3Putchar(u8 data)
{
  USART3->DR = data;   //д������
	while((USART3->SR&0X40) == 0); //�ȴ����ͽ���
}

/*****************************************
*��������    U3Putstr
*�������ܣ�  ����3�����ַ���
*����������  ��
*��������ֵ����
******************************************/
void U3Putstr(char *sp)
{
  while(*sp)    //����Ƿ����
	{
		U3Putchar(*sp);  //��USART3�з���1�ֽ�
		sp++;  //��ַ�Լ�
	}
}

/*****************************************
*��������    ESP8285_CmdSend
*�������ܣ�  ESP8285��������
*����������  char *strATָ�char *result�ɹ��󷵻صĽ����int timeout�ȴ�ʱ��
*��������ֵ���ɹ�1��ʧ��2����ʱ3
******************************************/
u8 ESP8285_CmdSend(char *str, char *result, int timeout)
{	
	state = 0;  //����״̬λΪ0
	U3Putstr(str);  //����ATָ��
	while(timeout--)  //�ȴ���Ӧ
	{
		if(strstr((const char*)USART3_RX_BUF, (const char*)result) != NULL)  //��ѯresult�Ƿ�ΪUSART3_RX_BUF���Ӽ����Ƿ����׵�ַ���񷵻�NULL
		{
			memset(USART3_RX_BUF, 0, sizeof(USART3_RX_BUF));	  //���USART3_RX_BUF
			state = 1;   //״̬λ��1
			return 1;  //����1
		}
		else if(strstr((const char*)USART3_RX_BUF, "ERROR") != NULL)   //��ѯERROR�Ƿ�ΪUSART3_RX_BUF���Ӽ����Ƿ����׵�ַ���񷵻�NULL
		{
			memset(USART3_RX_BUF, 0, sizeof(USART3_RX_BUF));//���USART3_RX_BUF
			state = 2;//״̬λ��2
			return 2;//����2
		}
		delay_ms(2); //��ʱ2MS
	}
	memset(USART3_RX_BUF, 0, sizeof(USART3_RX_BUF));//���USART3_RX_BUF
	state = 3;//״̬λ��3
	return 3;//����3
}
/*****************************************
*��������    ESP8285_Init
*�������ܣ�  ��ʼ��ESP8285
*����������  ��
*��������ֵ����
******************************************/
u8 Time_Send_Buf[48];
void ESP8285_Init(void)
{
	USART3_Configuration(115200);     //��ʼ������3������Ϊ115200
	USART_ClearFlag(USART3, USART_FLAG_TC); //���USART3��TC״̬λ������ᵼ�µ�һ���ֽ��޷�����
//	U3Putstr("AT+RESTORE\r\n");    //ģ��ظ���������
	delay_ms(10);//��Ҫ���ԣ���Ҫ���ڵȴ�ESP8285����
	memset(Time_Send_Buf, 0X00, sizeof(Time_Send_Buf)); //���Time_Send_Buf
	Time_Send_Buf[0] = 0XE3;  //����Time_Send_Buf��һ���ֽ�Ϊ  0XE3
	ESP8285_Log("Check ESP8285", ESP8285_CmdSend(AT, "OK", 10));   //���ESP8285�Ƿ�ɹ�����
	if(state == 1)
		printf("ESP8285 Init...OK\r\n");
	else
		printf("ESP8285 Init...Error\r\n");
}

/*****************************************
*��������    ESP8285_Init
*�������ܣ�  ��ʼ��ESP8285
*����������  ��
*��������ֵ����
******************************************/
void ESP8285_SetMode(void)
{
	U2Putstr("Change mode...");
	ESP8285_Log("Change mode", ESP8285_CmdSend(CWMODE, "OK", 10));   //����ESP8285Ϊģʽ3��AP��Stationģʽ
	if(state == 1)   //�ж�����״̬
	{
		U2Putstr("Restart...");
		state = ESP8285_CmdSend(RST, "OK", 10);
		if(state == 1)
		{
			printf("Restart OK\r\n");
			U2Putstr("Restart OK");
		}
		//ESP8285_Log("ESP8285 Restart", ESP8285_CmdSend(RST, "OK", 10));  //����ESP8285������ģʽ����Ҫ����
	}
	//U3Putstr("AT+CWMODE=3\r\n");//����WiFiΪģʽ3��AP��Stationģʽ����ָ����Ҫ������Ч
	//delay_ms(50);
	//U3Putstr("AT+RST\r\n"); //��������ATָ������Ϊʲô���ܼ�⵽WIFI���ص�OK������ģ��ȷʵ����������ķ������Լ��
	//delay_ms(10);
}

/*���Թ�������û��ʹ�ã�ATָ���ʹ��ESP8285_CmdSend����
void ESP8255_QueryIP(void)
{
	U3Putstr("AT+CIFSR\r\n");//��ѯģ��IP
}

void ESP8285_QueryMode(void)
{
	U3Putstr("AT+CWMODE?\r\n");//��ѯģ�鵱ǰ����ģʽ
}
void ESP8285_RESTORE(void)
{
	U3Putstr("AT+RESTORE\r\n");//�ָ���������
}*/

/*****************************************
*��������    Connect_Network
*�������ܣ�  ESP8285����2.4G WIFI
*����������  ��
*��������ֵ����
******************************************/
char Connect_Network[50];
void ESP8285_Connect_Network(void)
{
//	u8 i;
	memset(Connect_Network, '\0',sizeof(Connect_Network));  //���Connect_Network
	strcpy(Connect_Network, "AT+CWJAP=\"");//����"AT+CWJAP=\""��Connect_Network
	strcat(Connect_Network, WIFI_SSID);   //����WIFI_SSID��Connect_Networkĩβ
	strcat(Connect_Network, "\",\"");     //����"\",\""��Connect_Networkĩβ
	strcat(Connect_Network, WIFI_Pass);   //����WIFI_Pass��Connect_Networkĩβ
	strcat(Connect_Network, "\"\r\n");    //����"\"\r\n"��Connect_Networkĩβ
/*����Ҳ�ܷ��ͣ�ֻ�ǲ��ü��ESP8285���ص�״̬
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
	if(state)//WIFI���ӳɹ�
	{
		ESP8285_WIFIConnect_Flag = 1;//WIFI���ӱ�־��1
		printf("Connect WIFI is...OK\r\n");
		U2Putstr("WIFI is OK!");
	}
	else//WIFI����ʧ��
	{
		printf("Connect WIFI is...Error\r\n");
		U2Putstr("WIFI is Error!");
	}
	//ESP8285_Log("Connect WIFI is", ESP8285_CmdSend(Connect_Network, "OK", 3000));  //ESP8285������WiFi��ֻ����2.4G
}

/*****************************************
*��������    Connect_NTP
*�������ܣ�  ESP8285���ӵ�NTP������
*����������  ��
*��������ֵ����
******************************************/
void Connect_NTP(void)
{
//	U3Putstr("AT+CIPMUX=0\r\n");//����ESP8285Ϊ��·����
//	delay_ms(5);
//	U3Putstr("AT+CIPSTART=\"UDP\",\"time.windows.com\",123\r\n");	//���ӵ�NTP������
	ESP8285_Log("Set CIPMUX=0", ESP8285_CmdSend(CIPMODE0, "OK", 10));//����ESP8285Ϊ��·����
	if(state == 1)
		{
			ESP8285_Log("Connect NTP", ESP8285_CmdSend(CHINANTP, "OK", 1000));//���ӵ�NTP������
			U2Putstr("Connect NTP OK");
		}
}

/*****************************************
*��������    Time_Calibration
*�������ܣ�  ��ȡ����ʱ��
*����������  ��
*��������ֵ����
******************************************/
void Time_Calibration(void)
{
	u8 i;
	int timeout = 200;
	//U2Putstr("Get NTP time...");//����2�������ݣ�����ʹ��
	ESP8285_Log("Set CIPSEND=48", ESP8285_CmdSend(CIPSEND48, "OK", 10));  //�趨��·���ӳ���Ϊ48�ֽ�
	//U3Putstr("AT+CIPSEND=48\r\n");  //�趨��·���ӳ���Ϊ48�ֽ�
	while(timeout--) //�ȴ�ģ����Ӧ
	{
		if(state == 1) //ģ����Ӧ�ɹ�
		{
			USART3_MAX_BUF = 48;   //����WiFi�������ݵĴ�С�������жϽ��ܽ����ж�
			USART_ClearFlag(USART3, USART_FLAG_TC); //���USART3��TC״̬λ����ֹ��һ���ֽ��޷�����
			for(i = 0; i < 48; i++)
			{
				USART3->DR = Time_Send_Buf[i];   //����Time_Send_Buf
				while((USART3->SR&0X40) == 0); //�ȴ����ͽ���
			}	
			timeout = 0;  //����timeoutΪ0���˳�whileѭ��
		}
		delay_us(10); //��ʱ10us
	}
}

/*****************************************
*��������    WIFI_Data_Analysis
*�������ܣ�  �������յ���ESP8285����
*����������  ��
*��������ֵ����
******************************************/
unsigned int secTemp;  //��ǰʱ����1900��1��1��0ʱ0��0��֮���ʱ����λ���룩
void WIFI_Data_Analysis(void)
{
	USART3_RX_Flag = 0;  //���ܱ�־��0
	
	/*NTP��������Ϊ��+IPD���ݣ�   +��ASCII��Ϊ0X2B��IΪ0X49��PΪ0X50��DΪ0X44*/
	if(USART3_RX_BUF[0] == 0x2B && USART3_RX_BUF[1] == 0x49)  //�ж��Ƿ�Ϊʱ�����ݣ�
		{
			/*����ʱ�䣬��2��16���������*/
			secTemp = (u8)USART3_RX_BUF[40];
			secTemp <<= 8;
			secTemp |= (u8)USART3_RX_BUF[41];
			secTemp <<= 8;
			secTemp |= (u8)USART3_RX_BUF[42];
			secTemp <<= 8;
			secTemp |= USART3_RX_BUF[43];
			USART3_MAX_BUF = 128;   //����WiFi�������ݵĴ�С����ȡʱ��ǰ�޸ĳ���48

			Time_Analysis();//����ʱ��
		}
		/*�����յ��������ݲ���ʱ������ʱ�����ڴ˴�������λ�õĴ���ҲҪ��*/
		/*
		else if(.....)
		{
		}
		*/
}

/*****************************************
*��������    ESP8285_Init
*�������ܣ�  ��ʼ��ESP8285
*����������  ��
*��������ֵ����
******************************************/
u8 time_flag = 0;//����ʹ��
int Time_Analysis(void)
{
	char str[20];//USART2����ַ���
	int hour, minute, second;
	secTemp = secTemp % 86400; //�õ������Ѿ��߹�������
	/*����ʹ��*/
	if(time_flag == 0)//���ϵ���һ�λ�ȡNTPʱ��ʱ����RTC��ֵ
	{
		U2Putstr("Set RCC");
		Set_Time(secTemp);
		time_flag = 1;
	}
	/*************************/
	//Set_Time(secTemp);
	hour = secTemp / 3600 + 8; //�õ�����Сʱ���й� UTC+8
	minute = secTemp % 3600 / 60;//�õ�������
	second = secTemp % 60;//�õ�����
	//printf("NTP Time is: %d:%d:%d\r\n", hour, minute, second);//��ӡ���
	/*����Ϊ����ʹ��*/
	sprintf(str, "NTP is:%d:%d:%d", hour, minute, second);  //��ʽ���ַ���
	U2Putstr(str);//����2����������
	
	return secTemp;
}
