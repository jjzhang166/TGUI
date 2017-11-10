#include "winClass.hpp"

trackBarWin::trackBarWin(
		uint16_t winXpos,
		uint16_t winYpos,
		uint16_t winWidth,
		uint16_t winHigh,
		char* name,
		rootWin* parent,
		xQueueHandle queue,
		bool HorizorVert
	):controlWin(winXpos,winYpos,winWidth,winHigh,name,parent,queue)
{
	this->HorizorVert = HorizorVert;
	uint32_t tx =  getAbsoluteX();
	sliderStat  = (tx << 16);
	sliderStat +=  getAbsoluteY();
	if(HorizorVert)
		{sliderWidth = getWinHigh()*3/2;}
	else
		{sliderWidth = getWinWidth()*3/2;}
	sliderValue = 0;
}

trackBarWin::~trackBarWin()
{

}

//先画个背景在 再上面画个button作为滑块
void trackBarWin::paintWin()
{
	//背景
	LCD_SetColors(GREY1,GREY1);
	LCD_DrawFullRect(getAbsoluteX(),getAbsoluteY(),getWinWidth(),getWinHigh());
	LCD_SetColors(getTextColor(),getTextColor());
	paintSlider((uint16_t)(sliderStat>>16),(uint16_t)sliderStat);
	sliderSliding((uint16_t)(sliderStat>>16),(uint16_t)sliderStat);
} 	


//滑块滑动 只需pos
void trackBarWin::sliderSliding(uint16_t xpos,uint16_t ypos)
{
	if(xpos != (uint16_t)(sliderStat>>16) && ypos != (uint16_t)sliderStat)//如果相同就不进行绘画
	{
		//保存当前x,y值
		uint32_t tx =  xpos;
		sliderStat  = (tx << 16);
		sliderStat += ypos;
		//背景
		LCD_SetColors(GREY1,GREY1);
		LCD_DrawFullRect(getAbsoluteX(),getAbsoluteY(),getWinWidth(),getWinHigh());
		LCD_SetColors(WHITE,WHITE);
		paintSlider(xpos,ypos);
	}
}

//释放
void trackBarWin::releaseSlider()
{
	LCD_SetColors(getTextColor(),getTextColor());
	paintSlider((uint16_t)(sliderStat>>16),((uint16_t)sliderStat));
}

//根据现在的位置求 百分比
void trackBarWin::generateValue()
{
	if(HorizorVert)
	{
		int16_t t = getWinWidth() - sliderWidth;
		int16_t offset = (sliderStat >> 16) - getAbsoluteX() - sliderWidth/2;
		if(offset < 0) offset = 0;//有点不精确
		if(offset > t) offset = t;
		sliderValue =100 * offset / t ;//求百分比
	}else{
		int16_t t = getWinHigh() - sliderWidth;
		int16_t offset = ((uint16_t)sliderStat) - getAbsoluteY() - sliderWidth/2;
		if(offset < 0) offset = 0;
		if(offset > t) offset = t;
		sliderValue = 100 * offset / t ;//求百分比
	}
}
void trackBarWin::registerWin()
{
	rootWin::registerWin();
}	
void trackBarWin::unregisterWin()
{
	rootWin::unregisterWin();
}
void trackBarWin::destroyWin()
{
	rootWin::destroyWin();
}

void trackBarWin::paintSlider(uint16_t xpos,uint16_t ypos)
{
	
		uint16_t x,y,w,h;
		if(HorizorVert)//水平
		{
				w = sliderWidth;
				x = xpos-sliderWidth/2;
				y = getAbsoluteY();
				h = getWinHigh();
				if(x < getAbsoluteX())
				{x = getAbsoluteX();}
				else if(x > (getAbsoluteX()+getWinWidth()-w))
				{x = getAbsoluteX()+getWinWidth()-w;}
		}
		else//垂直
		{
			x = getAbsoluteX();
			y = ypos-sliderWidth/2;
			w = getWinWidth();
			h = sliderWidth;
			if(y < getAbsoluteY())
			{y = getAbsoluteY();}
			else if(y> (getAbsoluteY()+getWinHigh()-h))
			{y = getAbsoluteY()+getWinHigh()-h;}
		}
		LCD_DrawFullRect(x,y,w,h);
}





