#include "winClass.hpp"
static void itemWinProc(rootWin* rw , rootWin* fw , MsgType mt, uint32_t d1, uint32_t d2);

listBarWin::listBarWin(
			uint16_t winXpos,
			uint16_t winYpos,
			uint16_t winWidth,
			uint16_t winHigh,
			char* name,
			rootWin* parent,
			xQueueHandle queue
			):controlWin(winXpos,winYpos,winWidth,winHigh,name,parent,queue)
{
	this->itemHigh = winHigh; //保存当前的高 作为之后每一个item的高
	this->openStat = false;
	this->itemList = NULL;
}

listBarWin::~listBarWin()
{
	
}

//原理是一个按钮 一按 会出现很多按钮，每个按钮就是一个item
//多个item 是兄弟 都是 listBar 的孩子 
//改改 自己本身就是一个按钮

// |---------|>|
//       V
// |---------|V| 
// |item------1|
// |item------2|
// |item------3|
void listBarWin::itemInit(char**  itemList, uint8_t num)
{
	this->itemList = itemList;
	setRwNum(num);
	comboBoxInit();
	
	coverBufLen = getItemHigh()*getWinWidth()*getRwNum()*2;
	int i,max = getRwNum();
	for(i=0;i<max;i++)
	{
		//更具上一个进行 y ++；   ----getWinXpos ---还未改
		rootWin* temp = new buttonWin(0,(i+1)*getItemHigh(),getWinWidth(),getItemHigh(),itemList[i],this,getQueue());
		temp->setWinProc(itemWinProc);
		((controlWin*)temp)->setTextColor(getTextColor());
		((controlWin*)temp)->setBackColor(GREY1);
		//并存入相应的列表里
		getRwList()[i] = temp;
	}
}


//按钮失焦
void listBarWin::defocusListBar()	 	
{
	
}

//列表按下
void listBarWin::pressListBar() 	 	
{
	if(!isWinSelected())//之前未被选中 改为选中
	{
		changSelectedStat();
		LCD_SetColors(WHITE,WHITE);
		LCD_DrawFullRect(getAbsoluteX(),getAbsoluteY(),getWinWidth(),getItemHigh());
		LCD_SetColors(getTextColor(),getTextColor());
		LCD_DrawRect(getAbsoluteX(),getAbsoluteY(),getWinWidth(),getItemHigh());
		
		if(getWinName() != NULL)
		{
			uint16_t temp;
			uint8_t i = 0,num  = 0;
			sFONT f =getFont();
			while(getWinName()[num] != '\0' )
			{
				num++;
				if((getWinWidth()- getFont().Width*num) <= 0)
				{
					num--;
					break;
				}
			}
			uint16_t line = getAbsoluteY() + ((getItemHigh()- getFont().Height)/2);
			uint16_t column = getAbsoluteX() + ((getWinWidth()- getFont().Width*num)/2);
			LCD_SetFont(&f);
			LCD_SetColors(getTextColor(),WHITE);
			while(i < num )
			{
				temp = (uint16_t) getWinName()[i];
				LCD_DisplayChar(line,column,temp);
				column += getFont().Width;
				i++;
			}
		}
	}
}

//按钮释放
void listBarWin::releaseListBar()	 	
{
	if(isWinSelected())//若之前选中 改为未选中
	{
		paintAll();	
		changSelectedStat();
	}
}

void listBarWin::changeOpenList()
{
	if(getRwList() != NULL)
	{
		if(!isOpen())
		{ // 若未打开 -->  遍历列表 注册
			int i,max = getRwNum();
			for(i=0;i<max;i++)
			{
				getRwList()[i]->registerWin();
			}
			movtoFront(); //打开的话 就把这个列表推到最前 保证最新被访问
			setWinHigh(getItemHigh()*(max+1));
			saveToBuf();//把要被覆盖的数据先存入缓冲区
			changeOpenState();//改变打开状态
			paintAll();	
			printf("listHigh>>>>>>>>>>>>>>>%d",getWinHigh());
		}else
		{// 若打开 -->  遍历列表 注销
			printf("list is zx!\n");
			int i,max = getRwNum();
			for(i=0;i<max;i++)
			{
				getRwList()[i]->unregisterWin();
			}
			//这里应该从buffer中取出数据
			printf("listHigh>>>>>>>>>>>>>>>%d",getWinHigh());
			readFromBuf();
			setWinHigh(getItemHigh());
			printf("listHigh>>>>>>>>>>>>>>>%d",getWinHigh());
			//getParent()->paintAll();//重绘listBar的父亲窗口 因为打开时把父亲的给覆盖了
			changeOpenState();//改变打开状态
		}	
	}
}

void listBarWin::paintWin()
{
	LCD_SetColors(getBackColor(),getBackColor());
	LCD_DrawFullRect(getAbsoluteX(),getAbsoluteY(),getWinWidth(),getItemHigh());
	LCD_SetColors(getTextColor(),getTextColor());
	LCD_DrawRect(getAbsoluteX(),getAbsoluteY(),getWinWidth(),getItemHigh());
	if(getWinName() != NULL)
	{
		uint16_t temp;
		uint8_t i = 0,num  = 0;
		sFONT f =getFont();
		while(getWinName()[num] != '\0' )
		{
			num++;
			if((getWinWidth()- getFont().Width*num) <= 0)
			{
				num--;
				break;
			}
		}
		uint16_t line = getAbsoluteY() + ((getItemHigh()- getFont().Height)/2);
		uint16_t column = getAbsoluteX() + ((getWinWidth()- getFont().Width*num)/2);
		LCD_SetFont(&f);
		LCD_SetColors(getTextColor(),getBackColor());
		while(i < num )
		{
			temp = (uint16_t) getWinName()[i];
			LCD_DisplayChar(line,column,temp);
			column += getFont().Width;
			i++;
		}
	}
}

//激活控件--注册 中间会调用createWin（） 其他根据不同的窗口变化
void listBarWin::registerWin()
{
	rootWin::registerWin();
}

//注销控件  会调用destroy（）窗口 其他会根据不同窗口变化
void listBarWin::unregisterWin()
{
	rootWin::unregisterWin();
}

//摧毁控件
void listBarWin::destroyWin()
{
	delete itemList;
	comboBoxDestroy();
	rootWin::destroyWin();
}


//改变状态标志位
void listBarWin::changeOpenState()
{
	if(isOpen())
		{this->openStat = false;}
	else
		{this->openStat = true;}
}

//---------------------------------------

void listBarWin::saveToBuf()
{
	uint16_t i,j;
	uint16_t lineWidth = (getWinWidth()+1)*GUI_PIXELSIZE;//每行宽度
	uint16_t lineNum = getWinHigh()+1;//多少行
	uint16_t offset;
	uint32_t cP;
	cP	= (getAbsoluteX()+(getAbsoluteY())*GUI_WIDTH)*GUI_PIXELSIZE;//可能不是h衡向存储的
	uint16_t nextLine = GUI_WIDTH*GUI_PIXELSIZE;
	for(i=0;i<lineNum;i++)
	{	
		for(j=0;j<lineWidth;j++)
		{
			offset = j;
			win_buffer[cP+offset] = GUI_BUFADDR[cP+offset];
		}
			cP +=nextLine;
	}
}

void listBarWin::readFromBuf()
{
	uint16_t i,j;
	uint16_t lineWidth = (getWinWidth()+1)*GUI_PIXELSIZE;//每行宽度
	uint16_t lineNum = getWinHigh()+1;//多少行
	uint16_t offset;
	uint32_t cP;
	cP = (getAbsoluteX()+(getAbsoluteY())*GUI_WIDTH)*GUI_PIXELSIZE;//可能不是h衡向存储的
	uint16_t nextLine = GUI_WIDTH*GUI_PIXELSIZE;
	for(i=0;i<lineNum;i++)
	{
		for(j=0;j<lineWidth;j++)
		{
			offset = j;
			GUI_BUFADDR[cP+offset] = win_buffer[cP+offset];
		}
			cP +=nextLine;
	}
}


/*
//这里存储数据 
uint16_t x = ((listBarWin*)(rw->getParent()))->getRwList()[0]->getAbsoluteX();
uint16_t y = ((listBarWin*)(rw->getParent()))->getRwList()[0]->getAbsoluteY();
uint32_t start = ((x-1)*y+y)*GUI_PIXELSIZE;
uint8_t* temp = (uint8_t* )GUI_BUFADDR;
uint8_t offset = 0;
for(i=0;i < ((listBarWin*)(rw->getParent()))->getCoverBufLen();i++)
{
	offset = i % rw->getWinWidth();
	((listBarWin*)(rw->getParent()))->getCoverBuffer()[i] = temp[start+offset];
	if(i % rw->getWinWidth() == 0)
	{
		start += LCD_PIXEL_WIDTH ;
	}
}*/

/*//这里应该用导出数据
uint16_t x = ((listBarWin*)(rw->getParent()))->getRwList()[0]->getAbsoluteX();
uint16_t y = ((listBarWin*)(rw->getParent()))->getRwList()[0]->getAbsoluteY();
uint32_t start = ((x-1)*y+y)*GUI_PIXELSIZE;
uint8_t* temp = (uint8_t* )GUI_BUFADDR;
uint8_t offset = 0;
for(i=0;i < ((listBarWin*)(rw->getParent()))->getCoverBufLen();i++)
{
	offset = i % rw->getWinWidth();
	temp[start+offset] = ((listBarWin*)(rw->getParent()))->getCoverBuffer()[i] ;
	if(i % rw->getWinWidth() == 0)
	{
		start += LCD_PIXEL_WIDTH ;
	}
}
*/
				

static void itemWinProc(rootWin* rw , rootWin* fw , MsgType mt, uint32_t d1, uint32_t d2)
{
	printf("itemWIn\n");
	switch(mt)
	{
		case MSG_CLICK: 
		{
			if(rw->isInArea((uint16_t)d1,(uint16_t)d2))
			{
				((buttonWin*)rw)->pressButton();//显示被按下
				
			}else{
				((buttonWin*)rw)->releaseButton();//显示松开
			}
		}break;
		case MSG_RELEASECLICK:
		{
			((buttonWin*)rw)->releaseButton();//显示松开
			if(rw->isInArea(d1,d2))//若为空间内部松开-->执行应有的程序
			{
				//按下  存储数据
				message* msg = new message();
				msg->type = MSG_ITEM;
				msg->data1 = (uint32_t)rw->getWinName();//存储为uint32_t 到时需要强行转换为char*
				msg->data2 =  0;
				msg->destWin = rw->getParent();
				msg->fromWin = rw;
				rw->sendMSGtoBack(msg,rw->getQueue());
			}
		}break;
		default:break;
	}
}

