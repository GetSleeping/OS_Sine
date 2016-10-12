#ifndef _TOUCH_H_
#define _TOUCH_H_

#include "stm32f10x.h"

#define   uchar        unsigned char

/*****TOUCH_SPI�ӿ�����*****/
#define TOUCH_SPI_RCC            RCC_APB1Periph_SPI2
#define TOUCH_SPI                SPI2
#define SPI_RCC_APB2Periph       RCC_APB2Periph_GPIOB
#define SPI_SCK_GPIO_Pin         GPIO_Pin_13
#define SPI_MISO_GPIO_Pin        GPIO_Pin_14
#define SPI_MOSI_GPIO_Pin        GPIO_Pin_15
#define SPI_GPIO                 GPIOB

/*****TOUCH_CS�ӿ�����*****/
#define TOUCH_CS_APB2Periph      RCC_APB2Periph_GPIOB
#define TOUCH_CS_GPIO            GPIOB
#define TOUCH_CS_GPIO_Pin        GPIO_Pin_12

/*****TOUCH_IRQ�ӿ�����*****/
#define TOUCH_IRQ_APB2Periph     RCC_APB2Periph_GPIOG
#define TOUCH_IRQ_GPIO           GPIOG
#define TOUCH_IRQ_GPIO_Pin       GPIO_Pin_7
#define TOUCH_IRQ_CONFIG         GPIO_PinSource7
#define TOUCH_IRQ_LINE		     EXTI_Line7

/*****FLASH_CS�ӿ�����*****/
#define FLASH_CS_APB2Periph      RCC_APB2Periph_GPIOG
#define FLASH_CS_GPIO            GPIOG
#define FLASH_CS_GPIO_Pin        GPIO_Pin_11

/*****SD_CS�ӿ�����*****/
#define SD_CS_APB2Periph         RCC_APB2Periph_GPIOG
#define SD_CS_GPIO               GPIOG
#define SD_CS_GPIO_Pin           GPIO_Pin_15

#define TP_IO_STATE				 GPIO_ReadInputDataBit(TOUCH_IRQ_GPIO,TOUCH_IRQ_GPIO_Pin)

#define Select_TOUCH_CS()        GPIO_ResetBits(TOUCH_CS_GPIO,TOUCH_CS_GPIO_Pin) //TOUCH ѡ��
#define NotSelect_TOUCH_CS()     GPIO_SetBits(TOUCH_CS_GPIO,TOUCH_CS_GPIO_Pin)   //TOUCH ��ֹ

#define Select_FLASH_CS()        GPIO_ResetBits(FLASH_CS_GPIO,FLASH_CS_GPIO_Pin) //FLASH ѡ��
#define NotSelect_FLASH_CS()     GPIO_SetBits(FLASH_CS_GPIO,FLASH_CS_GPIO_Pin)   //FLASH ��ֹ

#define Select_SD_CS()           GPIO_ResetBits(SD_CS_GPIO,SD_CS_GPIO_Pin)        //SD ѡ��
#define NotSelect_SD_CS()        GPIO_SetBits(SD_CS_GPIO,SD_CS_GPIO_Pin)          //SD ��ֹ


/* ADS7843/7846/UH7843/7846/XPT2046/TSC2046 ָ� */
#define CMD_RDY 0X90  //0B10010000���ò�ַ�ʽ��X����
#define CMD_RDX	0XD0  //0B11010000���ò�ַ�ʽ��Y���� 

/* ����״̬	*/ 
#define Key_Down 0x01
#define Key_Up   0x00 

/* �ʸ˽ṹ�� */
typedef struct 
{
	uint16_t X0;//ԭʼ����
	uint16_t Y0;
	uint16_t X; //����/�ݴ�����
	uint16_t Y;						   	    
	uint8_t  Key_Sta;//�ʵ�״̬			  
	//������У׼����
	float xfac;
	float yfac;
	short xoff;
	short yoff;
}Pen_Holder;

uint16_t ADS_Read_XY(uint8_t xy);
void Touch_Init(void);
uint8_t Read_ADS(uint16_t *x,uint16_t *y);
void Touch_IRQ(void);
void Convert_Pos(void);
void Refreshes_Screen(void);
void Draw_Big_Point(uint8_t x,uint16_t y);
void Pen_Int_Set(uint8_t en);
#endif

