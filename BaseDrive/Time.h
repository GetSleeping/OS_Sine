#ifndef _TIM1TEST_H_
#define _TIM1TEST_H_

/*TIM使用声明*/
//#define TIM             TIM1					  //TIM声明
//#define RCC_APB_TIM     RCC_APB2Periph_TIM1	  //TIM时钟使能
#define TIM             TIM2					  //TIM声明
#define RCC_APB_TIM     RCC_APB1Periph_TIM2	      //TIM时钟使能

#define TIM_COUNTER     20					      //计数个数 10ms							
#define TIM_PSC         36000                     //分频系数

void TimCounterInit(void);
void Tim_IRQ(void);

#endif


