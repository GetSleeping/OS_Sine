
#include "stm32f10x.h"
#include "Picture_Move.h"
#include "lcd.h"

MovePicture_Info MovePicture_info;

void MovePicture_Init(u16 StartX,u16 StartY,u16 Height,u16 Width,u8 *pic)
{
    MovePicture_info.X_Star    = StartX;
	MovePicture_info.Y_Star    = StartY;
	MovePicture_info.X_Height  = Height;
	MovePicture_info.Y_Width   = Width;
	MovePicture_info.pic       = pic;  
}

//��ʾ����
void MovePicture(MovePicture_Info* picture)
{
    Refreshes_Screen();	  			//ˢ����Ļ
    picture->DrawPicture(picture->X_Star, picture->Y_Star, picture->X_Height, picture->Y_Width, picture->pic );
}

//����Ҫ�ƶ��Ĵ������Բ���
MovePicture_Info* Return_MovePicture_info(void)
{
    return &MovePicture_info;
}


