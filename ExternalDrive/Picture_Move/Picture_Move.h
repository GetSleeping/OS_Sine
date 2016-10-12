#ifndef _PICTURE_MOVE_H_
#define _PICTURE_MOVE_H_

typedef struct
{
    u16 X_Star;				   //图片的X起始位置
	u16 Y_Star;				   //图片的Y起始位置
	u16 X_Height;
	u16 Y_Width;
	u8  *pic;
    void(*DrawPicture)(u16 StartX,u16 StartY,u16 Height,u16 Width,u8 *pic);
}MovePicture_Info;

void MovePicture_Init(u16 StartX,u16 StartY,u16 Height,u16 Width,u8 *pic);
void MovePicture(MovePicture_Info* picture);
MovePicture_Info* Return_MovePicture_info(void);











#endif
