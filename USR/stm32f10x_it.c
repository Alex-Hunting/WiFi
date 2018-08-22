/**
  ******************************************************************************
  * @file GPIO/IOToggle/stm32f10x_it.c 
  * @author  MCD Application Team
  * @version  V3.0.0
  * @date  04/06/2009
  * @brief  Main Interrupt Service Routines.
  *         This file provides template for all exceptions handler and 
  *         peripherals interrupt service routine.
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x_it.h"	 
#include "stm32f10x_exti.h"
#include "stm32f10x_rcc.h"
#include "misc.h"
#include "pbdata.h"
#include "string.h"

void NMI_Handler(void)
{
	
}

void USART1_IRQHandler(void)
{
   if(USART_GetITStatus(USART1,USART_IT_RXNE)!=RESET)
   {
		 USART_ClearITPendingBit(USART1, USART_IT_RXNE);
		 USART_SendData(USART1,USART_ReceiveData(USART1));
		 while(USART_GetFlagStatus(USART1,USART_FLAG_TXE)==RESET);
   }
}

extern u8 BT_Data[30];//USART2���ܻ���
extern u8 USART2_RX_BUF[30];     //USART2���ܻ���,����һ����෢20���ֽ�
extern u8 USART2_RX_Flag;        //USART2���ݽ��ܱ�־
u8 USART2_RX = 0;    //USARY3��������λ
void USART2_IRQHandler(void)
{
	USART2_RX_Flag = 0;//���ձ�־��0
  if(USART_GetITStatus(USART2,USART_IT_RXNE) != RESET)//�жϲ���
		{
			USART_ClearITPendingBit(USART2,USART_IT_RXNE); //����жϱ�־
			USART2_RX_BUF[USART2_RX] = USART_ReceiveData(USART2);//��������
			USART2_RX++; //����λ�Լ�
			if(USART2_RX_BUF[USART2_RX-1] == '#' || USART2_RX == 20)   //��⵽����(���ܽ���)���ߴﵽ��󻺴�
				{
					memcpy(BT_Data, USART2_RX_BUF, sizeof(USART2_RX_BUF));  //��USART2_RX_BUF���Ƶ�BT_Data
					USART2_RX_Flag = 1;   //���ձ�־��1������ATָ��ؼ��
					USART2_RX = 0;        //����λ��0
				}
		}
}

u8 USART3_RX_BUF[128];     //USART3���ܻ���
u8 USART3_RX_Flag;         //USART3���ݽ��ܱ�־
u8 USART3_MAX_BUF;          //USART�����ܻ���
u8 USART3_RX = 0;    //USARY3��������λ
void USART3_IRQHandler(void)
{
	USART3_RX_Flag = 0;//���ձ�־��0
  if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET)//�жϲ���
		{
			USART_ClearITPendingBit(USART3,USART_IT_RXNE); //����жϱ�־
			USART3_RX_BUF[USART3_RX] = USART_ReceiveData(USART3);//��������
			USART_SendData(USART1, USART3_RX_BUF[USART3_RX]);
			USART3_RX++; //����λ�Լ�
			if((USART3_RX_BUF[USART3_RX-2] == 0x0d && USART3_RX_BUF[USART3_RX-1] == 0x0a) || USART3_RX == USART3_MAX_BUF)   //��⵽����(���ܽ���)���ߴﵽ��󻺴�
				{
					USART3_RX_Flag = 1;   //���ձ�־��1������ATָ��ؼ��
					USART3_RX = 0;        //����λ��0
				}
		}
}
/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval : None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval : None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval : None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval : None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval : None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval : None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval : None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval : None
  */
void SysTick_Handler(void)
{
}

void TIM3_IRQHandler(void)
{
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //TIM3�����ж�
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);//����жϱ�־λ
        if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13) != RESET)//���GPIOC_Pin13����ߵ�ƽ
					GPIO_ResetBits(GPIOC, GPIO_Pin_13);//����GPIOC_Pin13
				else
					GPIO_SetBits(GPIOC, GPIO_Pin_13);//����GPIOC_Pin13
    }
}
/****************************************************************************
* ��    �ƣ�void EXTI9_5_IRQHandler(void)
* ��    �ܣ�EXTI9-5�жϴ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void EXTI9_5_IRQHandler(void)
{
}

/****************************************************************************
* ��    �ƣ�void EXTI1_IRQHandler(void)
* ��    �ܣ�EXTI2�жϴ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void EXTI1_IRQHandler(void)
{
    
}

/****************************************************************************
* ��    �ƣ�void EXTI2_IRQHandler(void)
* ��    �ܣ�EXTI2�жϴ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void EXTI2_IRQHandler(void)
{
    
}

/****************************************************************************
* ��    �ƣ�void EXTI3_IRQHandler(void)
* ��    �ܣ�EXTI3�жϴ������
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷������� 
****************************************************************************/
void EXTI3_IRQHandler(void)
{
   
}
