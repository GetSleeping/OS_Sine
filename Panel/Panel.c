/*
******************************************************************
**                      uCGUIBuilder                            **
**                  Version:   4.0.0.0                          **
**                     2012 / 04                               **
**                   CpoyRight to: wyl                          **
**              Email:ucguibuilder@163.com                        **
**          This text was Created by uCGUIBuilder               **
******************************************************************/
#include "ucos_ii.h"
#include <stddef.h>
#include "GUI.h"
#include "DIALOG.h"
#include "WM.h"
#include "BUTTON.h"
#include "CHECKBOX.h"
#include "DROPDOWN.h"
#include "EDIT.h"
#include "FRAMEWIN.h"
#include "LISTBOX.h"
#include "MULTIEDIT.h"
#include "RADIO.h"
#include "SLIDER.h"
#include "TEXT.h"
#include "PROGBAR.h"
#include "SCROLLBAR.h"
#include "LISTVIEW.h"

//EventsFunctionList
void OnDropDownSelChanged(WM_MESSAGE * pMsg);
//EndofEventsFunctionList


/*********************************************************************
*
*       static data
*
**********************************************************************
*/

int hwin,hGUI_ID_DROPDOWN0,hTEXT,hMULTIEDIT;  //����ȫ�ֲ���

char text0[400] = {"A Good Boy Little Robert asked his mother for two cents.\n'What did you do with the money I gave you yesterday?''I gave it to a poor old woman,' he answered.'You��re a good boy,' said the mother proudly. 'Here are two cents more. But why are you so interested in the old woman?' 'She is the one who sells the candy."};
char readme[400] = {"if I Am a Manager One day in class, the teacher assigned his students to write a composition �C if I Am a Manager. All the students began to write except a boy. The teacher went to him and asked the reason. I am waiting for my secretary,�� was the boy��s answer."};
char stm32f1[500] = {"ST��s STM32 F1 series of mainstream MCUs covers the needs of a large variety of applications in the industrial, medical and consumer markets. With this series, ST has pioneered the world of ARM? Cortex?-M microcontrollers and set a milestone in the history of embedded applications. High performance with first-class peripherals and low-power, low-voltage operation is paired with a high level of integration at accessible prices with a simple architecture and easy-to-use tools."};
char mytest[400] = {"kiss my ass!"};
/*********************************************************************
*
*       Dialog resource
*
* This table conatins the info required to create the dialog.
* It has been created by ucGUIbuilder.
*/

static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {      //�����Ի���ؼ�����
    { FRAMEWIN_CreateIndirect,  "Sine System",   0,                       0,  0,  240,320,FRAMEWIN_CF_MOVEABLE,0},
    { DROPDOWN_CreateIndirect,   NULL,               GUI_ID_DROPDOWN0,        0, 0, 170,30, 0,0},
	{ DROPDOWN_CreateIndirect,   NULL,               GUI_ID_DROPDOWN1,        0, 20, 170,30, 0,0},
    //{ TEXT_CreateIndirect,      "Frequence:",         GUI_ID_TEXT0,            74, 46, 118,24, 0,0},
    { MULTIEDIT_CreateIndirect, "MULTIEDIT",         GUI_ID_MULTIEDIT0,       0, 55, 230,240,0,0}
};



/*****************************************************************
**      FunctionName:void PaintDialog(WM_MESSAGE * pMsg)
**      Function: to initialize the Dialog items
**                                                      
**      call this function in _cbCallback --> WM_PAINT
*****************************************************************/

void PaintDialog(WM_MESSAGE * pMsg)
{
    WM_HWIN hWin = pMsg->hWin;
	hWin = hWin;
}

/*****************************************************************
**      FunctionName:void InitDialog(WM_MESSAGE * pMsg)
**      Function: to initialize the Dialog items
**                                                      
**      call this function in _cbCallback --> WM_INIT_DIALOG
*****************************************************************/
void InitDialog(WM_MESSAGE * pMsg)                           //�Ի���ؼ���ʼ��
{
    WM_HWIN hWin = pMsg->hWin;
    //
    //FRAMEWIN
    //
    FRAMEWIN_SetClientColor(hWin,0xffc0c0);
	//FRAMEWIN_SetClientColor(hWin,0x0);
    FRAMEWIN_SetTextColor(hWin,0x0000ff);
    FRAMEWIN_SetFont(hWin,&GUI_Font16_ASCII);
    FRAMEWIN_SetTextAlign(hWin,GUI_TA_VCENTER|GUI_TA_CENTER);
    FRAMEWIN_AddCloseButton(hWin, FRAMEWIN_BUTTON_RIGHT, 0);
    //
    //GUI_ID_DROPDOWN0
    //
    DROPDOWN_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_DROPDOWN0),0,0xffffff);
    DROPDOWN_SetFont(WM_GetDialogItem(hWin,GUI_ID_DROPDOWN0),&GUI_Font16_ASCII);
    DROPDOWN_SetAutoScroll(WM_GetDialogItem(hWin,GUI_ID_DROPDOWN0),1);
    DROPDOWN_AddString(WM_GetDialogItem(hWin,GUI_ID_DROPDOWN0),"Frequence:");

    DROPDOWN_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_DROPDOWN1),0,0xffffff);
    DROPDOWN_SetFont(WM_GetDialogItem(hWin,GUI_ID_DROPDOWN1),&GUI_Font16_ASCII);
    DROPDOWN_SetAutoScroll(WM_GetDialogItem(hWin,GUI_ID_DROPDOWN1),1);
    DROPDOWN_AddString(WM_GetDialogItem(hWin,GUI_ID_DROPDOWN1),"Amplitude:");	
    //
    //GUI_ID_TEXT0
    //
    TEXT_SetBkColor(WM_GetDialogItem(hWin,GUI_ID_TEXT0),0xffc0c0);
    TEXT_SetFont(WM_GetDialogItem(hWin,GUI_ID_TEXT0),&GUI_Font16_ASCII);
    //
    //GUI_ID_MULTIEDIT0
    //
   /* MULTIEDIT_SetFont(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0),&GUI_Font16_ASCII); //�����ı��༭�������С
    MULTIEDIT_SetAutoScrollH(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0),1); //����ˮƽ������
    MULTIEDIT_SetAutoScrollV(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0),1); //���ô�ֱ������
    MULTIEDIT_SetText(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0),mytest);//�����ı��༭���ַ�
    MULTIEDIT_SetReadOnly(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0),0);//����Ϊֻ��
    MULTIEDIT_SetWrapChar(WM_GetDialogItem(hWin,GUI_ID_MULTIEDIT0));  //������ˮƽ������ �Զ�����
    */
}

/*********************************************************************
*
*       Dialog callback routine
*/
static void _cbCallback(WM_MESSAGE * pMsg) 
{
    int NCode, Id;
    WM_HWIN hWin = pMsg->hWin;
    switch (pMsg->MsgId) 
    {
        case WM_PAINT:
            PaintDialog(pMsg);
            break;
        case WM_INIT_DIALOG:
            InitDialog(pMsg);
            break;
        case WM_KEY:
            switch (((WM_KEY_INFO*)(pMsg->Data.p))->Key) 
            {
                case GUI_KEY_ESCAPE:
                    GUI_EndDialog(hWin, 1);
                    break;
                case GUI_KEY_ENTER:
                    GUI_EndDialog(hWin, 0);
                    break;
            }
            break;
        case WM_NOTIFY_PARENT:
            Id = WM_GetId(pMsg->hWinSrc); 
            NCode = pMsg->Data.v;        
            switch (Id) 
            {
                case GUI_ID_OK:
                    if(NCode==WM_NOTIFICATION_RELEASED)
                        GUI_EndDialog(hWin, 0);
                    break;
                case GUI_ID_CANCEL:
                    if(NCode==WM_NOTIFICATION_RELEASED)
                        GUI_EndDialog(hWin, 0);
                    break;
                case GUI_ID_DROPDOWN0:
                    switch(NCode)
                    {
                        case WM_NOTIFICATION_SEL_CHANGED:
                            OnDropDownSelChanged(pMsg);
                            break;
                    }
                    break;

            }
            break;
        default:
            WM_DefaultProc(pMsg);
    }
}


/*********************************************************************
*
*       Panel
*
**********************************************************************
*/
void Panel(void) 
{ 
    WM_SetDesktopColor(GUI_WHITE);      /* Automacally update desktop window */
    WM_SetCreateFlags(WM_CF_MEMDEV);  /* Use memory devices on all windows to avoid flicker */
    hwin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), &_cbCallback, 0, 0, 0);

    hGUI_ID_DROPDOWN0 = WM_GetDialogItem(hwin,GUI_ID_DROPDOWN0);   //�����������
    hTEXT = WM_GetDialogItem(hwin,GUI_ID_TEXT0);                   //����ı����
    hMULTIEDIT = WM_GetDialogItem(hwin,GUI_ID_MULTIEDIT0);         //����ı��༭����

    while(1)
    {
        OSTimeDly(1000);      //��ʱ
    }
}
/********************************************************************************************
*�������ƣ�void OnDropDownSelChanged(WM_MESSAGE * pMsg)
*
*��ڲ�����void *p_arg��������ָ��
*
*���ڲ�������
*
*����˵������������ı�ʱ����Ӧ���� 
*******************************************************************************************/
void OnDropDownSelChanged(WM_MESSAGE * pMsg)
{
    int i;

    i = DROPDOWN_GetSel(hGUI_ID_DROPDOWN0);      //��õ�ǰ�������ֵ

    switch(i)
    {
        case 0:
        {
            TEXT_SetText(hTEXT,"text0.txt");    //�޸��ı�����
            MULTIEDIT_SetText(hMULTIEDIT,text0);//�޸��ı��༭������
            break;
        }
        case 1:
        {
            TEXT_SetText(hTEXT,"readme.txt");
            MULTIEDIT_SetText(hMULTIEDIT,readme);
            break;
        }
        case 2:
        {
            TEXT_SetText(hTEXT,"STM32f1.txt");
            MULTIEDIT_SetText(hMULTIEDIT,stm32f1);
            break;
        }
        default:
            break;

    }
}
