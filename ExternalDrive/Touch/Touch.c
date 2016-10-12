#include "stm32f10x.h"
#include "Touch.h"
#include "Nvic.h"
#include "ssd1963_lcd.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
//#include "Systick.h"

Pen_Holder Pen_Point;	/* �����ʵ�� */

static void Delay(unsigned int counter)
{
    unsigned int i;
    for(;counter == 0;counter--)
        for(i = 0;i<1000;i++);
}

//����IO�ڳ�ʼ��
static void Touch_Gpio_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	/* Enable SPI and GPIO clocks */
	if(TOUCH_SPI == SPI1) 
	    RCC_APB2PeriphClockCmd(TOUCH_SPI_RCC , ENABLE);
	else if(TOUCH_SPI == SPI2) 
	    RCC_APB1PeriphClockCmd(TOUCH_SPI_RCC , ENABLE);
	else if(TOUCH_SPI == SPI3)
	{						   
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);
		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);//GPIO_Remap_SWJ_Disable 	
		RCC_APB1PeriphClockCmd(TOUCH_SPI_RCC , ENABLE);
	}

	RCC_APB2PeriphClockCmd( SPI_RCC_APB2Periph  | TOUCH_CS_APB2Periph | 
	                        TOUCH_IRQ_APB2Periph | FLASH_CS_APB2Periph | 
							SD_CS_APB2Periph, ENABLE);
	
	/* Configure SPI pins: SCK, MISO and MOSI */
	GPIO_InitStructure.GPIO_Pin = SPI_SCK_GPIO_Pin | SPI_MOSI_GPIO_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_GPIO, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = SPI_MISO_GPIO_Pin ;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(SPI_GPIO, &GPIO_InitStructure);
	
	/* Configure TOUCH_IRQ select */
	GPIO_InitStructure.GPIO_Pin = TOUCH_IRQ_GPIO_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_Init(TOUCH_IRQ_GPIO, &GPIO_InitStructure);
	
	/* Configure TOUCH_CS select */
	GPIO_InitStructure.GPIO_Pin = TOUCH_CS_GPIO_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(TOUCH_CS_GPIO, &GPIO_InitStructure);
 
	/* Configure FLASH_CS select */
	GPIO_InitStructure.GPIO_Pin = FLASH_CS_GPIO_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(FLASH_CS_GPIO, &GPIO_InitStructure);
	
	/* Configure TOUCH_CS select */
	GPIO_InitStructure.GPIO_Pin = SD_CS_GPIO_Pin;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(SD_CS_GPIO, &GPIO_InitStructure);
	
	NotSelect_TOUCH_CS();
	NotSelect_FLASH_CS();
	NotSelect_SD_CS();
     
}

/*SPIX��ʼ��*/
static void  TouchSPIx_Init(void)
{  
	SPI_InitTypeDef  SPI_InitStructure;
	
	NotSelect_TOUCH_CS();
	
	/* SPI1 configuration */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;//	 SPI_BaudRatePrescaler_32
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(TOUCH_SPI, &SPI_InitStructure);
	
	/* Enable SPI1  */
	SPI_Cmd(TOUCH_SPI, ENABLE);
}
//�����ж�ʵʼ��
static void TOUCH_EXTI_Configuration(void)
{
	EXTI_InitTypeDef EXTI_InitStructure;

	GPIO_EXTILineConfig(GPIO_PortSourceGPIOG, TOUCH_IRQ_CONFIG); 

	EXTI_InitStructure.EXTI_Line = TOUCH_IRQ_LINE; 
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd	= ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	EXTI_GenerateSWInterrupt(TOUCH_IRQ_LINE); // ���� �ܽ�NRF_IRQ_LINE�����ⲿ�ж���·
}

/*SPI��дһ�ֽ�*/
uchar SPI_SendByte(unsigned char byte)
{
	/* Loop while DR register in not emplty */
	while (SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_TXE) == RESET);
	
	/* Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(TOUCH_SPI, byte);
	
	/* Wait to receive a byte */
	while (SPI_I2S_GetFlagStatus(TOUCH_SPI, SPI_I2S_FLAG_RXNE) == RESET);
	
	/* Return the byte read from the SPI bus */
	return SPI_I2S_ReceiveData(TOUCH_SPI);
}

//2�ζ�ȡADS7846,������ȡ2����Ч��ADֵ,�������ε�ƫ��ܳ���
//50,��������,����Ϊ������ȷ,�����������.	   
//�ú����ܴ�����׼ȷ��
#define ERR_RANGE 50 //��Χ 
uint8_t Read_ADS2(uint16_t *x,uint16_t *y) 
{
	u16 x1,y1;
 	u16 x2,y2;
 	u8 flag;    
    flag=Read_ADS(&x1,&y1);   
    if(flag==0)return(0);
    flag=Read_ADS(&x2,&y2);	   
    if(flag==0)return(0);   
    if(((x2<=x1&&x1<x2+ERR_RANGE)||(x1<=x2&&x2<x1+ERR_RANGE))//ǰ�����β�����+-50��
    &&((y2<=y1&&y1<y2+ERR_RANGE)||(y1<=y2&&y2<y1+ERR_RANGE)))
    {
        *x=(x1+x2)/2;
        *y=(y1+y2)/2;
        return 1;
    }else return 0;	  
} 
/*****************************************************************************
** ��������: Pen_Int_Set
** ��������: PEN�ж�����
				EN=1�������ж�
					EN=0: �ر��ж�
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/	 
void Pen_Int_Set(uint8_t en)
{
	if(en)EXTI->IMR|=1<<7;   //����line13�ϵ��ж�	  	
	else EXTI->IMR&=~(1<<7); //�ر�line13�ϵ��ж�	   
}
//��ȡһ������ֵ	
//������ȡһ��,֪��PEN�ɿ��ŷ���!					   
uint8_t Read_TP_Once(void)
{
//	u8 t=0;	    
	Pen_Int_Set(0);//�ر��ж�


	Pen_Point.Key_Sta=Key_Up;

	if(Read_ADS2(&Pen_Point.X,&Pen_Point.Y))
	{
	    Delay(10);
		Pen_Int_Set(1);//�����ж�
	    return 1;
	}
	else
	{ 
		Pen_Int_Set(1);//�����ж�
	    return 0;
	}	
}
//////////////////////////////////////////////////
//��LCD�����йصĺ���  
//��һ��������
//����У׼�õ�
void Drow_Touch_Point(uint8_t x,uint16_t y)
{
	Draw_Circle(x,y,6,Red);//������Ȧ
}
//������У׼����
//�õ��ĸ�У׼����
void Touch_Adjust(void)
{								 
	uint16_t pos_temp[4][2];//���껺��ֵ
	uint8_t  cnt=0;	
	uint16_t d1,d2;
	uint32_t tem1,tem2;
	float fac; 	   
	cnt=0;				 
	LCD_Clear(White);//���� 
	Drow_Touch_Point(20,20);//����1 
	Pen_Point.Key_Sta=Key_Up;//���������ź� 
	Pen_Point.xfac=0;//xfac��������Ƿ�У׼��,����У׼֮ǰ�������!�������	 
	while(1)
	{
		if(Pen_Point.Key_Sta==Key_Down)//����������
		{
			if(Read_TP_Once())//�õ����ΰ���ֵ
			{  								   
				pos_temp[cnt][0]=Pen_Point.X;
				pos_temp[cnt][1]=Pen_Point.Y;
				cnt++;
			}			 
			switch(cnt)
			{			   
				case 1:
					LCD_Clear(White);//���� 
					Drow_Touch_Point(220,20);//����2
					break;
				case 2:
					LCD_Clear(White);//���� 
					Drow_Touch_Point(20,300);//����3
					break;
				case 3:
					LCD_Clear(White);//���� 
					Drow_Touch_Point(220,300);//����4
					break;
				case 4:	 //ȫ���ĸ����Ѿ��õ�
	    		    	//�Ա����
					tem1=abs(pos_temp[0][0]-pos_temp[1][0]);//x1-x2
					tem2=abs(pos_temp[0][1]-pos_temp[1][1]);//y1-y2
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,2�ľ���
					
					tem1=abs(pos_temp[2][0]-pos_temp[3][0]);//x3-x4
					tem2=abs(pos_temp[2][1]-pos_temp[3][1]);//y3-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�3,4�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05||d1==0||d2==0)//���ϸ�
					{
						cnt=0;
						LCD_Clear(White);//���� 
						Drow_Touch_Point(20,20);
						continue;
					}
					tem1=abs(pos_temp[0][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[0][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,3�ľ���
					
					tem1=abs(pos_temp[1][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[1][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�2,4�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//���ϸ�
					{
						cnt=0;
						LCD_Clear(White);//���� 
						Drow_Touch_Point(20,20);
						continue;
					}//��ȷ��
								   
					//�Խ������
					tem1=abs(pos_temp[1][0]-pos_temp[2][0]);//x1-x3
					tem2=abs(pos_temp[1][1]-pos_temp[2][1]);//y1-y3
					tem1*=tem1;
					tem2*=tem2;
					d1=sqrt(tem1+tem2);//�õ�1,4�ľ���
	
					tem1=abs(pos_temp[0][0]-pos_temp[3][0]);//x2-x4
					tem2=abs(pos_temp[0][1]-pos_temp[3][1]);//y2-y4
					tem1*=tem1;
					tem2*=tem2;
					d2=sqrt(tem1+tem2);//�õ�2,3�ľ���
					fac=(float)d1/d2;
					if(fac<0.95||fac>1.05)//���ϸ�
					{
						cnt=0;
						LCD_Clear(White);//���� 
						Drow_Touch_Point(20,20);
						continue;
					}//��ȷ��
					//������
					Pen_Point.xfac=(float)200/(pos_temp[1][0]-pos_temp[0][0]);//�õ�xfac		 
					Pen_Point.xoff=(240-Pen_Point.xfac*(pos_temp[1][0]+pos_temp[0][0]))/2;//�õ�xoff
						  
					Pen_Point.yfac=(float)280/(pos_temp[2][1]-pos_temp[0][1]);//�õ�yfac
					Pen_Point.yoff=(320-Pen_Point.yfac*(pos_temp[2][1]+pos_temp[0][1]))/2;//�õ�yoff  
//					POINT_COLOR=BLUE;
					LCD_Clear(White);//����
					LCD_DisplayStringLine(35,"Touch Screen Adjust OK!",Red,Blue);//У�����
					Delay(20);
					LCD_Clear(Magenta);//����   
					return;//У�����				 
			}
		}
	} 
}
//��������ʼ��
void Touch_Init(void)
{
    Touch_Gpio_Init();
	TouchSPIx_Init();
	//NVIC_Configuration();
	//TOUCH_EXTI_Configuration();

	//Touch_Adjust();
}
/*****************************************************************************
** ��������: void Touch_IRQ(void)
** ��������: �����жϺ���
** ��  ����: 
** �ա�  ��: 
*****************************************************************************/
void Touch_IRQ(void)
{
	//u16 i;
  	if(EXTI_GetITStatus(TOUCH_IRQ_GPIO_Pin) != RESET)
	{
		//if(Read_TP_Once() == 1)
		Pen_Point.Key_Sta=Key_Down;//�������� 
		EXTI_ClearITPendingBit(TOUCH_IRQ_GPIO_Pin);	 //����жϹ���λ			 		  				 
	}
}






//SPI������ 
//��7846/7843/XPT2046/UH7843/UH7846��ȡadcֵ	   
uint16_t ADS_Read_AD(uint8_t CMD)	  
{ 	 
//	u8 i;	  
	uint16_t Num,Date; 

	Select_TOUCH_CS();

    SPI_SendByte(CMD); 
	//for(i=100;i>0;i--); 
	Num=0;
	Date=0;
	Num = SPI_SendByte(0x00);
	Date = Num << 8;
	Num = SPI_SendByte(0x00);

	Date |= Num; 

    Date >>=3;
    Date &=0XFFF;

	NotSelect_TOUCH_CS();
	return(Date); 
}

//��ȡһ������ֵ
//������ȡREAD_TIMES������,����Щ������������,
//Ȼ��ȥ����ͺ����LOST_VAL����,ȡƽ��ֵ 
#define READ_TIMES 3 //��ȡ����
#define LOST_VAL 1	  //����ֵ
uint16_t ADS_Read_XY(uint8_t xy)
{
	uint16_t i, j;
	uint16_t buf[READ_TIMES];
	uint16_t sum=0;
	uint16_t temp;
	for(i=0;i<READ_TIMES;i++)
	{				 
		buf[i]=ADS_Read_AD(xy);	    
	}				    
	for(i=0;i<READ_TIMES-1; i++)//����
	{
		for(j=i+1;j<READ_TIMES;j++)
		{
			if(buf[i]>buf[j])//��������
			{
				temp=buf[i];
				buf[i]=buf[j];
				buf[j]=temp;
			}
		}
	}	  
	sum=0;
	for(i=LOST_VAL;i<READ_TIMES-LOST_VAL;i++)sum+=buf[i];
	temp=sum/(READ_TIMES-2*LOST_VAL);
	return temp;   
} 

//���˲��������ȡ
//��Сֵ��������100.
u8 Read_ADS(uint16_t *x,uint16_t *y)
{
	uint16_t xtemp,ytemp;			 	 		  
	xtemp=ADS_Read_XY(CMD_RDX);
	ytemp=ADS_Read_XY(CMD_RDY);
//	xtemp=ADS_Read_AD(CMD_RDX);
//	ytemp=ADS_Read_AD(CMD_RDY);
	  												   
	if(xtemp<100||ytemp<100)return 0;//����ʧ��
	*x=xtemp;
	*y=ytemp;
	return 1;//�����ɹ�
}

//ת�����
//���ݴ�������У׼����������ת����Ľ��,������X0,Y0��
void Convert_Pos(void)
{		 	  
	if(Read_ADS2(&Pen_Point.X,&Pen_Point.Y))
	{
		Pen_Point.X0=Pen_Point.xfac*Pen_Point.X+Pen_Point.xoff;
		Pen_Point.Y0=Pen_Point.yfac*Pen_Point.Y+Pen_Point.yoff;  
	}
}

/*****************************************************************************
** ��������: Refreshes_Screen
** ��������: ˢ����Ļ				
** ��  ����: Dream
** �ա�  ��: 2010��12��06��
*****************************************************************************/
void Refreshes_Screen(void)
{
	LCD_Clear(White);//����  
	WriteString(216,0,"RST",Red);
//  	POINT_COLOR=BLUE;//���û�����ɫ 
}

//��һ�����
//2*2�ĵ�			   
void Draw_Big_Point(uint8_t x,uint16_t y)
{	    
	LCD_SetPoint(x,y,Red);//���ĵ� 
	LCD_SetPoint(x+1,y,Red);
	LCD_SetPoint(x,y+1,Red);
	LCD_SetPoint(x+1,y+1,Red);	 	  	
}
