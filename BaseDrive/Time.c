/******************************************************************
**	  红龙开发板（V1.0）
**	  TIM配置文件
**
**	  论    坛：bbs.openmcu.com
**	  旺    宝：www.openmcu.com
**	  邮    箱：support@openmcu.com
**
**    版    本：V1.0
**	  作    者：FXL
**	  完成日期:	2012.7.25
********************************************************************/
#include "stm32f10x.h"
#include <stdio.h>
#include "Time.h"
#include "gui.h"
#include "Nvic.h"

volatile u32 capture;

/********************************************************************************************
*函数名称：void TimCounterInit(void)
*
*入口参数：无
*
*出口参数：无
*
*功能说明：TIM计数初始化配置
*	       TIMCLK = 72 MHz, Prescaler = TIM_PSC, TIM counter clock = TIMCLK/TIM_COUNTER MHz
*******************************************************************************************/
void TimCounterInit(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	//Gpio_Init();               //IO初始化
	NVIC_Configuration();        //打开定时器矢量

	/* TIM时钟使能 */
    if ((TIM == TIM1) || (TIM == TIM8))
	    RCC_APB2PeriphClockCmd(RCC_APB_TIM, ENABLE);
    else if((TIM == TIM2) || (TIM == TIM3) || (TIM == TIM4) || (TIM == TIM5))
	    RCC_APB1PeriphClockCmd(RCC_APB_TIM, ENABLE);

	/* 基定时器初始化 */
	TIM_TimeBaseStructure.TIM_Period = TIM_COUNTER;				//计数值
	TIM_TimeBaseStructure.TIM_Prescaler = TIM_PSC;				//分频系数		 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;				//时钟分割：寄存器（技术）手册基定时器未讲到。
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//计数模式
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;			//重复计数值
	
	TIM_TimeBaseInit(TIM, &TIM_TimeBaseStructure);				//初始化TIM
	
	TIM_ITConfig(TIM,TIM_IT_Update,ENABLE);						//打开 更新事件 中断
	TIM_Cmd(TIM, ENABLE);										//使能TIM
}

/********************************************************************************************
*函数名称：void Tim_IRQ(void)
*
*入口参数：无
*
*出口参数：无
*
*功能说明：TIM中断处理函数
*******************************************************************************************/
void Tim_IRQ(void)
{
    if(TIM_GetITStatus(TIM, TIM_IT_Update) != RESET)					//判断是否为 更新事件 标志位
	{
	    TIM_ClearITPendingBit(TIM, TIM_IT_Update);						//清除 	更新事件 标志
		
		GUI_TOUCH_Exec();	 //检测触摸
	
	}
}
