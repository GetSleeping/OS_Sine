/******************************************************************************************
**	  ���������壨V1.0��
**	  systick�����ļ�
**
**	  ��    ̳��bbs.openmcu.com
**	  ��    ����www.openmcu.com
**	  ��    �䣺support@openmcu.com
**
**    ��    ����V1.0
**	  ��    �ߣ�FXL
**	  �������:	2012.8.9
*******************************************************************************************/
#include "stm32f10x.h"
#include "Systick.h"
#include "ucos_ii.h"

static vu32 TimingDelay;

/********************************************************************************************
*�������ƣ�void Delay(u32 nTime)
*
*��ڲ�����u32 nTime:��ʱʱ��
*
*���ڲ�������
*
*����˵����SYSTICK�δ���ʱ����
*******************************************************************************************/
//void Delay(u32 nTime)
//{
//  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);	//ʱ��Դѡ��
//  SysTick_Config(720000);								//���ò����жϵļ�������
//
//  TimingDelay = nTime;
//  while(TimingDelay != 0);
//}
/********************************************************************************************
*�������ƣ�void Systick_IRQ(void)
*
*��ڲ�������
*
*���ڲ�������
*
*����˵����SYSTICK�δ��жϴ�����
*******************************************************************************************/
//void Systick_IRQ(void)
//{
//  if (TimingDelay != 0x00)
//  { 
//    TimingDelay--;
//  }
//}

void systick_init(void)
{
    RCC_ClocksTypeDef rcc_clocks;
    RCC_GetClocksFreq(&rcc_clocks);
    SysTick_Config(rcc_clocks.HCLK_Frequency/OS_TICKS_PER_SEC);
}
