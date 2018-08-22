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

extern u8 BT_Data[30];//USART2接受缓存
extern u8 USART2_RX_BUF[30];     //USART2接受缓存,蓝牙一次最多发20个字节
extern u8 USART2_RX_Flag;        //USART2数据接受标志
u8 USART2_RX = 0;    //USARY3接收数据位
void USART2_IRQHandler(void)
{
	USART2_RX_Flag = 0;//接收标志置0
  if(USART_GetITStatus(USART2,USART_IT_RXNE) != RESET)//中断产生
		{
			USART_ClearITPendingBit(USART2,USART_IT_RXNE); //清楚中断标志
			USART2_RX_BUF[USART2_RX] = USART_ReceiveData(USART2);//接收数据
			USART2_RX++; //数据位自加
			if(USART2_RX_BUF[USART2_RX-1] == '#' || USART2_RX == 20)   //监测到换行(接受结束)或者达到最大缓存
				{
					memcpy(BT_Data, USART2_RX_BUF, sizeof(USART2_RX_BUF));  //将USART2_RX_BUF复制到BT_Data
					USART2_RX_Flag = 1;   //接收标志置1，用于AT指令返回检查
					USART2_RX = 0;        //数据位置0
				}
		}
}

u8 USART3_RX_BUF[128];     //USART3接受缓存
u8 USART3_RX_Flag;         //USART3数据接受标志
u8 USART3_MAX_BUF;          //USART最大接受缓存
u8 USART3_RX = 0;    //USARY3接收数据位
void USART3_IRQHandler(void)
{
	USART3_RX_Flag = 0;//接收标志置0
  if(USART_GetITStatus(USART3,USART_IT_RXNE) != RESET)//中断产生
		{
			USART_ClearITPendingBit(USART3,USART_IT_RXNE); //清楚中断标志
			USART3_RX_BUF[USART3_RX] = USART_ReceiveData(USART3);//接收数据
			USART_SendData(USART1, USART3_RX_BUF[USART3_RX]);
			USART3_RX++; //数据位自加
			if((USART3_RX_BUF[USART3_RX-2] == 0x0d && USART3_RX_BUF[USART3_RX-1] == 0x0a) || USART3_RX == USART3_MAX_BUF)   //监测到换行(接受结束)或者达到最大缓存
				{
					USART3_RX_Flag = 1;   //接收标志置1，用于AT指令返回检查
					USART3_RX = 0;        //数据位置0
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
	if (TIM_GetITStatus(TIM3, TIM_IT_Update) != RESET) //TIM3产生中断
    {
        TIM_ClearITPendingBit(TIM3, TIM_IT_Update);//清除中断标志位
        if(GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13) != RESET)//如果GPIOC_Pin13输出高电平
					GPIO_ResetBits(GPIOC, GPIO_Pin_13);//拉低GPIOC_Pin13
				else
					GPIO_SetBits(GPIOC, GPIO_Pin_13);//拉高GPIOC_Pin13
    }
}
/****************************************************************************
* 名    称：void EXTI9_5_IRQHandler(void)
* 功    能：EXTI9-5中断处理程序
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void EXTI9_5_IRQHandler(void)
{
}

/****************************************************************************
* 名    称：void EXTI1_IRQHandler(void)
* 功    能：EXTI2中断处理程序
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void EXTI1_IRQHandler(void)
{
    
}

/****************************************************************************
* 名    称：void EXTI2_IRQHandler(void)
* 功    能：EXTI2中断处理程序
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void EXTI2_IRQHandler(void)
{
    
}

/****************************************************************************
* 名    称：void EXTI3_IRQHandler(void)
* 功    能：EXTI3中断处理程序
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：无 
****************************************************************************/
void EXTI3_IRQHandler(void)
{
   
}
