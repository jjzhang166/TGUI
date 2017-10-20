#include "TGUIConfig/tgui_drv_conf.h"
volatile uint16 TouchX = 65535;
volatile uint16 TouchY = 65535;
volatile uint8 TouchUp = 0;//δ������

void GUISetPoint(uint16 x,uint16 y)
{
	TouchX = x;
	TouchY = y;
}

void GUIGetPoint(uint16* x,uint16* y)
{
	*x = TouchX;
	*y = TouchY;
}

void GUITouchUp(int16 xid,int16 yid)
{
	//�ͷź��� ��ʱ������ ֻ�ǳ���һ����־λ
	TouchUp = 1 ;
}

void GUITouchDown(uint16 x,uint16 y)
{
	TouchX = x;
	TouchY = y;
}
