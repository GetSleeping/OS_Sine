/******************************************************************************************
**	  红龙开发板（V1.0）
**	  systick配置文件
**
**	  论    坛：bbs.openmcu.com
**	  旺    宝：www.openmcu.com
**	  邮    箱：support@openmcu.com
**
**    版    本：V1.0
**	  作    者：FXL
**	  完成日期:	2012.8.9
*******************************************************************************************/
#include "stm32f10x.h"
#include "Systick.h"
#include "ucos_ii.h"

static vu32 TimingDelay;

/********************************************************************************************
*函数名称：void Delay(u32 nTime)
*
*入口参数：u32 nTime:延时时间
*
*出口参数：无
*
*功能说明：SYSTICK滴答延时配置
*******************************************************************************************/
//void Delay(u32 nTime)
//{
//  SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK);	//时钟源选择
//  SysTick_Config(720000);								//调置产生中断的计数个数
//
//  TimingDelay = nTime;
//  while(TimingDelay != 0);
//}
/********************************************************************************************
*函数名称：void Systick_IRQ(void)
*
*入口参数：无
*
*出口参数：无
*
*功能说明：SYSTICK滴答中断处理函数
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
