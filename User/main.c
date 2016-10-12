#include "stm32f10x.h"
#include "ucos_ii.h"
#include <stdio.h>
#include "Gpio_Led.h"
#include "gui.h"
#include "Systick.h"
#include "WM.h"
#include "BUTTON.h"
#include "Panel.h"
#include "DAC_Sine.h"

#define STARTUP_TASK_PRIO              1
#define STARTUP_TASK_STK_SIZE          80
#define TOUCHTASK_PRIO                 20
#define TOUCHTASK_STK_SIZE             1000
#define PANEL_TASK_PRIO                5
#define PANEL_TASK_STK_SIZE            5000

static void PanelTask(void *p_arg);
static void TouchTask(void *p_arg);

static OS_STK starup_task_stk[STARTUP_TASK_STK_SIZE];
static OS_STK paneltask_stk[PANEL_TASK_STK_SIZE];
static OS_STK touchtask_stk[TOUCHTASK_STK_SIZE];
/********************************************************************************************
*函数名称：static void startup_task(void *p_arg)
*
*入口参数：void *p_arg：空类型指针
*
*出口参数：无
*
*功能说明：启动心跳时钟 初始化CPU使用率 建立应用任务 
*******************************************************************************************/
static void startup_task(void *p_arg)
{
    systick_init();               //心跳时钟初始化
#if(OS_TASK_STAT_EN > 0)          //统计CPU使用率
    OSStatInit();
#endif
    
    OSTaskCreate(PanelTask,(void*)0,&paneltask_stk[PANEL_TASK_STK_SIZE - 1],PANEL_TASK_PRIO);
    OSTaskCreate(TouchTask,(void*)0,&touchtask_stk[TOUCHTASK_STK_SIZE - 1],TOUCHTASK_PRIO);

    OSTaskDel(OS_PRIO_SELF);       //自我任务删除
}
/********************************************************************************************
*函数名称：int main(void)
*
*入口参数：无
*
*出口参数：int
*
*功能说明：主函数 初始化UCOSII 建立一个启动任务
*******************************************************************************************/
int main(void)
{
	DAC_Mode_Init();
    OSInit();
    OSTaskCreate(startup_task,(void*)0,&starup_task_stk[STARTUP_TASK_STK_SIZE - 1],STARTUP_TASK_PRIO);
    OSStart();
    return 0;
}
/********************************************************************************************
*函数名称：static void PanelTask(void *p_arg)
*
*入口参数：void *p_arg：空类型指针
*
*出口参数：无
*
*功能说明：演示一个进度条
*******************************************************************************************/
static void PanelTask(void *p_arg)
{
    GUI_Init();			 
	GUI_SetBkColor(GUI_BLACK);   //设置背景颜色
	GUI_SetColor(GUI_RED);      //设置前景颜色，及字体和绘图的颜色
	GUI_Clear();                 //按指定颜色清屏

	GUI_DispStringAt("UCGUI OK!",10,10);        //显示字符
    GUI_SetFont(&GUI_Font4x6);
    GUI_DispStringAt("www.openmcu.com",10,100); //显示字符
    GUI_SetFont(&GUI_Font6x8);
    GUI_DispStringAt("www.openmcu.com",10,130); //显示字符
    GUI_SetFont(&GUI_Font8x16);
    GUI_DispStringAt("www.openmcu.com",10,160); //显示字符

	//GUI_DrawCircle(100,100,50);

    Panel();                             //显示面板
}
/********************************************************************************************
*函数名称：static void TouchTask(void *p_arg)
*
*入口参数：void *p_arg：空类型指针
*
*出口参数：无
*
*功能说明：触摸检测 界面绘制检测 
*******************************************************************************************/
static void TouchTask(void *p_arg)
{
	GUI_CURSOR_Show();      //打开“鼠标显示”

    for(;;)
    {
		GUI_TOUCH_Exec();   //触摸检测
		OSTimeDly(15);      //延时
	    GUI_Exec();         //界面绘制检测
		//GUI_DrawCircle(100,100,50);
    }
}

