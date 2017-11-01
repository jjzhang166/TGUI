#include "winClass.hpp"

staticFrameWin::staticFrameWin(
			uint16_t winXpos,
			uint16_t winYpos,
			uint16_t winWidth,
			uint16_t winHigh,
			char* name,
			uint8_t wsStyle,
			rootWin* parent,
			xQueueHandle queue,
			uint32_t winColor
):controlWin(winXpos,winYpos,winWidth,winHigh,name,wsStyle,parent,queue)
{
	this->winColor = winColor;
}

staticFrameWin::~staticFrameWin()
{

}

//绘画 就自己 不同的窗口实现不同
void staticFrameWin::paintWin()
{
	LCD_SetColors(winColor,winColor);
	LCD_DrawFullRect(getAbsoluteX(),getAbsoluteY(),getWinWidth(),getWinHigh());
}

//激活控件--注册 中间会调用createWin（） 其他根据不同的窗口变化
void staticFrameWin::registerWin()
{
	this->createWin();
	this->setAbsoluteXY();
}

//注销控件  会调用destroy（）窗口 其他会根据不同窗口变化
void staticFrameWin::unregisterWin()
{
	this->destroyWin();
}

