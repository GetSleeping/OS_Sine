#include "DAC_Sine.h"
#include "stm32f10x.h"
#define DAC_DHR12RD_Address      0x40007420 //0x40007420




/* �������� ---------------------------------------------------------*/
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
  * @brief  ʹ��DAC��ʱ�ӣ���ʼ��GPIO
  * @param  ��
  * @retval ��
  */
static void DAC_Config(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	DAC_InitTypeDef  DAC_InitStructure;

  /* ʹ��GPIOAʱ�� */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);	
	
	/* ʹ��DACʱ�� */	
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	
  /* DAC��GPIO���ã�ģ������ */
  GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_4 | GPIO_Pin_5;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	


  /* ����DAC ͨ��1 */
  DAC_InitStructure.DAC_Trigger = DAC_Trigger_T2_TRGO;						//ʹ��TIM2��Ϊ����Դ
  DAC_InitStructure.DAC_WaveGeneration = DAC_WaveGeneration_None;	//��ʹ�ò��η�����
  DAC_InitStructure.DAC_OutputBuffer = DAC_OutputBuffer_Disable;	//��ʹ��DAC�������
  DAC_InitStructure.DAC_LFSRUnmask_TriangleAmplitude = 0x0;
  DAC_Init(DAC_Channel_1, &DAC_InitStructure);

  /* ����DAC ͨ��2 */
  DAC_Init(DAC_Channel_2, &DAC_InitStructure);

  /* ʹ��ͨ��1 ��PA4��� */
  DAC_Cmd(DAC_Channel_1, ENABLE);
  /* ʹ��ͨ��2 ��PA5��� */
  DAC_Cmd(DAC_Channel_2, ENABLE);

  /* ʹ��DAC��DMA���� */
  DAC_DMACmd(DAC_Channel_2, ENABLE);
}


/**
  * @brief  ����TIM
  * @param  ��
  * @retval ��
  */
static void DAC_TIM_Config(void)
{
	
	TIM_TimeBaseInitTypeDef    TIM_TimeBaseStructure;
	
	/* ʹ��TIM2ʱ�ӣ�TIM2CLK Ϊ72M */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
  /* TIM2������ʱ������ */
 // TIM_TimeBaseStructInit(&TIM_TimeBaseStructure); 
  TIM_TimeBaseStructure.TIM_Period = 240 - 1;       			//��ʱ����:1K = 720 - 1(����Ƶ)  3K = 240 - 1 
  TIM_TimeBaseStructure.TIM_Prescaler = 0;       				//Ԥ��Ƶ������Ƶ 72M / (0+1) = 72M
  TIM_TimeBaseStructure.TIM_ClockDivision = 0x0;    			//ʱ�ӷ�Ƶϵ��
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;  	//���ϼ���ģʽ
  TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);

  /* ����TIM2����Դ */
  TIM_SelectOutputTrigger(TIM2, TIM_TRGOSource_Update);

	/* ʹ��TIM2 */
  TIM_Cmd(TIM2, ENABLE);

}

/**
  * @brief  ����DMA
  * @param  ��
  * @retval ��
  */
static void DAC_DMA_Config(void)
{	
	DMA_InitTypeDef  DMA_InitStructure;

	/* ʹ��DMA2ʱ�� */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA2, ENABLE);
	
	/* ����DMA2 */
  DMA_InitStructure.DMA_PeripheralBaseAddr = DAC_DHR12RD_Address;			//�������ݵ�ַ
  DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&MyDualSine12bit ;			//�ڴ����ݵ�ַ DualSine12bit
  DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;						//���ݴ��䷽���ڴ�������
  DMA_InitStructure.DMA_BufferSize = 100;									//�����СΪ32�ֽ�	
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;			//�������ݵ�ַ�̶�	
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;					//�ڴ����ݵ�ַ����
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;	//������������Ϊ��λ
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;			//�ڴ���������Ϊ��λ	
  DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;							//ѭ��ģʽ
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;						//��DMAͨ�����ȼ�
  DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;								//���ڴ����ڴ�ģʽ	

  DMA_Init(DMA2_Channel4, &DMA_InitStructure);
	
  /* ʹ��DMA2-14ͨ�� */
  DMA_Cmd(DMA2_Channel4, ENABLE);
}


/**
  * @brief  DAC��ʼ������
  * @param  ��
  * @retval ��
  */
void DAC_Mode_Init(void)
{
	uint32_t Idx = 0;  

	DAC_Config();
	DAC_TIM_Config();
	DAC_DMA_Config();
	
	/* ������Ҳ������ݣ�˫ͨ���Ҷ���*/
  for (Idx = 0; Idx < 100; Idx++)
  {
    MyDualSine12bit[Idx] = (MySinebit[Idx] << 16) + (MySinebit[Idx]);
  }
}




	



