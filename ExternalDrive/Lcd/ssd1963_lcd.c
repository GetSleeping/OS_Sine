/****************************************Copyright (c)***************************************
**                                 http://www.openmcu.com
**--------------------------------------File Info--------------------------------------------
** File name:           lcd.c
** Last modified Date:  2012-08-13
** Last Version:        V1.00
** Descriptions:        3.2寸lcd屏驱动
**
**-------------------------------------------------------------------------------------------
** Created by:          FXL
** Created date:        2012-08-13
** Version:             V1.00
** Descriptions:        3.2寸LCD屏驱动
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
LCD/CS硬件连接：LCD/CS  CE4(NOR/SRAM Bank 4)
*****************************************************************/
#define LCD_BASE        ((u32)(0x60000000 | 0x0C000000))
#define LCD             ((LCD_TypeDef *) LCD_BASE)

//定义LCD的尺寸
#define LCD_W 240
#define LCD_H 320

/* 色值 全局变量 */
static  vu16 TextColor = 0x0000, BackColor = 0xFFFF;
/* 读取LCD的DeviceCode 全局变量 */
u16 DeviceCode;
/* 秒时钟延时函数声明 也可自己定义*/
//extern void Delay(u32 nCount);

/****************************************************************************
* 名    称：void LCD_SetPoint(u16 x,u16 y,u16 point)
* 功    能：在指定座标画点
* 入口参数：x      行座标
*           y      列座标
*           point  点的颜色
* 出口参数：无
* 说    明：
* 调用方法：ili9320_SetPoint(10,10,0x0fe0);
****************************************************************************/
void LCD_SetPoint(u16 x,u16 y,u16 point)
{
  if ( (x>240)||(y>360) ) return;

  LCD_SetCursor(x,y);
  LCD_WriteRAM_Prepare();
  LCD_WriteRAM(point);
}


/****************************************************************************
* 名    称：void LCD_PutChar(u16 x,u16 y,u8 c,u16 charColor,u16 bkColor)
* 功    能：在指定座标显示一个16x24点阵的ascii字符
* 入口参数：x          列座标
*           y          行座标
*           charColor  字符的颜色
*           bkColor    字符背景颜色
* 出口参数：无
* 说    明：显示范围限定为可显示的ascii码
* 调用方法：ili9320_PutChar(10,10,'a',0x0000,0xffff);
****************************************************************************/
void LCD_PutChar(u16 x,u16 y,u8 c,u16 charColor,u16 bkColor)
{
  u16 i=0;
  u16 j=0;
  
  u16 tmp_char=0;

  for (i=0;i<24;i++)
  {
    tmp_char=ASCII_Table[((c-0x20)*24)+i];	 //减去32的偏移，是因为字符表从空格开始的，参考字符表那的注释
    for (j=0;j<16;j++)
    {
//      if ( (tmp_char >> 15-j) & 0x01 == 0x01)	按照上面的显示，字符是倒过来的，到这里改过来就行了
	if ( (tmp_char >> j) & 0x01 == 0x01)
      {
        LCD_SetPoint(x+i,y+j,charColor); // 字符颜色
      }
      else
      {
        LCD_SetPoint(x+i,y+j,bkColor); // 背景颜色
      }
    }
  }
}

/****************************************************************************
* 名    称：u16 findHzIndex(u8 *hz)
* 功    能：在自定义汉字库在查找所要显示的汉字的位置
* 入口参数：hz ：指向汉字(机内码)的指针
* 出口参数：u16：汉字的位置
* 说    明：
* 调用方法：
****************************************************************************/
u16 findHzIndex(u8 *hz)                     /* 在自定义汉字库在查找所要显示 */
                                                      /* 的汉字的位置 */
{
    u16 i=0;
    
    FNT_GB16 *ptGb16 = (FNT_GB16 *)GBHZ_16;
    
    while(ptGb16[i].Index[0] > 0x80) {
        if ((*hz == ptGb16[i].Index[0]) && (*(hz+1) == ptGb16[i].Index[1])) 
		{
            return i;
        }
        i++;
        if(i > (sizeof((FNT_GB16 *)GBHZ_16) / sizeof(FNT_GB16) - 1))  /* 搜索下标约束 */
        {
		    break;
        }
    }
    return 0;
}

/****************************************************************************
* 名    称：u16 WriteOneHzChar(u8 *pucMsk,u16 x0,u16 y0,u16 color)
* 功    能：显示一个指定大小的汉字
* 入口参数：u8 *pucMsk：当前字库地址
*			u16 x0    ：X轴坐标点
*		    u16 y0    ：Y轴坐标点
*			u16 color : 字体颜色
* 出口参数：u16：16位列宽
* 说    明：
* 调用方法：
****************************************************************************/
u16 WriteOneHzChar(u8 *pucMsk,
                               u16 x0,
                               u16 y0,
                               u16 color)
{
    u16 i,j;
    u16 mod[16];                                      /* 当前字模                     */
    u16 *pusMsk;                                      /* 当前字库地址                 */
    u16 y;
    
    pusMsk = (u16 *)pucMsk;
    for(i=0; i<16; i++)                                    /* 保存当前汉字点阵式字模       */
    {
        mod[i] = *pusMsk++;                                /* 取得当前字模，半字对齐访问   */
        mod[i] = ((mod[i] & 0xff00) >> 8) | ((mod[i] & 0x00ff) << 8);/* 字模交换高低字节（为了显示   */
                                                           /* 需要）                       */
    }
    y = y0;
    for(i=0; i<16; i++)                                    /* 16行                         */
    { 
	    #ifdef __DISPLAY_BUFFER                            /* 使用显存显示                 */
        for(j=0; j<16; j++)                                /* 16列                         */
        {
		    if((mod[i] << j)& 0x8000)                      /* 显示字模                     */
            {
			    DispBuf[240*(y0+i) + x0+j] = color;
            }
        }
        #else                                              /* 直接显示                     */
        
        LCD_SetCursor(x0, y);                              /* 设置写数据地址指针           */
		LCD_WriteRAM_Prepare();        					   /*开始写入GRAM	*/   
        for(j=0; j<16; j++)                                /* 16列                         */
        {
		    if((mod[i] << j) & 0x8000)                     /* 显示字模                     */
            { 
			    LCD_WriteRAM(color);
            } 
			else 
			{
                LCD_WriteRAM(White);                     /* 用读方式跳过写空白点的像素   */
            	//LCD_ReadDat();
			}
        }
        y++;
        #endif
    }
    return (16);                                          /* 返回16位列宽                 */
}

/****************************************************************************
* 名    称：u16 WriteOneASCII(u8 *pucMsk,u16 x0,u16 y0,u16 color)
* 功    能：显示一个指定大小的字符
* 入口参数：u8 *pucMsk：当前字库地址
*			u16 x0    ：X轴坐标点
*		    u16 y0    ：Y轴坐标点
*			u16 color : 字体颜色
* 出口参数：u16：16位列宽
* 说    明：
* 调用方法：
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
    for(i=0; i<16; i++) {                                 /* 16行                         */
        ucChar = *pucMsk++;
        #ifdef __DISPLAY_BUFFER                           /* 使用显存显示                 */
        for(j=0; j<8; j++) {                              /* 8列                          */
            if((ucChar << j)& 0x80) {                     /* 显示字模                     */
                DispBuf[240*(y0+i) + x0+j] = color;
            }
        }
        #else                                             /* 直接显示                     */
        
        LCD_SetCursor(x0, y);                             /* 设置写数据地址指针           */
		LCD_WriteRAM_Prepare();        					  /* 开始写入GRAM	    		  */
        for(j=0; j<8; j++) {                              /* 8列                          */
            if((ucChar << j) & 0x80) {                    /* 显示字模                     */
                LCD_WriteRAM(color);
            } else {
                LCD_WriteRAM(White);
				  //LCD_ReadDat();
            }
        }
        y++;
        #endif
    }
    return (8);                                           /* 返回16位列宽                 */
} 
/****************************************************************************
* 名    称：void WriteString(u16 x0, u16 y0,u8 *pcStr, u16 color)
* 功    能：在指定位置开始显示一个字符串和一串汉字 支持自动换行
* 入口参数：u16 x0    ：X轴坐标点
*		    u16 y0    ：Y轴坐标点
*			u8 *pcStr : 字地址
*			u16 color : 字体颜色
* 出口参数：无
* 说    明：
* 调用方法：
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
            break;                                     /* 字符串结束            */
        }      
        x0 = x0 + (usWidth);                           /* 调节字符串显示松紧度         */
        if(*pcStr > 0x80)                              /* 判断为汉字                   */
        {
		    if((x0 + 16) > LCD_W)                      /* 检查剩余空间是否足够         */
            {
			    x0 = 0;
                y0 = y0 + 16;                          /* 改变显示坐标                 */
                if(y0 > LCD_H)                         /* 纵坐标超出                   */
                {
				    y0 = 0;
                }
            }
            usIndex = findHzIndex(pcStr);
            usWidth = WriteOneHzChar((u8 *)&(ptGb16[usIndex].Msk[0]), x0, y0, color);
                                                       /* 显示字符                     */
            pcStr += 2;
        }
		else 
		{                                               /* 判断为非汉字                 */
            if (*pcStr == '\r')                         /* 换行                         */
            { 
			    y0 = y0 + 16;                           /* 改变显示坐标                 */
                if(y0 > LCD_H)                          /* 纵坐标超出                   */
                {
				    y0 = 0;
                }
                pcStr++;
                usWidth = 0;
                continue;
            } 
			else if (*pcStr == '\n')                    /* 对齐到起点                   */
            {
			    x0 = 0;
                pcStr++;
                usWidth = 0;
                continue;
            } 
			else 
			{
                if((x0 + 8) > LCD_W)                     /* 检查剩余空间是否足够         */
                {
				    x0 = 0;
                    y0 = y0 + 16;                        /* 改变显示坐标                 */
                    if(y0 > LCD_H)                       /* 纵坐标超出                   */
                    { 
					    y0 = 0;
                    }
                }
                usWidth = WriteOneASCII((u8 *)&ASCII_1608[(*pcStr - 0x20)][0], x0, y0, color);
                                                         /* ASCII码表21H的值对应区位码3区*/
                pcStr += 1;
            }
		}
	}												  	  
}

/****************************************************************************
* 名    称：void LCD_DisplayStringLine(u8 Line, u8 *ptr, u16 charColor, u16 bkColor)
* 功    能：显示最多40个字符一行在LCD上
* 入口参数：Line 行数 *ptr指向字符串的指针 charColor字符颜色 bkColor背景颜色
* 出口参数：无
* 说    明：
* 调用方法：LCD_DisplayStringLine(Line0,"I Love you...",White,Blue);  
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
* 名    称：void Draw_Circle(u8 x0,u16 y0,u8 r,u16 color)
* 功    能：在指定位置画一个指定大小的圆
* 入口参数：(x,y):中心点 
*       	 r   :半径
*           u16 color:颜色
* 出口参数：无
* 说    明：在指定位置画一个指定大小的圆
* 调用方法：
****************************************************************************/
void Draw_Circle(u8 x0,u16 y0,u8 r,u16 color)
{
	int a,b;
	int di;
	a=0;b=r;	  
	di=3-(r<<1);             //判断下个点位置的标志
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
		//使用Bresenham算法画圆     
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
* 名    称：void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
* 功    能：在指定的两侧位置画一条线
* 入口参数：x1,y1:起点坐标度 
*       	x2,y2:终点坐标
*           u16 color:颜色 
* 出口参数：无
* 说    明：在指定位置画一个指定大小的圆
* 调用方法：
****************************************************************************/
void LCD_DrawLine(u16 x1, u16 y1, u16 x2, u16 y2,u16 color)
{
	u16 t; 
	int xerr=0,yerr=0,delta_x,delta_y,distance; 
	int incx,incy,uRow,uCol; 

	delta_x=x2-x1; 				//计算坐标增量 
	delta_y=y2-y1; 
	uRow=x1; 
	uCol=y1; 
	if(delta_x>0)incx=1; 		//设置单步方向 
	else if(delta_x==0)incx=0;	//垂直线 
	else {incx=-1;delta_x=-delta_x;} 
	if(delta_y>0)incy=1; 
	else if(delta_y==0)incy=0;	//水平线 
	else{incy=-1;delta_y=-delta_y;} 
	if( delta_x>delta_y)distance=delta_x; //选取基本增量坐标轴 
	else distance=delta_y; 
	for(t=0;t<=distance+1;t++ )	//画线输出 
	{  
		LCD_SetPoint(uRow,uCol,color);//画点 
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
* 名    称：u16 LCD_BGR2RGB(u16 c)
* 功    能：RRRRRGGGGGGBBBBB 改为 BBBBBGGGGGGRRRRR 格式
* 入口参数：c      BRG 颜色值
* 出口参数：RGB 颜色值
* 说    明：内部函数调用
* 调用方法：
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
* 名    称：void STM3210E_LCD_Init(void)
* 功    能：LCD屏初始化
* 入口参数：无
* 出口参数：无
* 说    明：LCD屏初始化
* 调用方法：STM3210E_LCD_Init();
****************************************************************************/
void STM3210E_LCD_Init(void)
{ 
/* LCD管脚初始化 --------------------------------------------*/
  LCD_CtrlLinesConfig();

/* FSMC初始化 -----------------------------------------------*/
  LCD_FSMCConfig();

  DeviceCode = LCD_ReadReg(0x0000); 
  if(DeviceCode != 0x8999)
      DeviceCode = 0x8999;

  if(DeviceCode==0x9325)			  
  {
//	  Delay(5); /* 延时 50 ms */
//	  Delay(5); /* 延时 50 ms */
	  
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
//		  Delay(5); /* 延时 50 ms */
		
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
	else if(DeviceCode==0x8999)	   //对应的驱动IC为SSD1289
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
* 名    称：void LCD_Clear(u16 Color)
* 功    能：LCD清屏
* 入口参数：u16 Color：清屏色值
* 出口参数：无
* 说    明：LCD清屏
* 调用方法：void LCD_Clear(Black)
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
* 名    称：void LCD_SetCursor(u8 Xpos, u16 Ypos)
* 功    能：设置光标位置
* 入口参数：u8 Xpos : X轴光标
*           u16 Ypos：Y轴光标
* 出口参数：无
* 说    明：设置光标位置
* 调用方法：LCD_SetCursor(0，0)
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
* 名    称：void LCD_SetDisplayWindow(u8 Xpos, u16 Ypos, u8 Height, u16 Width)
* 功    能：设置一个显示窗口
* 入口参数：u8 Xpos  : 显示窗口X轴
*           u16 Ypos ：显示窗口Y轴
*			u8 Height：显示窗口的长
*			u16 Width：显示窗口的宽
* 出口参数：无
* 说    明：设置一个显示窗口
* 调用方法：LCD_SetDisplayWindow(0,0,240,320)
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
* 名    称：void ShowOneChinese(u8 Number, u8 x, u16 y,u16 PenColor,u16 BackColor)
* 功    能：显示一个汉字（自取模汉字 数据在FONTS.h中）
* 入口参数：u8 Number    : 字模数组中的第几个汉字
*			u8 x         ：X轴坐标
*           u16 y        ：Y轴坐标
*			u16 PenColor ：字体色
*			u16 BackColor：背景色d
* 出口参数：无
* 说    明：显示一个汉字
* 调用方法：
************************************************************************************/
void ShowOneChinese(u8 Number, u8 x, u16 y,u16 PenColor,u16 BackColor)
{
    u8 i,j;
	/*  汉字为16*16  */
	u8 sizex = 16;
	u8 sizey = 16;	
	u8 *Chinese_buf = &Chinese_Table[Number * sizex * 2];
 
    for(i=0; i<sizex; i++)                        /* sizex行   */
    {                                              
        for(j=0; j<sizey; j++)                    /* sizey列   */
        {
		    if((Chinese_buf[i*2+j/8] << (j%8)) & 0x80)      /* 显示第i行 共16个点 */
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
* 名    称：void ShowOneChinese(u8 Number, u8 x, u16 y,u16 PenColor,u16 BackColor)
* 功    能：显示一个汉字（自取模汉字 数据在FONTS.h中）
* 入口参数：u8 Number    : 字模数组中的第几个汉字
*			u8 x         ：X轴坐标
*           u16 y        ：Y轴坐标
*			u16 PenColor ：字体色
*			u16 BackColor：背景色d
* 出口参数：无
* 说    明：设置一个显示窗口
* 调用方法：LCD_SetDisplayWindow(0,0,240,320)
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
* 名    称：void LCD_WriteBMP(u8 Xpos, u16 Ypos, u8 Height, u16 Width, u8 *bitmap)
* 功    能：绘图
* 入口参数：u8 Xpos   : 显示图的X轴
*           u16 Ypos  ：显示图的Y轴
*			u8 Height ：显示图的长
*			u16 Width ：显示图的宽
*			u8 *bitmap: 显示图数据指针
* 出口参数：无
* 说    明：显示一个图片
* 调用方法：
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
//	if(*bitmap_ptr == 0xffff)	 //为了显示透明
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
* 名    称：void LCD_DrawPicture(u16 StartX,u16 StartY,u16 EndX,u16 EndY,u16 *pic)
* 功    能：在指定座标范围显示一副图片
* 入口参数：StartX     行起始座标
*           StartY     列起始座标
*           EndX       行结束座标
*           EndY       列结束座标
            pic        图片头指针
* 出口参数：无
* 说    明：图片取模格式为水平扫描，16位颜色模式
* 调用方法：ili9320_DrawPicture(0,0,100,100,(u16*)demo);
****************************************************************************/
void LCD_DrawPicture(u16 StartX,u16 StartY,u16 Height,u16 Width,u8 *pic)
{
  u32  i;
  
  u16 *bitmap = (u16 *)pic;
  if(DeviceCode==0x8999)
  {
	//设置图片显示窗口大小
	LCD_WriteReg(0x44,StartX|(StartX+Width-1)<<8); //设置水平方向GRAM起始地址(低字节为起始地址，高字节为结束地址)
	LCD_WriteReg(0x45,StartY);				  	//设置窗口的Y方向的开始位置
	LCD_WriteReg(0x46,StartY+Height-1);			//设置窗口的Y方向的结束位置
  }
  else 
  {
  	LCD_WriteReg(R80, StartX);	   	   	//水平方向GRAM起始地址
  	LCD_WriteReg(R81, StartX+Width-1); 	//水平方向GRAM结束地址 
  	LCD_WriteReg(R82, StartY);		  	//垂直方向GRAM起始地址
  	LCD_WriteReg(R83, StartY+Height-1);  	//垂直方向GRAM结束地址
  }

  LCD_SetCursor(StartX,StartY);
  LCD_WriteRAM_Prepare();

  for (i=0;i<(Height*Width);i++)
  {
    LCD_WriteRAM(*bitmap++);
  }

  if(DeviceCode==0x8999)
  {
	//恢复窗口
	LCD_WriteReg(0x44,0|239<<8); 					//设置窗口的X方向开始位置和结束位置
	LCD_WriteReg(0x45,0);				  			//设置窗口的Y方向的开始位置
	LCD_WriteReg(0x46,319);							//设置窗口的Y方向的结束位置
  }
  else
  {
  	LCD_WriteReg(R80, 0);	   	   	//水平方向GRAM起始地址
  	LCD_WriteReg(R81, 0xef); 	//水平方向GRAM结束地址 
  	LCD_WriteReg(R82, 0);		  	//垂直方向GRAM起始地址
  	LCD_WriteReg(R83, 0x13f);  	//垂直方向GRAM结束地址
  }
}
//画点绘图
void LCD_DrawPicture_dot(u16 StartX,u16 StartY,u16 Height,u16 Width,u8 *pic)
{
//  u32  i;
  u16 i,j;
  
  u16 *bitmap = (u16 *)pic;
  if(DeviceCode==0x8999)
  {
	//设置图片显示窗口大小
	LCD_WriteReg(0x44,StartX|(StartX+Width-1)<<8); //设置水平方向GRAM起始地址(低字节为起始地址，高字节为结束地址)
	LCD_WriteReg(0x45,320 - StartY - Height - 1);				  	//设置窗口的Y方向的开始位置
	LCD_WriteReg(0x46,320 - StartY);			//设置窗口的Y方向的结束位置
  }
  else 
  {
  	LCD_WriteReg(R80, StartX);	   	   	//水平方向GRAM起始地址
  	LCD_WriteReg(R81, StartX+Width-1); 	//水平方向GRAM结束地址 
  	LCD_WriteReg(R82, StartY);		  	//垂直方向GRAM起始地址
  	LCD_WriteReg(R83, StartY+Height-1);  	//垂直方向GRAM结束地址
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
	//恢复窗口
	LCD_WriteReg(0x44,0|239<<8); 					//设置窗口的X方向开始位置和结束位置
	LCD_WriteReg(0x45,0);				  			//设置窗口的Y方向的开始位置
	LCD_WriteReg(0x46,319);							//设置窗口的Y方向的结束位置
  }
  else
  {
  	LCD_WriteReg(R80, 0);	   	   	//水平方向GRAM起始地址
  	LCD_WriteReg(R81, 0xef); 	//水平方向GRAM结束地址 
  	LCD_WriteReg(R82, 0);		  	//垂直方向GRAM起始地址
  	LCD_WriteReg(R83, 0x13f);  	//垂直方向GRAM结束地址
  }
}

/************************************************************************************
* 名    称：void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
* 功    能：寄存器 写值
* 入口参数：u8 LCD_Reg        : 寄存器
*           u16 LCD_RegValue  ：要写入的值
* 出口参数：无
* 说    明：寄存器 写值
* 调用方法：
************************************************************************************/
void LCD_WriteReg(u8 LCD_Reg, u16 LCD_RegValue)
{
  /* Write 16-bit Index, then Write Reg */
  LCD->LCD_REG = LCD_Reg;
  /* Write 16-bit Reg */
  LCD->LCD_RAM = LCD_RegValue;
}

/************************************************************************************
* 名    称：u16 LCD_ReadReg(u8 LCD_Reg)
* 功    能：读 寄存器 的值
* 入口参数：u8 LCD_Reg        : 寄存器
* 出口参数：要读 寄存器 的值
* 说    明：读 寄存器 的值
* 调用方法：
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
* 名    称：void LCD_WriteRAM_Prepare(void)
* 功    能：准备写 RAM
* 入口参数：无
* 出口参数：无
* 说    明：
* 调用方法：
************************************************************************************/
void LCD_WriteRAM_Prepare(void)
{
  LCD->LCD_REG = R34;
}

/************************************************************************************
* 名    称：void LCD_WriteRAM(u16 RGB_Code)
* 功    能：写LCD RAM
* 入口参数：u16 RGB_Code：颜色模式（5-6-5）
* 出口参数：无
* 说    明：
* 调用方法：
************************************************************************************/
void LCD_WriteRAM(u16 RGB_Code)
{
  /* Write 16-bit GRAM Reg */
  LCD->LCD_RAM = RGB_Code;
}

/************************************************************************************
* 名    称：u16 LCD_ReadRAM(void)
* 功    能：读LCD RAM
* 入口参数：无 
* 出口参数：u16 : LCD RAM值
* 说    明：
* 调用方法：
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
* 名    称：void LCD_PowerOn(void)
* 功    能：使能显示
* 入口参数：无 
* 出口参数：无
* 说    明：
* 调用方法：
************************************************************************************/
void LCD_DisplayOn(void)
{
  LCD_WriteReg(R7, 0x0173); /* 262K color and display ON */
}

/************************************************************************************
* 名    称：void LCD_PowerOn(void)
* 功    能：失能显示
* 入口参数：无 
* 出口参数：无
* 说    明：
* 调用方法：
************************************************************************************/
void LCD_DisplayOff(void)
{
  LCD_WriteReg(R7, 0x0); 
}

/************************************************************************************
* 名    称：void LCD_CtrlLinesConfig(void)
* 功    能：lcd屏IO口初始化(FSMC)
* 入口参数：无 
* 出口参数：无
* 说    明：
* 调用方法：
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
* 名    称：void LCD_FSMCConfig(void)
* 功    能：LCD屏的FSMC初始化
* 入口参数：无 
* 出口参数：无
* 说    明：
* 调用方法：
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
* 名    称：u16 ili9320_GetPoint(u16 x,u16 y)
* 功    能：获取指定座标的颜色值
* 入口参数：x      行座标
*           y      列座标
* 出口参数：当前座标颜色值
* 说    明：
* 调用方法：i=ili9320_GetPoint(10,10);
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
