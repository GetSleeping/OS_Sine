/****************************************Copyright (c)***************************************
**                                 http://www.openmcu.com
**--------------------------------------File Info--------------------------------------------
** File name:           lcd.c
** Last modified Date:  2012-08-13
** Last Version:        V1.00
** Descriptions:        3.2��lcd������
**
**-------------------------------------------------------------------------------------------
** Created by:          FXL
** Created date:        2012-08-13
** Version:             V1.00
** Descriptions:        3.2��LCD������
**
**-------------------------------------------------------------------------------------------
** Modified by:         
** Modified date:       
** Version:             
** Descriptions:        
**-------------------------------------------------------------------------------------------
********************************************************************************************/
#include "ssd1963_lcd.h"
#include "fonts.h"

/*****************************************************************
LCD/CSӲ�����ӣ�LCD/CS  CE4(NOR/SRAM Bank 4)
*****************************************************************/
#define LCD_BASE        ((u32)(0x60000000 | 0x0C000000))
#define LCD             ((LCD_TypeDef *) LCD_BASE)

//����LCD�ĳߴ�
#define LCD_W 240
#define LCD_H 320

/* ɫֵ ȫ�ֱ��� */
static  vu16 TextColor = 0x0000, BackColor = 0xFFFF;
/* ��ȡLCD��DeviceCode ȫ�ֱ��� */
u16 DeviceCode;
/* ��ʱ����ʱ�������� Ҳ���Լ�����*/
//extern void Delay(u32 nCount);

/****************************************************************************
* ��    �ƣ�void LCD_SetPoint(u16 x,u16 y,u16 point)
* ��    �ܣ���ָ�����껭��
* ��ڲ�����x      ������
*           y      ������
*           point  �����ɫ
* ���ڲ�������
* ˵    ����
* ���÷�����ili9320_SetPoint(10,10,0x0fe0);
****************************************************************************/
void LCD_SetPoint(u16 x,u16 y,u16 point)
{
  if ( (x>240)||(y>360) ) return;

  LCD_SetCursor(x,y);
  LCD_WriteRAM_Prepare();
  LCD_WriteRAM(point);
}


/****************************************************************************
* ��    �ƣ�void LCD_PutChar(u16 x,u16 y,u8 c,u16 charColor,u16 bkColor)
* ��    �ܣ���ָ��������ʾһ��16x24�����ascii�ַ�
* ��ڲ�����x          ������
*           y          ������
*           charColor  �ַ�����ɫ
*           bkColor    �ַ�������ɫ
* ���ڲ�������
* ˵    ������ʾ��Χ�޶�Ϊ����ʾ��ascii��
* ���÷�����ili9320_PutChar(10,10,'a',0x0000,0xffff);
****************************************************************************/
void LCD_PutChar(u16 x,u16 y,u8 c,u16 charColor,u16 bkColor)
{
  u16 i=0;
  u16 j=0;
  
  u16 tmp_char=0;

  for (i=0;i<24;i++)
  {
    tmp_char=ASCII_Table[((c-0x20)*24)+i];	 //��ȥ32��ƫ�ƣ�����Ϊ�ַ���ӿո�ʼ�ģ��ο��ַ����ǵ�ע��
    for (j=0;j<16;j++)
    {
//      if ( (tmp_char >> 15-j) & 0x01 == 0x01)	�����������ʾ���ַ��ǵ������ģ�������Ĺ���������
	if ( (tmp_char >> j) & 0x01 == 0x01)
      {
        LCD_SetPoint(x+i,y+j,charColor); // �ַ���ɫ
      }
      else
      {
        LCD_SetPoint(x+i,y+j,bkColor); // ������ɫ
      }
    }
  }
}

/****************************************************************************
* ��    �ƣ�u16 findHzIndex(u8 *hz)
* ��    �ܣ����Զ��庺�ֿ��ڲ�����Ҫ��ʾ�ĺ��ֵ�λ��
* ��ڲ�����hz ��ָ����(������)��ָ��
* ���ڲ�����u16�����ֵ�λ��
* ˵    ����
* ���÷�����
****************************************************************************/
u16 findHzIndex(u8 *hz)                     /* ���Զ��庺�ֿ��ڲ�����Ҫ��ʾ */
                                                      /* �ĺ��ֵ�λ�� */
{
    u16 i=0;
    
    FNT_GB16 *ptGb16 = (FNT_GB16 *)GBHZ_16;
    
    while(ptGb16[i].Index[0] > 0x80) {
        if ((*hz == ptGb16[i].Index[0]) && (*(hz+1) == ptGb16[i].Index[1])) 
		{
            return i;
        }
        i++;
        if(i > (sizeof((FNT_GB16 *)GBHZ_16) / sizeof(FNT_GB16) - 1))  /* �����±�Լ�� */
        {
		    break;
        }
    }
    return 0;
}

/****************************************************************************
* ��    �ƣ�u16 WriteOneHzChar(u8 *pucMsk,u16 x0,u16 y0,u16 color)
* ��    �ܣ���ʾһ��ָ����С�ĺ���
* ��ڲ�����u8 *pucMsk����ǰ�ֿ��ַ
*			u16 x0    ��X�������
*		    u16 y0    ��Y�������
*			u16 color : ������ɫ
* ���ڲ�����u16��16λ�п�
* ˵    ����
* ���÷�����
****************************************************************************/
u16 WriteOneHzChar(u8 *pucMsk,
                               u16 x0,
                               u16 y0,
                               u16 color)
{
    u16 i,j;
    u16 mod[16];                                      /* ��ǰ��ģ                     */
    u16 *pusMsk;                                      /* ��ǰ�ֿ��ַ                 */
    u16 y;
    
    pusMsk = (u16 *)pucMsk;
    for(i=0; i<16; i++)                                    /* ���浱ǰ���ֵ���ʽ��ģ       */
    {
        mod[i] = *pusMsk++;                                /* ȡ�õ�ǰ��ģ�����ֶ������   */
        mod[i] = ((mod[i] & 0xff00) >> 8) | ((mod[i] & 0x00ff) << 8);/* ��ģ�����ߵ��ֽڣ�Ϊ����ʾ   */
                                                           /* ��Ҫ��                       */
    }
    y = y0;
    for(i=0; i<16; i++)                                    /* 16��                         */
    { 
	    #ifdef __DISPLAY_BUFFER                            /* ʹ���Դ���ʾ                 */
        for(j=0; j<16; j++)                                /* 16��                         */
        {
		    if((mod[i] << j)& 0x8000)                      /* ��ʾ��ģ                     */
            {
			    DispBuf[240*(y0+i) + x0+j] = color;
            }
        }
        #else                                              /* ֱ����ʾ                     */
        
        LCD_SetCursor(x0, y);                              /* ����д���ݵ�ַָ��           */
		LCD_WriteRAM_Prepare();        					   /*��ʼд��GRAM	*/   
        for(j=0; j<16; j++)                                /* 16��                         */
        {
		    if((mod[i] << j) & 0x8000)                     /* ��ʾ��ģ                     */
            { 
			    LCD_WriteRAM(color);
            } 
			else 
			{
                LCD_WriteRAM(White);                     /* �ö���ʽ����д�հ׵������   */
            	//LCD_ReadDat();
			}
        }
        y++;
        #endif
    }
    return (16);                                          /* ����16λ�п�                 */
}

/****************************************************************************
* ��    �ƣ�u16 WriteOneASCII(u8 *pucMsk,u16 x0,u16 y0,u16 color)
* ��    �ܣ���ʾһ��ָ����С���ַ�
* ��ڲ�����u8 *pucMsk����ǰ�ֿ��ַ
*			u16 x0    ��X�������
*		    u16 y0    ��Y�������
*			u16 color : ������ɫ
* ���ڲ�����u16��16λ�п�
* ˵    ����
* ���÷�����
****************************************************************************/
u16 WriteOneASCII(u8 *pucMsk,
                              u16 x0,
                              u16 y0,
                              u16 color)
{
    u16 i,j;
    u16 y;
    u8 ucChar;
    
    y = y0;
    for(i=0; i<16; i++) {                                 /* 16��                         */
        ucChar = *pucMsk++;
        #ifdef __DISPLAY_BUFFER                           /* ʹ���Դ���ʾ                 */
        for(j=0; j<8; j++) {                              /* 8��                          */
            if((ucChar << j)& 0x80) {                     /* ��ʾ��ģ                     */
                DispBuf[240*(y0+i) + x0+j] = color;
            }
        }
        #else                                             /* ֱ����ʾ                     */
        
        LCD_SetCursor(x0, y);                             /* ����д���ݵ�ַָ��           */
		LCD_WriteRAM_Prepare();        					  /* ��ʼд��GRAM	    		  */
        for(j=0; j<8; j++) {                              /* 8��                          */
            if((ucChar << j) & 0x80) {                    /* ��ʾ��ģ                     */
                LCD_WriteRAM(color);
            } else {
                LCD_WriteRAM(White);
				  //LCD_ReadDat();
            }
        }
        y++;
        #endif
    }
    return (8);                                           /* ����16λ�п�                 */
} 
/****************************************************************************
* ��    �ƣ�void WriteString(u16 x0, u16 y0,u8 *pcStr, u16 color)
* ��    �ܣ���ָ��λ�ÿ�ʼ��ʾһ���ַ�����һ������ ֧���Զ�����
* ��ڲ�����u16 x0    ��X�������
*		    u16 y0    ��Y�������
*			u8 *pcStr : �ֵ�ַ
*			u16 color : ������ɫ
* ���ڲ�������
* ˵    ����
* ���÷�����
****************************************************************************/  			   
void WriteString(u16 x0, u16 y0,u8 *pcStr, u16 color)
{
	u16 usIndex;
    u16 usWidth = 0;
    FNT_GB16 *ptGb16 = 0;
    
    ptGb16 = (FNT_GB16 *)GBHZ_16;  
	while(1)
    {
        if(*pcStr == 0) 
		{
            break;                                     /* �ַ�������            */
        }      
        x0 = x0 + (usWidth);                           /* �����ַ�����ʾ�ɽ���         */
        if(*pcStr > 0x80)                              /* �ж�Ϊ����                   */
        {
		    if((x0 + 16) > LCD_W)                      /* ���ʣ��ռ��Ƿ��㹻         */
            {
			    x0 = 0;
                y0 = y0 + 16;                          /* �ı���ʾ����                 */
                if(y0 > LCD_H)                         /* �����곬��                   */
                {
				    y0 = 0;
                }
            }
            usIndex = findHzIndex(pcStr);
            usWidth = WriteOneHzChar((u8 *)&(ptGb16[usIndex].Msk[0]), x0, y0, color);
                                                       /* ��ʾ�ַ�                     */
            pcStr += 2;
        }
		else 
		{                                               /* �ж�Ϊ�Ǻ���                 */
            if (*pcStr == '\r')                         /* ����                         */
            { 
			    y0 = y0 + 16;                           /* �ı���ʾ����                 */
                if(y0 > LCD_H)                          /* �����곬��                   */
                {
				    y0 = 0;
                }
                pcStr++;
                usWidth = 0;
                continue;
            } 
			else if (*pcStr == '\n')                    /* ���뵽���                   */
            {
			    x0 = 0;
                pcStr++;
                usWidth = 0;
                continue;
            } 
			else 
			{
                if((x0 + 8) > LCD_W)                     /* ���ʣ��ռ��Ƿ��㹻         */
                {
				    x0 = 0;
                    y0 = y0 + 16;                        /* �ı���ʾ����                 */
                    if(y0 > LCD_H)                       /* �����곬��                   */
                    { 
					    y0 = 0;
                    }
                }
                usWidth = WriteOneASCII((u8 *)&ASCII_1608[(*pcStr - 0x20)][0], x0, y0, color);
                                                         /* ASCII���21H��ֵ��Ӧ��λ��3��*/
                pcStr += 1;
            }
		}
	}												  	  
}

/****************************************************************************
* ��    �ƣ�void LCD_DisplayStringLine(u8 Line, u8 *ptr, u16 charColor, u16 bkColor)
* ��    �ܣ���ʾ���40���ַ�һ����LCD��
* ��ڲ�����Line ���� *ptrָ���ַ�����ָ�� charColor�ַ���ɫ bkColor������ɫ
* ���ڲ�������
* ˵    ����
* ���÷�����LCD_DisplayStringLine(Line0,"I Love you...",White,Blue);  
****************************************************************************/
void LCD_DisplayStringLine(u8 Line, u8 *ptr, u16 charColor, u16 bkColor)
{
  u32 i = 0;
  u16 refcolumn = 0;

  /* Send the string character by character on lCD */
  while ((*ptr != 0) & (i < 40))
  {
    /* Display one character on LCD */
//    LCD_PutChar(refcolumn, Line, *ptr, charColor, bkColor);
	LCD_PutChar(Line, refcolumn, *ptr, charColor, bkColor);
    /* Decrement the column position by 16 */
    refcolumn += 16;
    /* Point on the next character */
    ptr++;
    /* Increment the character counter */
    i++;
  }
}
/****************************************************************************
* ��    �ƣ�void Draw_Circle(u8 x0,u16 y0,u8 r,u16 color)
* ��    �ܣ���ָ��λ�û�һ��ָ����С��Բ
* ��ڲ�����(x,y):���ĵ� 
*       	 r   :�뾶
*           u16 color:��ɫ
* ���ڲ�������
* ˵    ������ָ��λ�û�һ��ָ����С��Բ
* ���÷�����
****************************************************************************/
void Draw_Circle(u8 x0,u16 y0,u8 r,u16 color)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //�ж��¸���λ�õı�־
	while(a<=b)
	{
		LCD_SetPoint(x0-b,y0-a,color);             //3           
		LCD_SetPoint(x0+b,y0-a,color);             //0           
		LCD_SetPoint(x0-a,y0+b,color);             //1       
		LCD_SetPoint(x0-b,y0-a,color);             //7           
		LCD_SetPoint(x0-a,y0-b,color);             //2             
		LCD_SetPoint(x0+b,y0+a,color);             //4               
		LCD_SetPoint(x0+a,y0-b,color);             //5
		LCD_SetPoint(x0+a,y0+b,color);             //6 
		LCD_SetPoint(x0-b,y0+a,color);             
		a++;
		//ʹ��Bresenham�㷨��Բ     
		if(di<0)di +=4*a+6;	  
		else
		{
			di+=10+4*(a-b);   
			b--;
		} 
		LCD_SetPoint(x0+a,y0+b,color);
	}
}
/****************************************************************************
* ��    �ƣ�void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
* ��    �ܣ���ָ��������λ�û�һ����
* ��ڲ�����x1,y1:�������� 
*       	x2,y2:�յ�����
*           u16 color:��ɫ 
* ���ڲ�������
* ˵    ������ָ��λ�û�һ��ָ����С��Բ
* ���÷�����
****************************************************************************/
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1; 				//������������ 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; 		//���õ������� 
	else if(delta_x==0)incx=0;	//��ֱ�� 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;	//ˮƽ�� 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //ѡȡ�������������� 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )	//������� 
	{  
		LCD_SetPoint(uRow,uCol,color);//���� 
		xerr+=delta_x ; 
		yerr+=delta_y ; 
		if(xerr>distance) 
		{ 
			xerr-=distance; 
			uRow+=incx; 
		} 
		if(yerr>distance) 
		{ 
			yerr-=distance; 
			uCol+=incy; 
		} 
	}  
}
/****************************************************************************
* ��    �ƣ�u16 LCD_BGR2RGB(u16 c)
* ��    �ܣ�RRRRRGGGGGGBBBBB ��Ϊ BBBBBGGGGGGRRRRR ��ʽ
* ��ڲ�����c      BRG ��ɫֵ
* ���ڲ�����RGB ��ɫֵ
* ˵    �����ڲ���������
* ���÷�����
****************************************************************************/
u16 LCD_BGR2RGB(u16 c)
{
  u16  r, g, b, rgb;

  b = (c>>0)  & 0x1f;
  g = (c>>5)  & 0x3f;
  r = (c>>11) & 0x1f;
  
  rgb =  (b<<11) + (g<<5) + (r<<0);

  return( rgb );
}

/****************************************************************************
* ��    �ƣ�void STM3210E_LCD_Init(void)
* ��    �ܣ�LCD����ʼ��
* ��ڲ�������
* ���ڲ�������
* ˵    ����LCD����ʼ��
* ���÷�����STM3210E_LCD_Init();
****************************************************************************/
void STM3210E_LCD_Init(void)
{ 
/* LCD�ܽų�ʼ�� --------------------------------------------*/
  LCD_CtrlLinesConfig();

/* FSMC��ʼ�� -----------------------------------------------*/
  LCD_FSMCConfig();

  DeviceCode = LCD_ReadReg(0x0000); 
  if(DeviceCode != 0x8999)
      DeviceCode = 0x8999;

  if(DeviceCode==0x9325)			  
  {
//	  Delay(5); /* ��ʱ 50 ms */
//	  Delay(5); /* ��ʱ 50 ms */
	  
	  LCD_WriteReg(0x00FF,0x0001);
	  LCD_WriteReg(0x00F3,0x0008);
	  LCD_WriteReg(0x0001,0x0100);
	  LCD_WriteReg(0x0002,0x0700);
	  LCD_WriteReg(0x0003,0x1030);  //0x1030
	  LCD_WriteReg(0x0008,0x0302);
	  LCD_WriteReg(0x0008,0x0207);
	  LCD_WriteReg(0x0009,0x0000);
	  LCD_WriteReg(0x000A,0x0000);
	  LCD_WriteReg(0x0010,0x0000);  //0x0790
	  LCD_WriteReg(0x0011,0x0005);
	  LCD_WriteReg(0x0012,0x0000);
	  LCD_WriteReg(0x0013,0x0000);
//	  Delay(5);
	  LCD_WriteReg(0x0010,0x12B0);
//	  Delay(5);
	  LCD_WriteReg(0x0011,0x0007);
//	  Delay(5);
	  LCD_WriteReg(0x0012,0x008B);
//	  Delay(5);
	  LCD_WriteReg(0x0013,0x1700);
//	  Delay(5);
	  LCD_WriteReg(0x0029,0x0022);

	  LCD_WriteReg(0x0030,0x0000);
	  LCD_WriteReg(0x0031,0x0707);
	  LCD_WriteReg(0x0032,0x0505);
	  LCD_WriteReg(0x0035,0x0107);
	  LCD_WriteReg(0x0036,0x0008);
	  LCD_WriteReg(0x0037,0x0000);
	  LCD_WriteReg(0x0038,0x0202);
	  LCD_WriteReg(0x0039,0x0106);
	  LCD_WriteReg(0x003C,0x0202);
	  LCD_WriteReg(0x003D,0x0408);
//	  Delay(5);
			
	  LCD_WriteReg(0x0050,0x0000);		
	  LCD_WriteReg(0x0051,0x00EF);		
	  LCD_WriteReg(0x0052,0x0000);		
	  LCD_WriteReg(0x0053,0x013F);		
	  LCD_WriteReg(0x0060,0x2700);		
	  LCD_WriteReg(0x0061,0x0001);
	  LCD_WriteReg(0x0090,0x0033);				
	  LCD_WriteReg(0x002B,0x000B);		
	  LCD_WriteReg(0x0007,0x0133);
//	  Delay(5); 
	}
	else if(DeviceCode==0x9320)	
	{				
//		  Delay(5); /* ��ʱ 50 ms */
		
		  LCD_WriteReg(R229,0x8000); /* Set the internal vcore voltage */
		  LCD_WriteReg(R0,  0x0001); /* Start internal OSC. */
		  LCD_WriteReg(R1,  0x0100); /* set SS and SM bit */
		  LCD_WriteReg(R2,  0x0700); /* set 1 line inversion */
		  LCD_WriteReg(R3,  0x1030); /* set GRAM write direction and BGR=1. */
		  LCD_WriteReg(R4,  0x0000); /* Resize register */
		  LCD_WriteReg(R8,  0x0202); /* set the back porch and front porch */
		  LCD_WriteReg(R9,  0x0000); /* set non-display area refresh cycle ISC[3:0] */
		  LCD_WriteReg(R10, 0x0000); /* FMARK function */
		  LCD_WriteReg(R12, 0x0000); /* RGB interface setting */
		  LCD_WriteReg(R13, 0x0000); /* Frame marker Position */
		  LCD_WriteReg(R15, 0x0000); /* RGB interface polarity */
		
		/* Power On sequence ---------------------------------------------------------*/
		  LCD_WriteReg(R16, 0x0000); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
		  LCD_WriteReg(R17, 0x0000); /* DC1[2:0], DC0[2:0], VC[2:0] */
		  LCD_WriteReg(R18, 0x0000); /* VREG1OUT voltage */
		  LCD_WriteReg(R19, 0x0000); /* VDV[4:0] for VCOM amplitude */
//		  Delay(20);                 /* Dis-charge capacitor power voltage (200ms) */
		  LCD_WriteReg(R16, 0x17B0); /* SAP, BT[3:0], AP, DSTB, SLP, STB */
		  LCD_WriteReg(R17, 0x0137); /* DC1[2:0], DC0[2:0], VC[2:0] */
//		  Delay(5);                  /* Delay 50 ms */
		  LCD_WriteReg(R18, 0x0139); /* VREG1OUT voltage */
//		  Delay(5);                  /* Delay 50 ms */
		  LCD_WriteReg(R19, 0x1d00); /* VDV[4:0] for VCOM amplitude */
		  LCD_WriteReg(R41, 0x0013); /* VCM[4:0] for VCOMH */
//		  Delay(5);                  /* Delay 50 ms */
		  LCD_WriteReg(R32, 0x0000); /* GRAM horizontal Address */
		  LCD_WriteReg(R33, 0x0000); /* GRAM Vertical Address */
		
		/* Adjust the Gamma Curve ----------------------------------------------------*/
		  LCD_WriteReg(R48, 0x0006);
		  LCD_WriteReg(R49, 0x0101);
		  LCD_WriteReg(R50, 0x0003);
		  LCD_WriteReg(R53, 0x0106);
		  LCD_WriteReg(R54, 0x0b02);
		  LCD_WriteReg(R55, 0x0302);
		  LCD_WriteReg(R56, 0x0707);
		  LCD_WriteReg(R57, 0x0007);
		  LCD_WriteReg(R60, 0x0600);
		  LCD_WriteReg(R61, 0x020b);
		  
		/* Set GRAM area -------------------------------------------------------------*/
		  LCD_WriteReg(R80, 0x0000); /* Horizontal GRAM Start Address */
		  LCD_WriteReg(R81, 0x00EF); /* Horizontal GRAM End Address */
		  LCD_WriteReg(R82, 0x0000); /* Vertical GRAM Start Address */
		  LCD_WriteReg(R83, 0x013F); /* Vertical GRAM End Address */
				
		  LCD_WriteReg(R96,  0xA700); /* 3.2-->9320->0xA700 Gate Scan Line */
		
		  LCD_WriteReg(R97,  0x0001); /* NDL,VLE, REV */
		  LCD_WriteReg(R106, 0x0000); /* set scrolling line */
		
		/* Partial Display Control ---------------------------------------------------*/
		  LCD_WriteReg(R128, 0x0000);
		  LCD_WriteReg(R129, 0x0000);
		  LCD_WriteReg(R130, 0x0000);
		  LCD_WriteReg(R131, 0x0000);
		  LCD_WriteReg(R132, 0x0000);
		  LCD_WriteReg(R133, 0x0000);
		
		/* Panel Control -------------------------------------------------------------*/
		  LCD_WriteReg(R144, 0x0010);
		  LCD_WriteReg(R146, 0x0000);
		  LCD_WriteReg(R147, 0x0003);
		  LCD_WriteReg(R149, 0x0110);
		  LCD_WriteReg(R151, 0x0000);
		  LCD_WriteReg(R152, 0x0000);
		
		  /* Set GRAM write direction and BGR = 1 */
		  /* I/D=01 (Horizontal : increment, Vertical : decrement) */
		  /* AM=1 (address is updated in vertical writing direction) */
		  LCD_WriteReg(R3, 0x1018);
		
		  LCD_WriteReg(R7, 0x0173); /* 262K color and display ON */
		  
//		  Delay(5); /* delay 50 ms */
//		  Delay(5); /* delay 50 ms */			//start internal osc
	}
	else if(DeviceCode==0x8999)	   //��Ӧ������ICΪSSD1289
	{
		//************* Start Initial Sequence **********//
		LCD_WriteReg(0x00, 0x0001); // Start internal OSC.
		LCD_WriteReg(0x01, 0x7b3F); // Driver o 0011 1101 utput control, RL=0;REV=1;GD=1;BGR=0;SM=0;TB=1
		LCD_WriteReg(0x02, 0x0600); // set 1 line inversion
		//************* Power control setup ************/
		LCD_WriteReg(0x0C, 0x0007); // Adjust VCIX2 output voltage
		LCD_WriteReg(0x0D, 0x0006); // Set amplitude magnification of VLCD63
		LCD_WriteReg(0x0E, 0x3200); // Set alternating amplitude of VCOM
		LCD_WriteReg(0x1E, 0x00BB); // Set VcomH voltage
		LCD_WriteReg(0x03, 0x6A64); // Step-up factor/cycle setting
		//************ RAM position control **********/
		LCD_WriteReg(0x0F, 0x0000); // Gate scan position start at G0.
		LCD_WriteReg(0x44, 0xEF00); // Horizontal RAM address position
		LCD_WriteReg(0x45, 0x0000); // Vertical RAM address start position
		LCD_WriteReg(0x46, 0x013F); // Vertical RAM address end position
		// ----------- Adjust the Gamma Curve ----------//
		LCD_WriteReg(0x30, 0x0000);
		LCD_WriteReg(0x31, 0x0706);
		LCD_WriteReg(0x32, 0x0206);
		LCD_WriteReg(0x33, 0x0300);
		LCD_WriteReg(0x34, 0x0002);
		LCD_WriteReg(0x35, 0x0000);
		LCD_WriteReg(0x36, 0x0707);
		LCD_WriteReg(0x37, 0x0200);
		LCD_WriteReg(0x3A, 0x0908);
		LCD_WriteReg(0x3B, 0x0F0D);
		//************* Special command **************/
		LCD_WriteReg(0x28, 0x0006); // Enable test command
		LCD_WriteReg(0x2F, 0x12EB); // RAM speed tuning
		LCD_WriteReg(0x26, 0x7000); // Internal Bandgap strength
		LCD_WriteReg(0x20, 0xB0E3); // Internal Vcom strength
		LCD_WriteReg(0x27, 0x0044); // Internal Vcomh/VcomL timing
		LCD_WriteReg(0x2E, 0x7E45); // VCOM charge sharing time  
		//************* Turn On display ******************/
		LCD_WriteReg(0x10, 0x0000); // Sleep mode off.
		//Delay(8); // Wait 30mS
		LCD_WriteReg(0x11, 0x6870);// Entry mode setup. 262K type B, take care on the data bus with 16it only
		LCD_WriteReg(0x07, 0x0033); // Display ON	*/
	}
	LCD_Clear(Black);	
}

/****************************************************************************
* ��    �ƣ�void LCD_Clear(u16 Color)
* ��    �ܣ�LCD����
* ��ڲ�����u16 Color������ɫֵ
* ���ڲ�������
* ˵    ����LCD����
* ���÷�����void LCD_Clear(Black)
****************************************************************************/
void LCD_Clear(u16 Color)
{
  u32 index = 0;
  
  LCD_SetCursor(0x00, 0x00); 

  LCD_WriteRAM_Prepare(); /* Prepare to write GRAM */

  for(index = 0; index < 76800; index++)
  {
    LCD->LCD_RAM = Color;
  }  
}

/****************************************************************************
* ��    �ƣ�void LCD_SetCursor(u8 Xpos, u16 Ypos)
* ��    �ܣ����ù��λ��
* ��ڲ�����u8 Xpos : X����
*           u16 Ypos��Y����
* ���ڲ�������
* ˵    �������ù��λ��
* ���÷�����LCD_SetCursor(0��0)
****************************************************************************/
void LCD_SetCursor(u8 Xpos, u16 Ypos)
{
 	if(DeviceCode==0x8999||DeviceCode==0x9919)
	{
		LCD_WriteReg(0x004E, Xpos);
		LCD_WriteReg(0X004F, 320 - Ypos);
	}
	else 
	{
		LCD_WriteReg(0x0020, Xpos);
		LCD_WriteReg(0X0021, Ypos);
	}
}

/****************************************************************************
* ��    �ƣ�void LCD_SetDisplayWindow(u8 Xpos, u16 Ypos, u8 Height, u16 Width)
* ��    �ܣ�����һ����ʾ����
* ��ڲ�����u8 Xpos  : ��ʾ����X��
*           u16 Ypos ����ʾ����Y��
*			u8 Height����ʾ���ڵĳ�
*			u16 Width����ʾ���ڵĿ�
* ���ڲ�������
* ˵    ��������һ����ʾ����
* ���÷�����LCD_SetDisplayWindow(0,0,240,320)
****************************************************************************/
void LCD_SetDisplayWindow(u8 Xpos, u16 Ypos, u8 Height, u16 Width)
{
	if(DeviceCode==0x8999)
	{
    	LCD_WriteReg(0x44,Xpos|(Xpos+Height<<8));
    	LCD_WriteReg(0x45,Ypos);
    	LCD_WriteReg(0x46,Ypos + Width);
	}
	else
	{
		LCD_WriteReg(0x50, Xpos);                                                /*Horizontal GRAM Start Address */
		LCD_WriteReg(0x51, Xpos+Height);                                          /*Horizontal GRAM EndAddress(-1)*/
		LCD_WriteReg(0x52, Ypos);                                                /*Vertical GRAM Start Address   */
		LCD_WriteReg(0x53, Ypos+Width);                                          /*Vertical GRAM End Address (-1)*/
	}
    LCD_SetCursor(Xpos, Ypos);
}
/************************************************************************************
* ��    �ƣ�void ShowOneChinese(u8 Number, u8 x, u16 y,u16 PenColor,u16 BackColor)
* ��    �ܣ���ʾһ�����֣���ȡģ���� ������FONTS.h�У�
* ��ڲ�����u8 Number    : ��ģ�����еĵڼ�������
*			u8 x         ��X������
*           u16 y        ��Y������
*			u16 PenColor ������ɫ
*			u16 BackColor������ɫd
* ���ڲ�������
* ˵    ������ʾһ������
* ���÷�����
************************************************************************************/
void ShowOneChinese(u8 Number, u8 x, u16 y,u16 PenColor,u16 BackColor)
{
    u8 i,j;
	/*  ����Ϊ16*16  */
	u8 sizex = 16;
	u8 sizey = 16;	
	u8 *Chinese_buf = &Chinese_Table[Number * sizex * 2];
 
    for(i=0; i<sizex; i++)                        /* sizex��   */
    {                                              
        for(j=0; j<sizey; j++)                    /* sizey��   */
        {
		    if((Chinese_buf[i*2+j/8] << (j%8)) & 0x80)      /* ��ʾ��i�� ��16���� */
            { 
			    LCD_SetPoint(x+i,y+j,PenColor);
            } 
			else 
			{
                LCD_SetPoint(x+i,y+j,BackColor);         
			}
        }
    }
}
/************************************************************************************
* ��    �ƣ�void ShowOneChinese(u8 Number, u8 x, u16 y,u16 PenColor,u16 BackColor)
* ��    �ܣ���ʾһ�����֣���ȡģ���� ������FONTS.h�У�
* ��ڲ�����u8 Number    : ��ģ�����еĵڼ�������
*			u8 x         ��X������
*           u16 y        ��Y������
*			u16 PenColor ������ɫ
*			u16 BackColor������ɫd
* ���ڲ�������
* ˵    ��������һ����ʾ����
* ���÷�����LCD_SetDisplayWindow(0,0,240,320)
************************************************************************************/
void ShowLineChinese(u8 Number,u8 counter ,u8 x, u16 y,u16 PenColor,u16 BackColor)
{
    u8 i;
    for(i=0;i<counter;i++)
	{
	    ShowOneChinese(Number + i, x , y + i*16,PenColor ,BackColor);    
	}
}

/************************************************************************************
* ��    �ƣ�void LCD_WriteBMP(u8 Xpos, u16 Ypos, u8 Height, u16 Width, u8 *bitmap)
* ��    �ܣ���ͼ
* ��ڲ�����u8 Xpos   : ��ʾͼ��X��
*           u16 Ypos  ����ʾͼ��Y��
*			u8 Height ����ʾͼ�ĳ�
*			u16 Width ����ʾͼ�Ŀ�
*			u8 *bitmap: ��ʾͼ����ָ��
* ���ڲ�������
* ˵    ������ʾһ��ͼƬ
* ���÷�����
************************************************************************************/
void LCD_WriteBMP(u16 Xpos, u16 Ypos, u16 Height, u16 Width, u8 *bitmap)
{
  vu32 index;
  vu32 size = Height * Width;
  u16 *bitmap_ptr = (u16 *)bitmap;

  LCD_SetDisplayWindow(Xpos,Ypos , Height-1, Width-1);

  /* Set GRAM write direction and BGR = 1 */
  /* I/D=00 (Horizontal : decrement, Vertical : decrement) */
  /* AM=1 (address is updated in vertical writing direction) */
  LCD_WriteReg(R3, 0x1038);
 
  LCD_WriteRAM_Prepare();
  bitmap_ptr += size - 1;
  for(index = 0; index < size; index++)
  {
//	if(*bitmap_ptr == 0xffff)	 //Ϊ����ʾ͸��
//	{
//		bitmap_ptr--;
//
//	}   
//	else
//	{
	    LCD_WriteRAM(*bitmap_ptr--);
//	}
  }
 
  /* Set GRAM write direction and BGR = 1 */
  /* I/D = 01 (Horizontal : increment, Vertical : decrement) */
  /* AM = 1 (address is updated in vertical writing direction) */
  LCD_WriteReg(R3, 0x1018);
  LCD_SetDisplayWindow(00, 00, 239, 319);
}
/****************************************************************************
* ��    �ƣ�void LCD_DrawPicture(u16 StartX,u16 StartY,u16 EndX,u16 EndY,u16 *pic)
* ��    �ܣ���ָ�����귶Χ��ʾһ��ͼƬ
* ��ڲ�����StartX     ����ʼ����
*           StartY     ����ʼ����
*           EndX       �н�������
*           EndY       �н�������
            pic        ͼƬͷָ��
* ���ڲ�������
* ˵    ����ͼƬȡģ��ʽΪˮƽɨ�裬16λ��ɫģʽ
* ���÷�����ili9320_DrawPicture(0,0,100,100,(u16*)demo);
****************************************************************************/
void LCD_DrawPicture(u16 StartX,u16 StartY,u16 Height,u16 Width,u8 *pic)
{
  u32  i;
  
  u16 *bitmap = (u16 *)pic;
  if(DeviceCode==0x8999)
  {
	//����ͼƬ��ʾ���ڴ�С
	LCD_WriteReg(0x44,StartX|(StartX+Width-1)<<8); //����ˮƽ����GRAM��ʼ��ַ(���ֽ�Ϊ��ʼ��ַ�����ֽ�Ϊ������ַ)
	LCD_WriteReg(0x45,StartY);				  	//���ô��ڵ�Y����Ŀ�ʼλ��
	LCD_WriteReg(0x46,StartY+Height-1);			//���ô��ڵ�Y����Ľ���λ��
  }
  else 
  {
  	LCD_WriteReg(R80, StartX);	   	   	//ˮƽ����GRAM��ʼ��ַ
  	LCD_WriteReg(R81, StartX+Width-1); 	//ˮƽ����GRAM������ַ 
  	LCD_WriteReg(R82, StartY);		  	//��ֱ����GRAM��ʼ��ַ
  	LCD_WriteReg(R83, StartY+Height-1);  	//��ֱ����GRAM������ַ
  }

  LCD_SetCursor(StartX,StartY);
  LCD_WriteRAM_Prepare();

  for (i=0;i<(Height*Width);i++)
  {
    LCD_WriteRAM(*bitmap++);
  }

  if(DeviceCode==0x8999)
  {
	//�ָ�����
	LCD_WriteReg(0x44,0|239<<8); 					//���ô��ڵ�X����ʼλ�úͽ���λ��
	LCD_WriteReg(0x45,0);				  			//���ô��ڵ�Y����Ŀ�ʼλ��
	LCD_WriteReg(0x46,319);							//���ô��ڵ�Y����Ľ���λ��
  }
  else
  {
  	LCD_WriteReg(R80, 0);	   	   	//ˮƽ����GRAM��ʼ��ַ
  	LCD_WriteReg(R81, 0xef); 	//ˮƽ����GRAM������ַ 
  	LCD_WriteReg(R82, 0);		  	//��ֱ����GRAM��ʼ��ַ
  	LCD_WriteReg(R83, 0x13f);  	//��ֱ����GRAM������ַ
  }
}
//�����ͼ
void LCD_DrawPicture_dot(u16 StartX,u16 StartY,u16 Height,u16 Width,u8 *pic)
{
//  u32  i;
  u16 i,j;
  
  u16 *bitmap = (u16 *)pic;
  if(DeviceCode==0x8999)
  {
	//����ͼƬ��ʾ���ڴ�С
	LCD_WriteReg(0x44,StartX|(StartX+Width-1)<<8); //����ˮƽ����GRAM��ʼ��ַ(���ֽ�Ϊ��ʼ��ַ�����ֽ�Ϊ������ַ)
	LCD_WriteReg(0x45,320 - StartY - Height - 1);				  	//���ô��ڵ�Y����Ŀ�ʼλ��
	LCD_WriteReg(0x46,320 - StartY);			//���ô��ڵ�Y����Ľ���λ��
  }
  else 
  {
  	LCD_WriteReg(R80, StartX);	   	   	//ˮƽ����GRAM��ʼ��ַ
  	LCD_WriteReg(R81, StartX+Width-1); 	//ˮƽ����GRAM������ַ 
  	LCD_WriteReg(R82, StartY);		  	//��ֱ����GRAM��ʼ��ַ
  	LCD_WriteReg(R83, StartY+Height-1);  	//��ֱ����GRAM������ַ
  }

  LCD_SetCursor(StartX,StartY);
  //bitmap += Height * Width - 1;
  for(i = StartX;i < (StartX + Height);i++)
  {
    for(j = StartY;j < (StartY + Width);j++)
	{
	  if((i<240) && (j<320))
	  {
	    if(*bitmap == 0xffff)
          bitmap++;
		else
	      LCD_SetPoint(i,j,*bitmap++);
	  }
	  else
	  {
	    bitmap++; 
	  }
	}
  }


  if(DeviceCode==0x8999)
  {
	//�ָ�����
	LCD_WriteReg(0x44,0|239<<8); 					//���ô��ڵ�X����ʼλ�úͽ���λ��
	LCD_WriteReg(0x45,0);				  			//���ô��ڵ�Y����Ŀ�ʼλ��
	LCD_WriteReg(0x46,319);							//���ô��ڵ�Y����Ľ���λ��
  }
  else
  {
  	LCD_WriteReg(R80, 0);	   	   	//ˮƽ����GRAM��ʼ��ַ
  	LCD_WriteReg(R81, 0xef); 	//ˮƽ����GRAM������ַ 
  	LCD_WriteReg(R82, 0);		  	//��ֱ����GRAM��ʼ��ַ
  	LCD_WriteReg(R83, 0x13f);  	//��ֱ����GRAM������ַ
  }
}

/************************************************************************************
* ��    �ƣ�void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
* ��    �ܣ��Ĵ��� дֵ
* ��ڲ�����u8 LCD_Reg        : �Ĵ���
*           u16 LCD_RegValue  ��Ҫд���ֵ
* ���ڲ�������
* ˵    �����Ĵ��� дֵ
* ���÷�����
************************************************************************************/
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
{
  /* Write 16-bit Index, then Write Reg */
  LCD->LCD_REG = LCD_Reg;
  /* Write 16-bit Reg */
  LCD->LCD_RAM = LCD_RegValue;
}

/************************************************************************************
* ��    �ƣ�u16 LCD_ReadReg(u8 LCD_Reg)
* ��    �ܣ��� �Ĵ��� ��ֵ
* ��ڲ�����u8 LCD_Reg        : �Ĵ���
* ���ڲ�����Ҫ�� �Ĵ��� ��ֵ
* ˵    ������ �Ĵ��� ��ֵ
* ���÷�����
************************************************************************************/
u16 LCD_ReadReg(u8 LCD_Reg)
{
  /* Write 16-bit Index (then Read Reg) */
  LCD->LCD_REG = LCD_Reg;
  LCD->LCD_RAM;
  /* Read 16-bit Reg */
  return (LCD->LCD_RAM);
}

/************************************************************************************
* ��    �ƣ�void LCD_WriteRAM_Prepare(void)
* ��    �ܣ�׼��д RAM
* ��ڲ�������
* ���ڲ�������
* ˵    ����
* ���÷�����
************************************************************************************/
void LCD_WriteRAM_Prepare(void)
{
  LCD->LCD_REG = R34;
}

/************************************************************************************
* ��    �ƣ�void LCD_WriteRAM(u16 RGB_Code)
* ��    �ܣ�дLCD RAM
* ��ڲ�����u16 RGB_Code����ɫģʽ��5-6-5��
* ���ڲ�������
* ˵    ����
* ���÷�����
************************************************************************************/
void LCD_WriteRAM(u16 RGB_Code)
{
  /* Write 16-bit GRAM Reg */
  LCD->LCD_RAM = RGB_Code;
}

/************************************************************************************
* ��    �ƣ�u16 LCD_ReadRAM(void)
* ��    �ܣ���LCD RAM
* ��ڲ������� 
* ���ڲ�����u16 : LCD RAMֵ
* ˵    ����
* ���÷�����
************************************************************************************/
u16 LCD_ReadRAM(void)
{
  /* Write 16-bit Index (then Read Reg) */
  LCD->LCD_REG = R34; /* Select GRAM Reg */
  /* Read 16-bit Reg */
  return LCD->LCD_RAM;
}
u16 LCD_ReadData(void)
{
 u16 val =0;
 val = LCD->LCD_RAM;
 return val;
}

/************************************************************************************
* ��    �ƣ�void LCD_PowerOn(void)
* ��    �ܣ�ʹ����ʾ
* ��ڲ������� 
* ���ڲ�������
* ˵    ����
* ���÷�����
************************************************************************************/
void LCD_DisplayOn(void)
{
  LCD_WriteReg(R7, 0x0173); /* 262K color and display ON */
}

/************************************************************************************
* ��    �ƣ�void LCD_PowerOn(void)
* ��    �ܣ�ʧ����ʾ
* ��ڲ������� 
* ���ڲ�������
* ˵    ����
* ���÷�����
************************************************************************************/
void LCD_DisplayOff(void)
{
  LCD_WriteReg(R7, 0x0); 
}

/************************************************************************************
* ��    �ƣ�void LCD_CtrlLinesConfig(void)
* ��    �ܣ�lcd��IO�ڳ�ʼ��(FSMC)
* ��ڲ������� 
* ���ڲ�������
* ˵    ����
* ���÷�����
************************************************************************************/
void LCD_CtrlLinesConfig(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable FSMC, GPIOD, GPIOE, GPIOF, GPIOG and AFIO clocks */
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_FSMC, ENABLE);

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE |
                         RCC_APB2Periph_GPIOF | RCC_APB2Periph_GPIOG |
                         RCC_APB2Periph_AFIO, ENABLE);

  /* Set PD.00(D2), PD.01(D3), PD.04(NOE), PD.05(NWE), PD.08(D13), PD.09(D14),
     PD.10(D15), PD.14(D0), PD.15(D1) as alternate 
     function push pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_4 | GPIO_Pin_5 |
                                GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOD, &GPIO_InitStructure);

  /* Set PE.07(D4), PE.08(D5), PE.09(D6), PE.10(D7), PE.11(D8), PE.12(D9), PE.13(D10),
     PE.14(D11), PE.15(D12) as alternate function push pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10 | 
                                GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | 
                                GPIO_Pin_15;
  GPIO_Init(GPIOE, &GPIO_InitStructure);

  GPIO_WriteBit(GPIOE, GPIO_Pin_6, Bit_SET);

  /* Set PF.00(A0 (RS)) as alternate function push pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
  GPIO_Init(GPIOF, &GPIO_InitStructure);

  /* Set PG.12(NE4 (LCD/CS)) as alternate function push pull - CE3(LCD /CS) */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
  GPIO_Init(GPIOG, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOE, &GPIO_InitStructure);
  GPIO_WriteBit(GPIOE, GPIO_Pin_2, Bit_RESET);
}

/************************************************************************************
* ��    �ƣ�void LCD_FSMCConfig(void)
* ��    �ܣ�LCD����FSMC��ʼ��
* ��ڲ������� 
* ���ڲ�������
* ˵    ����
* ���÷�����
************************************************************************************/
void LCD_FSMCConfig(void)
{
  FSMC_NORSRAMInitTypeDef  FSMC_NORSRAMInitStructure;
  FSMC_NORSRAMTimingInitTypeDef  p;

/*-- FSMC Configuration ------------------------------------------------------*/
/*----------------------- SRAM Bank 4 ----------------------------------------*/
  /* FSMC_Bank1_NORSRAM4 configuration */
  p.FSMC_AddressSetupTime = 0;
  p.FSMC_AddressHoldTime = 0;
  p.FSMC_DataSetupTime = 2;
  p.FSMC_BusTurnAroundDuration = 0;
  p.FSMC_CLKDivision = 0;
  p.FSMC_DataLatency = 0;
  p.FSMC_AccessMode = FSMC_AccessMode_A;

  /* Color LCD configuration ------------------------------------
     LCD configured as follow:
        - Data/Address MUX = Disable
        - Memory Type = SRAM
        - Data Width = 16bit
        - Write Operation = Enable
        - Extended Mode = Enable
        - Asynchronous Wait = Disable */
  FSMC_NORSRAMInitStructure.FSMC_Bank = FSMC_Bank1_NORSRAM4;
  FSMC_NORSRAMInitStructure.FSMC_DataAddressMux = FSMC_DataAddressMux_Disable;
  FSMC_NORSRAMInitStructure.FSMC_MemoryType = FSMC_MemoryType_SRAM;
  FSMC_NORSRAMInitStructure.FSMC_MemoryDataWidth = FSMC_MemoryDataWidth_16b;
  FSMC_NORSRAMInitStructure.FSMC_BurstAccessMode = FSMC_BurstAccessMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalPolarity = FSMC_WaitSignalPolarity_Low;
  FSMC_NORSRAMInitStructure.FSMC_WrapMode = FSMC_WrapMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignalActive = FSMC_WaitSignalActive_BeforeWaitState;
  FSMC_NORSRAMInitStructure.FSMC_WriteOperation = FSMC_WriteOperation_Enable;
  FSMC_NORSRAMInitStructure.FSMC_WaitSignal = FSMC_WaitSignal_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ExtendedMode = FSMC_ExtendedMode_Disable;
  FSMC_NORSRAMInitStructure.FSMC_WriteBurst = FSMC_WriteBurst_Disable;
  FSMC_NORSRAMInitStructure.FSMC_ReadWriteTimingStruct = &p;
  FSMC_NORSRAMInitStructure.FSMC_WriteTimingStruct = &p;

  FSMC_NORSRAMInit(&FSMC_NORSRAMInitStructure);  

  /* BANK 4 (of NOR/SRAM Bank 1~4) is enabled */
  FSMC_NORSRAMCmd(FSMC_Bank1_NORSRAM4, ENABLE);
}
/****************************************************************************
* ��    �ƣ�u16 ili9320_GetPoint(u16 x,u16 y)
* ��    �ܣ���ȡָ���������ɫֵ
* ��ڲ�����x      ������
*           y      ������
* ���ڲ�������ǰ������ɫֵ
* ˵    ����
* ���÷�����i=ili9320_GetPoint(10,10);
****************************************************************************/
u16 LCD_GetPoint(u16 x,u16 y)
{
  u16 temp;
  LCD_SetCursor(x,y);
  LCD_WriteRAM_Prepare();

   temp = LCD_ReadData();
   temp = LCD_ReadData();
  return (temp);
//  return (ili9320_BGR2RGB(LCD_ReadData()));
}
