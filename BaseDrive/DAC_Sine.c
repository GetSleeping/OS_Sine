#include "DAC_Sine.h"
#include "stm32f10x.h"
#define DAC_DHR12RD_Address      0x40007420 //0x40007420




/* 波形数据 ---------------------------------------------------------*/
const uint16_t Sine12bit[32] = {
	2448,2832,3186,3496,3751,3940,4057,4095,4057,3940,
	3751,3496,3186,2832,2448,2048,1648,1264,910,600,345,
	156,39,0,39,156,345,600,910,1264,1648,2048
};


uint32_t DualSine12bit[32];


const uint16_t MySinebit[100] = {
	1552,1649,1747,1843,1938,2032,2123,2213,2300,2384,
	2464,2541,2614,2683,2748,2808,2862,2912,2956,2995,
	3028,3055,3077,3092,3101,3104,3101,3092,3077,3055,
	3028,2995,2956,2912,2862,2808,2748,2683,2614,2541,
	2464,2384,2300,2213,2123,2032,1938,1843,1747,1649,
	1552,1455,1357,1261,1166,1072,981,891,804,720,640,
	563,490,421,356,296,242,192,148,109,76,49,27,12,3,
	0,3,12,27,49,76,109,148,192,242,296,356,421,490,
	563,640,720,804,891,981,1072,1166,1261,1357,1455
};

uint32_t MyDualSine12bit[100];
/**
  * @brief  使能DAC的时钟，初始化GPIO
  * @param  无
  * @retval 无
  */
static void DAC_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	DAC_InitTypeDef  DAC_InitStructure;

  /* 使能GPIOA时钟 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	
	/* 使能DAC时钟 */	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	
  /* DAC的GPIO配置，模拟输入 */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	


  /* 配置DAC 通道1 */
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T2_TRGO;						//使用TIM2作为触发源
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;	//不使用波形发生器
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;	//不使用DAC输出缓冲
  DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = 0x0;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);

  /* 配置DAC 通道2 */
  DAC_Init(DAC_Channel_2, &DAC_InitStructure);

  /* 使能通道1 由PA4输出 */
  DAC_Cmd(DAC_Channel_1, ENABLE);
  /* 使能通道2 由PA5输出 */
  DAC_Cmd(DAC_Channel_2, ENABLE);

  /* 使能DAC的DMA请求 */
  DAC_DMACmd(DAC_Channel_2, ENABLE);
}


/**
  * @brief  配置TIM
  * @param  无
  * @retval 无
  */
static void DAC_TIM_Config(void)
{
	
	TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
	
	/* 使能TIM2时钟，TIM2CLK 为72M */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
  /* TIM2基本定时器配置 */
 // TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
  TIM_TimeBaseStructure.TIM_Period = 240 - 1;       			//定时周期:1K = 720 - 1(不分频)  3K = 240 - 1 
  TIM_TimeBaseStructure.TIM_Prescaler = 0;       				//预分频，不分频 72M / (0+1) = 72M
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    			//时钟分频系数
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//向上计数模式
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* 配置TIM2触发源 */
  TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);

	/* 使能TIM2 */
  TIM_Cmd(TIM2, ENABLE);

}

/**
  * @brief  配置DMA
  * @param  无
  * @retval 无
  */
static void DAC_DMA_Config(void)
{	
	DMA_InitTypeDef  DMA_InitStructure;

	/* 使能DMA2时钟 */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	
	/* 配置DMA2 */
  DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12RD_Address;			//外设数据地址
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&MyDualSine12bit ;			//内存数据地址 DualSine12bit
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;						//数据传输方向内存至外设
  DMA_InitStructure.DMA_BufferSize = 100;									//缓存大小为32字节	
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			//外设数据地址固定	
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//内存数据地址自增
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;	//外设数据以字为单位
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;			//内存数据以字为单位	
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;							//循环模式
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;						//高DMA通道优先级
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;								//非内存至内存模式	

  DMA_Init(DMA2_Channel4, &DMA_InitStructure);
	
  /* 使能DMA2-14通道 */
  DMA_Cmd(DMA2_Channel4, ENABLE);
}


/**
  * @brief  DAC初始化函数
  * @param  无
  * @retval 无
  */
void DAC_Mode_Init(void)
{
	uint32_t Idx = 0;  

	DAC_Config();
	DAC_TIM_Config();
	DAC_DMA_Config();
	
	/* 填充正弦波形数据，双通道右对齐*/
  for (Idx = 0; Idx < 100; Idx++)
  {
    MyDualSine12bit[Idx] = (MySinebit[Idx] << 16) + (MySinebit[Idx]);
  }
}




	




