/******************************************************************
**	  ���������壨V1.0��
**	  TIM�����ļ�
**
**	  ��    ̳��bbs.openmcu.com
**	  ��    ����www.openmcu.com
**	  ��    �䣺support@openmcu.com
**
**    ��    ����V1.0
**	  ��    �ߣ�FXL
**	  �������:	2012.7.25
********************************************************************/
#include "stm32f10x.h"
#include <stdio.h>
#include "Time.h"
#include "gui.h"
#include "Nvic.h"

volatile u32 capture;

/********************************************************************************************
*�������ƣ�void TimCounterInit(void)
*
*��ڲ�������
*
*���ڲ�������
*
*����˵����TIM������ʼ������
*	       TIMCLK = 72 MHz, Prescaler = TIM_PSC, TIM counter clock = TIMCLK/TIM_COUNTER MHz
*******************************************************************************************/
void TimCounterInit(void)
{
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;

	//Gpio_Init();               //IO��ʼ��
	NVIC_Configuration();        //�򿪶�ʱ��ʸ��

	/* TIMʱ��ʹ�� */
    if ((TIM == TIM1) || (TIM == TIM8))
	    RCC_APB2PeriphClockCmd(RCC_APB_TIM, ENABLE);
    else if((TIM == TIM2) || (TIM == TIM3) || (TIM == TIM4) || (TIM == TIM5))
	    RCC_APB1PeriphClockCmd(RCC_APB_TIM, ENABLE);

	/* ����ʱ����ʼ�� */
	TIM_TimeBaseStructure.TIM_Period = TIM_COUNTER;				//����ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = TIM_PSC;				//��Ƶϵ��		 
	TIM_TimeBaseStructure.TIM_ClockDivision = 0;				//ʱ�ӷָ�Ĵ������������ֲ����ʱ��δ������
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;	//����ģʽ
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;			//�ظ�����ֵ
	
	TIM_TimeBaseInit(TIM, &TIM_TimeBaseStructure);				//��ʼ��TIM
	
	TIM_ITConfig(TIM,TIM_IT_Update,ENABLE);						//�� �����¼� �ж�
	TIM_Cmd(TIM, ENABLE);										//ʹ��TIM
}

/********************************************************************************************
*�������ƣ�void Tim_IRQ(void)
*
*��ڲ�������
*
*���ڲ�������
*
*����˵����TIM�жϴ�����
*******************************************************************************************/
void Tim_IRQ(void)
{
    if(TIM_GetITStatus(TIM, TIM_IT_Update) != RESET)					//�ж��Ƿ�Ϊ �����¼� ��־λ
	{
	    TIM_ClearITPendingBit(TIM, TIM_IT_Update);						//��� 	�����¼� ��־
		
		GUI_TOUCH_Exec();	 //��ⴥ��
	
	}
}
