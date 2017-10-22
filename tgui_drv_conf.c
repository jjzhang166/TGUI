#include "TGUIConfig/tgui_drv_conf.h"
volatile uint16_t TouchX = 65535;
volatile uint16_t TouchY = 65535;
volatile uint8_t TouchUp = 0;//δ������

void GUISetPoint(uint16_t x,uint16_t y)
{
	TouchX = x;
	TouchY = y;
}

void GUIGetPoint(uint16_t* x,uint16_t* y)
{
	*x = TouchX;
	*y = TouchY;
}

void GUITouchUp(int16_t xid,int16_t yid)
{
	//�ͷź��� ��ʱ������ ֻ�ǳ���һ����־λ
	//��ȡ�ͷŵ�  ����ͷžͽ����ͷ�
	TouchUp = 1 ;
}

void GUITouchDown(uint16_t x,uint16_t y)
{
	TouchX = x;
	TouchY = y;
}
