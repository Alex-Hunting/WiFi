#include "stm32f10x_usart.h"
#include "misc.h"
#include "stdio.h"
#include "USART.h"

/*****************************************
*函数名：    fputc
*函数功能：  重定向c库函数printf到USART1
*函数参数：  无
*函数返回值：无
*调用：      由printf调用
******************************************/
int fputc(int ch,FILE *f)
{
	USART_SendData(USART1,(u8)ch);
	while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
	return ch;
}

/*****************************************
*函数名：    UART1_Configuration
*函数功能：  初始化USART1
*函数参数：  初始化波特率的值
*函数返回值：无
******************************************/
void USART1_Configuration(int Baudrate)
{
	GPIO_InitTypeDef    GPIO_InitStructure;     
	USART_InitTypeDef   USART_InitStructure; 
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1 | RCC_APB2Periph_GPIOA, ENABLE);  //USART1时钟使能，GPIOA时钟使能
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;   //USART1 TX
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    //设置频率50MHZ
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		//复用推挽输出
  GPIO_Init(GPIOA,&GPIO_InitStructure);                //GPIOA

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;  //USART1 RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    //复用开漏输入
	GPIO_Init(GPIOA,&GPIO_InitStructure);                //GPIOA   
	
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3  
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;      //子优先级3  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQ通道使能
  NVIC_Init(&NVIC_InitStructure); //根据参数初始化NVIC
		
  USART_InitStructure.USART_BaudRate = Baudrate;                //速率设定
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;   //数据位8位
  USART_InitStructure.USART_StopBits = USART_StopBits_1;        //停止位1位
  USART_InitStructure.USART_Parity = USART_Parity_No;           //无校验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   // 无硬件流控
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;   //收发模式
  USART_Init(USART1, &USART_InitStructure);                         //配置串口
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                    //使能接收中断
  USART_Cmd(USART1,ENABLE);                                         //启动USART1         
	printf("USART1 Init...OK\r\n");
}

/*****************************************
*函数名：    Sendware
*函数功能：  波形发送函数，可使用山外多功能调试助使构成虚拟示波器
*函数参数：  待发送数组，数组字节大小；推荐使用格式      Sendware((uint8_t*)addr, sizeof(addr));
*函数返回值：无
******************************************/
void Sendware(uint8_t *wareaddr, uint32_t waresize)
{
	USART1->DR = 0X03;       //发送帧头1  0X03
	while((USART1->SR&0X40) == 0);
	USART1->DR = 0XFC;       //发送帧头2  0XFC
	while((USART1->SR&0X40) == 0);

	/*发送波形数据*/
	while(waresize--)      
	{
		USART1->DR = *wareaddr;
		while((USART1->SR&0X40) == 0);
		wareaddr++;
	}
	
	USART1->DR = 0XFC; //发送结尾标识1
	while((USART1->SR&0X40) == 0);
	USART1->DR = 0X03; //发送结尾标识2
	while((USART1->SR&0X40) == 0);
}

/*****************************************
*函数名：    UART2_Configuration
*函数功能：  初始化USART2
*函数参数：  初始化波特率的值
*函数返回值：无
******************************************/
void USART2_Configuration(int Baudrate)
{
	GPIO_InitTypeDef    GPIO_InitStructure;     
	USART_InitTypeDef   USART_InitStructure; 
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);  //USART1时钟使能，GPIOA时钟使能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);	//??USART2??
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;   //USART1 TX
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    //设置频率50MHZ
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		//复用推挽输出
  GPIO_Init(GPIOA,&GPIO_InitStructure);                //GPIOA

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;  //USART1 RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    //复用开漏输入
	GPIO_Init(GPIOA,&GPIO_InitStructure);                //GPIOA   
	
	//NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0  
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;      //子优先级0 
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQ通道使能
  NVIC_Init(&NVIC_InitStructure); //根据参数初始化NVIC
		
  USART_InitStructure.USART_BaudRate = Baudrate;                //速率设定
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;   //数据位8位
  USART_InitStructure.USART_StopBits = USART_StopBits_1;        //停止位1位
  USART_InitStructure.USART_Parity = USART_Parity_No;           //无校验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   // 无硬件流控
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;   //收发模式
  USART_Init(USART2, &USART_InitStructure);                         //配置串口
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);                    //使能接收中断
  USART_Cmd(USART2,ENABLE);                                         //启动USART2      
	printf("USART2 Init...OK\r\n");
}

/*****************************************
*函数名：    UART2_Configuration
*函数功能：  初始化USART2
*函数参数：  初始化波特率的值
*函数返回值：无
******************************************/
void USART3_Configuration(int Baudrate)
{
	GPIO_InitTypeDef    GPIO_InitStructure;     
	USART_InitTypeDef   USART_InitStructure; 
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);  //USART1时钟使能，GPIOA时钟使能
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);
	
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;   //USART1 TX
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    //设置频率50MHZ
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; 		//复用推挽输出
  GPIO_Init(GPIOB,&GPIO_InitStructure);                //GPIOA

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;  //USART1 RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;    //复用开漏输入
	GPIO_Init(GPIOB,&GPIO_InitStructure);                //GPIOA   
	
	NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;  
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=3 ;//抢占优先级3  
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;      //子优先级3  
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;         //IRQ通道使能
  NVIC_Init(&NVIC_InitStructure); //根据参数初始化NVIC
		
  USART_InitStructure.USART_BaudRate = Baudrate;                //速率设定
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;   //数据位8位
  USART_InitStructure.USART_StopBits = USART_StopBits_1;        //停止位1位
  USART_InitStructure.USART_Parity = USART_Parity_No;           //无校验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;   // 无硬件流控
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;   //收发模式
  USART_Init(USART3, &USART_InitStructure);                         //配置串口
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);                    //使能接收中断
  USART_Cmd(USART3,ENABLE);                                         //启动USART3    
	printf("USART3 Init...OK\r\n");
}
