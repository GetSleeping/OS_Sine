#ifndef _TIM1TEST_H_
#define _TIM1TEST_H_

/*TIMʹ������*/
//#define TIM             TIM1					  //TIM����
//#define RCC_APB_TIM     RCC_APB2Periph_TIM1	  //TIMʱ��ʹ��
#define TIM             TIM2					  //TIM����
#define RCC_APB_TIM     RCC_APB1Periph_TIM2	      //TIMʱ��ʹ��

#define TIM_COUNTER     20					      //�������� 10ms							
#define TIM_PSC         36000                     //��Ƶϵ��

void TimCounterInit(void);
void Tim_IRQ(void);

#endif


