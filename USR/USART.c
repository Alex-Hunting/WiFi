#include "stm32f10x_usart.h"
#include "misc.h"
#include "stdio.h"
#include "USART.h"

/*****************************************
*��������    fputc
*�������ܣ�  �ض���c�⺯��printf��USART1
*����������  ��
*��������ֵ����
*���ã�      ��printf����
******************************************/
int fputc(int ch,FILE *f)
{
	USART_SendData(USART1,(u8)ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	return ch;
}

/*****************************************
*��������    UART1_Configuration
*�������ܣ�  ��ʼ��USART1
*����������  ��ʼ�������ʵ�ֵ
*��������ֵ����
******************************************/
void USART1_Configuration(int Baudrate)
{
	GPIO_InitTypeDef    GPIO_InitStructure;     
	USART_InitTypeDef   USART_InitStructure; 
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);  //USART1ʱ��ʹ�ܣ�GPIOAʱ��ʹ��
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;   //USART1 TX
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    //����Ƶ��50MHZ
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		//�����������
  GPIO_Init(GPIOA,&GPIO_InitStructure);                //GPIOA

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  //USART1 RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    //���ÿ�©����
	GPIO_Init(GPIOA,&GPIO_InitStructure);                //GPIOA   
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3  
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;      //�����ȼ�3  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQͨ��ʹ��
  NVIC_Init(&NVIC_InitStructure); //���ݲ�����ʼ��NVIC
		
  USART_InitStructure.USART_BaudRate = Baudrate;                //�����趨
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;   //����λ8λ
  USART_InitStructure.USART_StopBits = USART_StopBits_1;        //ֹͣλ1λ
  USART_InitStructure.USART_Parity = USART_Parity_No;           //��У��λ
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   // ��Ӳ������
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;   //�շ�ģʽ
  USART_Init(USART1, &USART_InitStructure);                         //���ô���
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                    //ʹ�ܽ����ж�
  USART_Cmd(USART1,ENABLE);                                         //����USART1         
	printf("USART1 Init...OK\r\n");
}

/*****************************************
*��������    Sendware
*�������ܣ�  ���η��ͺ�������ʹ��ɽ��๦�ܵ�����ʹ��������ʾ����
*����������  ���������飬�����ֽڴ�С���Ƽ�ʹ�ø�ʽ      Sendware((uint8_t*)addr, sizeof(addr));
*��������ֵ����
******************************************/
void Sendware(uint8_t *wareaddr, uint32_t waresize)
{
	USART1->DR = 0X03;       //����֡ͷ1  0X03
	while((USART1->SR&0X40) == 0);
	USART1->DR = 0XFC;       //����֡ͷ2  0XFC
	while((USART1->SR&0X40) == 0);

	/*���Ͳ�������*/
	while(waresize--)      
	{
		USART1->DR = *wareaddr;
		while((USART1->SR&0X40) == 0);
		wareaddr++;
	}
	
	USART1->DR = 0XFC; //���ͽ�β��ʶ1
	while((USART1->SR&0X40) == 0);
	USART1->DR = 0X03; //���ͽ�β��ʶ2
	while((USART1->SR&0X40) == 0);
}

/*****************************************
*��������    UART2_Configuration
*�������ܣ�  ��ʼ��USART2
*����������  ��ʼ�������ʵ�ֵ
*��������ֵ����
******************************************/
void USART2_Configuration(int Baudrate)
{
	GPIO_InitTypeDef    GPIO_InitStructure;     
	USART_InitTypeDef   USART_InitStructure; 
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //USART1ʱ��ʹ�ܣ�GPIOAʱ��ʹ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//??USART2??
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;   //USART1 TX
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    //����Ƶ��50MHZ
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		//�����������
  GPIO_Init(GPIOA,&GPIO_InitStructure);                //GPIOA

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;  //USART1 RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    //���ÿ�©����
	GPIO_Init(GPIOA,&GPIO_InitStructure);                //GPIOA   
	
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//��ռ���ȼ�0  
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;      //�����ȼ�0 
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQͨ��ʹ��
  NVIC_Init(&NVIC_InitStructure); //���ݲ�����ʼ��NVIC
		
  USART_InitStructure.USART_BaudRate = Baudrate;                //�����趨
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;   //����λ8λ
  USART_InitStructure.USART_StopBits = USART_StopBits_1;        //ֹͣλ1λ
  USART_InitStructure.USART_Parity = USART_Parity_No;           //��У��λ
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   // ��Ӳ������
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;   //�շ�ģʽ
  USART_Init(USART2, &USART_InitStructure);                         //���ô���
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);                    //ʹ�ܽ����ж�
  USART_Cmd(USART2,ENABLE);                                         //����USART2      
	printf("USART2 Init...OK\r\n");
}

/*****************************************
*��������    UART2_Configuration
*�������ܣ�  ��ʼ��USART2
*����������  ��ʼ�������ʵ�ֵ
*��������ֵ����
******************************************/
void USART3_Configuration(int Baudrate)
{
	GPIO_InitTypeDef    GPIO_InitStructure;     
	USART_InitTypeDef   USART_InitStructure; 
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //USART1ʱ��ʹ�ܣ�GPIOAʱ��ʹ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;   //USART1 TX
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    //����Ƶ��50MHZ
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		//�����������
  GPIO_Init(GPIOB,&GPIO_InitStructure);                //GPIOA

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;  //USART1 RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    //���ÿ�©����
	GPIO_Init(GPIOB,&GPIO_InitStructure);                //GPIOA   
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//��ռ���ȼ�3  
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;      //�����ȼ�3  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQͨ��ʹ��
  NVIC_Init(&NVIC_InitStructure); //���ݲ�����ʼ��NVIC
		
  USART_InitStructure.USART_BaudRate = Baudrate;                //�����趨
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;   //����λ8λ
  USART_InitStructure.USART_StopBits = USART_StopBits_1;        //ֹͣλ1λ
  USART_InitStructure.USART_Parity = USART_Parity_No;           //��У��λ
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   // ��Ӳ������
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;   //�շ�ģʽ
  USART_Init(USART3, &USART_InitStructure);                         //���ô���
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);                    //ʹ�ܽ����ж�
  USART_Cmd(USART3,ENABLE);                                         //����USART3    
	printf("USART3 Init...OK\r\n");
}
