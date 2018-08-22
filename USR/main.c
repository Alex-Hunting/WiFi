#include "pbdata.h"
#include "string.h"
#include "USART.h"
#include "ESP8285.h"
#include "stdlib.h"
#include "RTC.h"

void RCC_Configuration(void);  //����RCC
void BT_Data_Analysis(void);   //�����������յ�������

u8 USART2_RX_Flag;         //USART2���ݽ��ܱ�־
u8 USART2_RX_BUF[30];      //USART2���ܻ���
extern u8 USART3_RX_Flag;         //USART3���ݽ��ܱ�־
u8 BT_Data[30];   //BT�Ļ�������
char WIFI_SSID[20] = "SegoPet";   //WIFI���֣�Ϊ�˵��Է��㣬��λ��ֱ��д��ֵ
char WIFI_Pass[20] = "Segotech@301";   //WIFI���룬Ϊ�˵��Է��㣬��λ��ֱ��д��ֵ
u8 RTC_TimeCheck_flag = 0;  //RTC�Ĵ���У�Ա�־��0��ʾ��У�ԣ�1��ʾû��У��
u8 ESP8285_WIFIConnect_Flag;  //ESP8285�������ӱ�־��1�ɹ���0ʧ��

void LED_Configuration(void)
{
	GPIO_InitTypeDef    GPIO_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//ʹ��GPIOCʱ��
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;//�趨GPIOΪPin13
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//����Ƶ��50MHZ
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;//�������
  GPIO_Init(GPIOC,&GPIO_InitStructure);//��ʼ��GPIOC_Pin13
}

void TIM3_Configuration(void)
{
	TIM_TimeBaseInitTypeDef		TIM_BaseInitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);//TIM3ʱ����ʹ��
	
	TIM_BaseInitStructure.TIM_Period = 4999;//�����Զ�����ֵΪ  (4999 + 1) = 5K
  TIM_BaseInitStructure.TIM_Prescaler = 7199;//����Ԥ��Ƶֵ  72M / (7199 + 1) = 10K
  TIM_BaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;//��Ƶ0
  TIM_BaseInitStructure.TIM_CounterMode = TIM_CounterMode_Up;//���ϼ���
	TIM_TimeBaseInit(TIM3, &TIM_BaseInitStructure);//��ʼ��TIM3
	TIM_ITConfig(TIM3, TIM_IT_Update, ENABLE);//�ж�ʹ��
	
	NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;//TIM3�ж�
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;//��ռ���ȼ�0
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;//��Ӧ���ȼ�3
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;//IRQͨ��ʹ��
  NVIC_Init(&NVIC_InitStructure);//��ʼ��NVIC�Ĵ���
	
	TIM_Cmd(TIM3, ENABLE);//ʹ��TIM3
}

int main(void)
{ 
	USART1_Configuration(115200);  //��ʼ��USART1��������Ϊ115200
	USART_ClearFlag(USART1, USART_FLAG_TC);  //���USART1��TC״̬λ������ᵼ�µ�һ���ֽ��޷�����
	USART2_Configuration(115200);  //��ʼ��USART2��������Ϊ115200
	USART_ClearFlag(USART2, USART_FLAG_TC);  //���USART2��TC״̬λ������ᵼ�µ�һ���ֽ��޷�����
	U2Putstr("AT+NAMESego");//������������
	//RTC_Configuartion();
	RTC_Init();//��ʼ��RTC
	LED_Configuration();//��ʼ��GPIOC_Pin13
	TIM3_Configuration();//��ʼ��TIM3
	GPIO_SetBits(GPIOC, GPIO_Pin_13);//����GPIOC_Pin13
	ESP8285_Init();								//��ʼ��ESP8285
	while(1)
  {
		if(USART2_RX_Flag) //��鵽��������
		{
			//printf("%s", USART2_RX_BUF);
			//memcpy(BT_Data, USART2_RX_BUF, sizeof(USART2_RX_BUF));  //��USART2_RX_BUF���Ƶ�BT_Data
			memset(USART2_RX_BUF, 0x00, sizeof(USART2_RX_BUF)); //���USART2_RX_BUF
			BT_Data_Analysis();//������������
			USART2_RX_Flag = 0;//���ܱ�־��0
		}
		if(USART3_RX_Flag)//��鵽WiFi����
		{
			WIFI_Data_Analysis();//����WiFi����
		}
//		if(RTC_TimeCheck_flag) //RTC�Ĵ���ʱ��û��У��
//		{
//			if(ESP8285_WIFIConnect_Flag)//ESP8285�����ɹ�
//				Get_Time();//��ȡ����ʱ��
//			else//ESP8285����δ�ɹ�
//				printf("ESP8285 not connect to WIFI\r\n");
//		}
	}
}

/*****************************************
*��������    Get_WIFI_SSID
*�������ܣ�  ��ȡWiFi����
*����������  ��
*��������ֵ����
******************************************/
void Get_WIFI_SSID(void)
{
	u8 i;
	memset(WIFI_SSID, '\0', sizeof(WIFI_SSID));  //���WIFI_SSID����
	for(i = 2; i < 18; i++)
	{	
		if(BT_Data[i] != '#')  //�ж��Ƿ���������
			WIFI_SSID[i - 2] = BT_Data[i]; //��������
		else
			return;   
	}
	U2Putstr("WIFI SSID Got\r\n");
}
/*****************************************
*��������    Get_WIFI_Pass
*�������ܣ�  ��ȡWiFi����
*����������  ��
*��������ֵ����
******************************************/
void Get_WIFI_Pass(void)
{
	u8 i;
	memset(WIFI_Pass, '\0', sizeof(WIFI_Pass));   //���WIFI_Pass����
	for(i = 2; i < 18; i++)
	{	
		if(BT_Data[i] != '#') //�ж��Ƿ���������
			WIFI_Pass[i - 2] = BT_Data[i];  //��������
		else
			return;
	}
	U2Putstr("WIFI Password Got\r\n");
}

/*****************************************
*��������    BT_Data_Analysis
*�������ܣ�  ���������յ�������
*����������  ��
*��������ֵ����
******************************************/
void BT_Data_Analysis(void)
{
	if(BT_Data[0] == '$')  //�ж���������֡ͷ
	{
		switch (BT_Data[1])  //�ж���������λ
		{
			case '1':
				ESP8285_SetMode(); break;   //�޸�ESP8285��ģʽ
			case '2':
				ESP8285_Connect_Network(); break;  //ESP8285����2.4G WiFi
			case '3':
				Connect_NTP(); break;  //ESP8285����NTP������
			case '4':
				Time_Calibration(); break;  //ESP8285��������ʱ�����ݵ�NTP������
			case '5':
				Get_Time(); break;//��ȡRTCʱ��
			case 'S':
				Get_WIFI_SSID(); break;  //��ȡWiFi SSID
			case 'P':
				Get_WIFI_Pass(); break; //��ȡWiFi����
		}
		memset(BT_Data, 0X00, sizeof(BT_Data));  //���BT_Data
	}
}

