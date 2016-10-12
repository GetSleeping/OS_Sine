#ifndef _GPIO_LED_H_
#define _GPIO_LED_H_ 

#include "stm32f10x.h"

/*****led1接口声明*****/
#define LED1_RCC_APB2Periph  RCC_APB2Periph_GPIOF
#define LED1_GPIO            GPIOF
#define LED1_GPIO_Pin        GPIO_Pin_6
#define LED1_ON()            GPIO_ResetBits(LED1_GPIO,LED1_GPIO_Pin)
#define LED1_OFF()           GPIO_SetBits(LED1_GPIO,LED1_GPIO_Pin)

/*****led2接口声明*****/
#define LED2_RCC_APB2Periph  RCC_APB2Periph_GPIOF
#define LED2_GPIO            GPIOF
#define LED2_GPIO_Pin        GPIO_Pin_7
#define LED2_ON()            GPIO_ResetBits(LED2_GPIO,LED2_GPIO_Pin)
#define LED2_OFF()           GPIO_SetBits(LED2_GPIO,LED2_GPIO_Pin)

/*****led3接口声明*****/
#define LED3_RCC_APB2Periph  RCC_APB2Periph_GPIOF
#define LED3_GPIO            GPIOF
#define LED3_GPIO_Pin        GPIO_Pin_8
#define LED3_ON()            GPIO_ResetBits(LED3_GPIO,LED3_GPIO_Pin)
#define LED3_OFF()           GPIO_SetBits(LED3_GPIO,LED3_GPIO_Pin)

/*****按键USER1接口声明*****/
#define KEY1_RCC_APB2Periph  RCC_APB2Periph_GPIOA
#define KEY1_GPIO            GPIOA
#define KEY1_GPIO_Pin        GPIO_Pin_8

/*****按键USER2接口声明*****/
#define KEY2_RCC_APB2Periph  RCC_APB2Periph_GPIOD
#define KEY2_GPIO            GPIOD
#define KEY2_GPIO_Pin        GPIO_Pin_3

/*****按键WAKEUP接口声明*****/
#define KEY3_RCC_APB2Periph  RCC_APB2Periph_GPIOA
#define KEY3_GPIO            GPIOA
#define KEY3_GPIO_Pin        GPIO_Pin_0

typedef enum
{
    false=0,
	true
}Bool;

typedef struct
{
    Bool Key1_state;	//按键1的状态标记
	Bool Key2_state;	//按键2的状态标记
	Bool Key3_state;	//按键3的状态标记
	u32  Counter1;      //临时计数
	u32  Counter2;      //临时计数
}Key_Info;

void GpioLed_Init(void);
void LED_Display(void);
void Key_Init(void);
void Key_Test(void);


#endif
