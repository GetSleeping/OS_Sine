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
*�������ƣ�static void startup_task(void *p_arg)
*
*��ڲ�����void *p_arg��������ָ��
*
*���ڲ�������
*
*����˵������������ʱ�� ��ʼ��CPUʹ���� ����Ӧ������ 
*******************************************************************************************/
static void startup_task(void *p_arg)
{
    systick_init();               //����ʱ�ӳ�ʼ��
#if(OS_TASK_STAT_EN > 0)          //ͳ��CPUʹ����
    OSStatInit();
#endif
    
    OSTaskCreate(PanelTask,(void*)0,&paneltask_stk[PANEL_TASK_STK_SIZE - 1],PANEL_TASK_PRIO);
    OSTaskCreate(TouchTask,(void*)0,&touchtask_stk[TOUCHTASK_STK_SIZE - 1],TOUCHTASK_PRIO);

    OSTaskDel(OS_PRIO_SELF);       //��������ɾ��
}
/********************************************************************************************
*�������ƣ�int main(void)
*
*��ڲ�������
*
*���ڲ�����int
*
*����˵���������� ��ʼ��UCOSII ����һ����������
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
*�������ƣ�static void PanelTask(void *p_arg)
*
*��ڲ�����void *p_arg��������ָ��
*
*���ڲ�������
*
*����˵������ʾһ��������
*******************************************************************************************/
static void PanelTask(void *p_arg)
{
    GUI_Init();			 
	GUI_SetBkColor(GUI_BLACK);   //���ñ�����ɫ
	GUI_SetColor(GUI_RED);      //����ǰ����ɫ��������ͻ�ͼ����ɫ
	GUI_Clear();                 //��ָ����ɫ����

	GUI_DispStringAt("UCGUI OK!",10,10);        //��ʾ�ַ�
    GUI_SetFont(&GUI_Font4x6);
    GUI_DispStringAt("www.openmcu.com",10,100); //��ʾ�ַ�
    GUI_SetFont(&GUI_Font6x8);
    GUI_DispStringAt("www.openmcu.com",10,130); //��ʾ�ַ�
    GUI_SetFont(&GUI_Font8x16);
    GUI_DispStringAt("www.openmcu.com",10,160); //��ʾ�ַ�

	//GUI_DrawCircle(100,100,50);

    Panel();                             //��ʾ���
}
/********************************************************************************************
*�������ƣ�static void TouchTask(void *p_arg)
*
*��ڲ�����void *p_arg��������ָ��
*
*���ڲ�������
*
*����˵����������� ������Ƽ�� 
*******************************************************************************************/
static void TouchTask(void *p_arg)
{
	GUI_CURSOR_Show();      //�򿪡������ʾ��

    for(;;)
    {
		GUI_TOUCH_Exec();   //�������
		OSTimeDly(15);      //��ʱ
	    GUI_Exec();         //������Ƽ��
		//GUI_DrawCircle(100,100,50);
    }
}

