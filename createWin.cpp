#include "createWin.hpp"

/*
问题1：缓冲区多重叠加不适用
问题2：重绘会覆盖最上面的图案
*/

static void win0(void *pvParameters);


void GUI_Run()
{
	xTaskCreate( win0, "win0", 1000, NULL, 1, NULL);
	vTaskStartScheduler();		// 启动调度器，任务开始执行	
}



//用孩子兄弟二叉树的第一个孩子作为当前界面的

xQueueHandle queue =  xQueueCreate( MSG_QUENUM, sizeof( message));
rootWin* curClickWin;  
virKeyboardWin* vkWin;

static void ClickHandle(uint16_t x , uint16_t y ,rootWin* mw,uint8_t* stat);
static void winDefaultProc(rootWin* rw,rootWin* fw, MsgType mt, uint32_t d1, uint32_t d2);
static void winWeakProc0(rootWin* rw,rootWin* fw, MsgType mt, uint32_t d1, uint32_t d2);
static void winWeakProc1(rootWin* rw,rootWin* fw, MsgType mt, uint32_t d1, uint32_t d2);
static void winWeakProc2(rootWin* rw,rootWin* fw, MsgType mt, uint32_t d1, uint32_t d2);
static void winWeakProc3(rootWin* rw,rootWin* fw, MsgType mt, uint32_t d1, uint32_t d2);
static void winWeakProc4(rootWin* rw , rootWin* fw , MsgType mt, uint32_t d1, uint32_t d2);
static void winWeakProc5(rootWin* rw , rootWin* fw , MsgType mt, uint32_t d1, uint32_t d2);
static void winWeakProc61(rootWin* rw , rootWin* fw , MsgType mt, uint32_t d1, uint32_t d2);
static void winWeakProc6(rootWin* rw , rootWin* fw , MsgType mt, uint32_t d1, uint32_t d2);
static void winWeakProc7(rootWin* rw , rootWin* fw , MsgType mt, uint32_t d1, uint32_t d2);
static void winWeakProc71(rootWin* rw , rootWin* fw , MsgType mt, uint32_t d1, uint32_t d2);
rootWin* root1;
progressBarWin* tempWin;

static void win0(void *pvParameters)
{
	//----------------这里开始配置窗口-------------------
	char* item[] = {"item1","item2","item3"};
	char* o1[] = {"option1","option2","option3"};
	rootWin* root = new mainWin(0,0,GUI_WIDTH,GUI_HIGH,"ROOT!",NULL,queue,WS_DEFAULT);
	//mainWin
	mainWin* mw = new mainWin(0,0,GUI_WIDTH,GUI_HIGH,"IT IS TEST WIN 1!",root,queue,WS_DEFAULT);
	//------注册&打印-------
	mw->registerWin();
	mw->setWinProc(winWeakProc0);
	rootWin* sf1 = mw->getBackWin();
	root1 =mw;
	
	listBarWin* lbw1 = new listBarWin(50,50,180,30,"listBar",sf1,queue);
	lbw1->setTextColor(BLACK);
	lbw1->setBackColor(GREEN);
	lbw1->setWinProc(winWeakProc5);
	lbw1->itemInit(item,3);
	lbw1->registerWin();
	
	rootWin* bt3 = new buttonWin(50,100,180,100,"button",sf1,queue);
	((buttonWin*)bt3)->setTextColor(BLACK);
	((buttonWin*)bt3)->setBackColor(CYAN);
	bt3->registerWin();
	bt3->setWinProc(winWeakProc4);
	
	radioBtnWin* rbw1 = new radioBtnWin(50,220,180,110,NULL,sf1,queue);
	rbw1->radioBtnInit(o1,3,false);
	rbw1->registerWin();
	rbw1->setWinProc(winWeakProc6);
	
	//radioBtnWin* rbw2 = new radioBtnWin(250,185,400,30,NULL,sf1,queue);
	//rbw2->radioBtnInit(o1,3,true);
	//rbw2->registerWin();
	//rbw2->setWinProc(winWeakProc6);
	
	optionWin* opw1 = new optionWin(50,350,180,30,"check1",sf1,queue,false);
	opw1->registerWin();
	opw1->setWinProc(winWeakProc61);
	
	optionWin* opw2 = new optionWin(50,400,180,30,"check2",sf1,queue,false);
	opw2->registerWin();
	opw2->setWinProc(winWeakProc61);
	
	trackTextWin* tbw1 = new trackTextWin(280,50,380,30,NULL,sf1,queue);
	tbw1->trackTextInit();
	tbw1->registerWin();
	tbw1->setWinProc(winWeakProc71);
	
	rootWin* tbw2 = new trackBarWin(750,30,30,400,NULL,sf1,queue,false);
	tbw2->registerWin();
	tbw2->setWinProc(winWeakProc7);
	
	progressBarWin* pbw1 = new progressBarWin(280,110,300,30,NULL,sf1,queue);
	pbw1->registerWin();
	tempWin = (progressBarWin*)pbw1;
	
	virKeyboardWin* kbw = new virKeyboardWin(0,244,400,200,NULL,sf1,queue);
	kbw->setTextColor(BLACK);
	kbw->setBackColor(GREY2);
	kbw->keyBoardInit();
	kbw->setWinProc(winWeakProc1);
//	kbw->registerWin();
	vkWin = kbw;
 
	textBarWin* tbw = new textBarWin(250,160,400,90,NULL,sf1,queue);
	tbw->setTextColor(BLACK);
	tbw->setBackColor(YELLOW);
	tbw->setWinProc(winWeakProc2);
	tbw->registerWin();
	
	mw->paintAll();

	//------------------------窗口介绍------------------------
	
	//----------------------信息处理函数----------------------
	uint16_t x,y;
	rootWin* rw;
	uint8_t thouchStat = 0;//确定是否按下 以及脱手
	
	message* buffer; 
	buffer = (message*)pvPortMalloc(sizeof(message));
	printf("message = %d\n",sizeof( message));
	while(1)//获取msg
	{
		ClickHandle(x,y,mw,&thouchStat); //点击处理函数
		portBASE_TYPE xStatus = xQueueReceive(queue,buffer,0); //获取信息
		if(xStatus == pdPASS )
		{
			printf("-------have msg \ntype = %d\nid = %d\n",buffer->type,buffer->destWin->getWinID());
			rw = buffer->destWin;
			//这里应该判断是否有窗口过程，没有就使用默认winProc函数
			if(rw->isHaveWinProc())
			{
				rw->winProc(buffer->destWin,buffer->fromWin,buffer->type,buffer->data1,buffer->data2);
				printf("-------end\n");
			}
			else
			{
				rw->setWinProc(winDefaultProc);
				rw->winProc(buffer->destWin,buffer->fromWin,buffer->type,buffer->data1,buffer->data2);
			}
		}
	}
}

//------------------------------------static function------------------------------------

static void winDefaultProc(rootWin* rw,rootWin* fw, MsgType mt, uint32_t d1, uint32_t d2)
{ //默认没有参数 可以添加这个 把数据原封不动的给父类
	switch(mt)
	{
		case MSG_CLICK: break;
		case MSG_RELEASECLICK: break;
		default: 
			if(rw->getParent() != NULL){
				message* msg = new message();
				msg->type = mt;
				msg->data1 = d1;
				msg->data2 = d2;
				msg->destWin = rw->getParent();
				msg->fromWin = fw; //
				rw->sendMSGtoBack(msg,queue);
			}break; 
	}
}	

//窗口过程0
static void winWeakProc0(rootWin* rw,rootWin* fw, MsgType mt, uint32_t d1, uint32_t d2)
{
	printf("--winproc 0\n");
	switch(mt)
	{
		case MSG_CLICK: break;
		case MSG_RELEASECLICK: break;
		case MSG_CLOSE: 
		{
			printf("close\n");
				dialogWin* dw1 = new dialogWin(800/5,120,3*800/5,150,"Click OK to close the window.",rw,queue,WS_DEFAULT);
				dw1->dialogInit();	
				dw1->setWinProc(winWeakProc3);
				dw1->registerWin();
				dw1->paintAll();
		}break;
		default:	
			printf("undefinition msg\n");
		break;
	}
}

static void winWeakProc1(rootWin* rw,rootWin* fw, MsgType mt, uint32_t d1, uint32_t d2)
{
	printf("keyboard--winproc 4\n");
	switch(mt)
	{
		case MSG_KEYBOARD:
		{
			char temp = ' ';
			((virKeyboardWin*)rw)->intConverChar(d1,d2,&temp);//数值转换为字符
			((virKeyboardWin*)rw)->keyboardSendChar(temp);
		}
		default:break;
	}
}

//窗口过程2
static void winWeakProc2(rootWin* rw,rootWin* fw, MsgType mt, uint32_t d1, uint32_t d2)
{
	printf("textBar--winproc 2\n");
	switch(mt)
	{
		case MSG_CLICK: break;
		case MSG_RELEASECLICK:
		{
			//唤醒键盘
			vkWin->wakeupAndConnect(rw);
		}
		break;
		case MSG_CHAR:
		{
			((textBarWin*)rw)->writeChar(((char)d1));
		}
		default:break;
	}
}

//窗口过程3
static void winWeakProc3(rootWin* rw,rootWin* fw, MsgType mt, uint32_t d1, uint32_t d2)
{
	printf("dialog--winproc 3\n");
	switch(mt)
	{
		case MSG_CLOSE:
		{
			printf("\n cancel close window! \n");
			((dialogWin*)rw)->closeDialog();
			rw->destroyWin();
		}break;
		case MSG_DIALOG:
		{
			if(((char*)d1)[0] == 'o')
			{
				printf("\n window will be closed! \n");
			((dialogWin*)rw)->closeDialog();
				rw->destroyWin();
			}else{
				printf("\n cancel close window! \n");
			((dialogWin*)rw)->closeDialog();
				rw->destroyWin();
			}
		}break;
		default:break;
	}
}

static void winWeakProc4(rootWin* rw , rootWin* fw , MsgType mt, uint32_t d1, uint32_t d2)
{
	printf("button--winproc 4\n");
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
			if(rw->isInArea(d1,d2))
			{
				LCD_SetColors(RED,RED);
				LCD_FillTriangle(300,500,400,240,240,340);
				LCD_DrawFullCircle(350,190,70);
				LCD_DrawFullCircle(450,190,70);
			}
		}break;
		default:	//处理不了就给父类
		if(rw->getParent() != NULL){
			message* msg = new message();
			msg->type = mt;
			msg->data1 = d1;
			msg->data2 = d2;
			msg->destWin = rw->getParent();
			msg->fromWin = fw; //
			rw->sendMSGtoBack(msg,queue);
		}break;
	}
} 

static void winWeakProc5(rootWin* rw , rootWin* fw , MsgType mt, uint32_t d1, uint32_t d2)
{
	printf("listBar--winproc 5\n");
	switch(mt)
	{
		case MSG_CLICK: 
		{
			if(rw->isInArea((uint16_t)d1,(uint16_t)d2))
			{
				((listBarWin*)rw)->pressListBar();//显示被按下
			}else{
				((listBarWin*)rw)->releaseListBar();
			}
			
		}break;
		case MSG_RELEASECLICK:
		{
			((listBarWin*)rw)->releaseListBar();//松开 
			if(rw->isInArea(d1,d2))//若为空间内部松开-->执行应有的程序
			{
				((listBarWin*)rw)->changeOpenList();
			}
		}break;
		case MSG_ITEM:
		{
			char* temp = (char*)d1;
			printf("--%s\n",temp);
		}break;
		default:
			if(rw->getParent() != NULL){
			message* msg = new message();
			msg->type = mt;
			msg->data1 = d1;
			msg->data2 = d2;
			msg->destWin = rw->getParent();
			msg->fromWin = fw; //
			rw->sendMSGtoBack(msg,queue);
		}break;
	}
}
static void winWeakProc61(rootWin* rw , rootWin* fw , MsgType mt, uint32_t d1, uint32_t d2)
{
	printf("option--winproc 61\n");
	switch(mt)
	{
		case MSG_RELEASECLICK:
		{
			((optionWin*)rw)->clickOption();
			printf("check>>>>>>>>>>%d",((optionWin*)rw)->isSelect());
		}
		default:break;
	}
}

static void winWeakProc6(rootWin* rw , rootWin* fw , MsgType mt, uint32_t d1, uint32_t d2)
{
	printf("radiobtn--winproc 6\n");
	switch(mt)
	{
		case MSG_RADIOBTN: 
		{
			//显示被按下
			printf(">>>>>>>>>>>>>>%s\n",(char*)d1);
		}break;
		default:break;
	}
}

static void winWeakProc71(rootWin* rw , rootWin* fw , MsgType mt, uint32_t d1, uint32_t d2)
{
	printf("slider1--winproc 71\n");
	switch(mt)
	{
		case MSG_CLICK: 
		{//显示被按下
			((trackTextWin*)rw)->sliderSliding(d1,d2);
		}break;
		case MSG_RELEASECLICK:
		{
			((trackTextWin*)rw)->releaseSlider();//显示松开
		}break;
		default:break;
	}
}
static void winWeakProc7(rootWin* rw , rootWin* fw , MsgType mt, uint32_t d1, uint32_t d2)
{
	printf("slider2--winproc 7\n");
	switch(mt)
	{
		case MSG_CLICK: 
		{//显示被按下
			((trackBarWin*)rw)->sliderSliding(d1,d2);
				printf("slider---------->>>>>%d",((trackBarWin*)rw)->getSliderValue());
				tempWin->setProgressValue(((trackBarWin*)rw)->getSliderValue());
		}break;
		case MSG_RELEASECLICK:
		{
			((trackBarWin*)rw)->releaseSlider();//显示松开
		}break;
		default:break;
	}
}



static void ClickHandle(uint16_t x , uint16_t y ,rootWin* mw,uint8_t* stat)
{
	//--------- 获取点按情况 ---------
	//stat 这里有点绝对 按下就不再获取x ，y值了 像滑块就无法实现了需改进
	if(getTouchUP() == 0) //&& (*stat) == 0)
	{
		GUIGetPoint(&x,&y);
		if(x < GUI_WIDTH && y < GUI_HIGH)
		{	
			//若被按下为松开 且位置出了当前的窗口区，向之前的对象发送click信号
			if(*stat == 1)
			{		
				if(curClickWin != NULL)
				{
					message* msg = new message();
					msg->type = MSG_CLICK;
					msg->data1 = x;
					msg->data2 = y;
					msg->destWin = curClickWin;
					msg->fromWin = NULL;
					if(xQueueSendToBack(queue,msg,QUE_WAIT_TIME) == pdPASS)
					{delete msg;}
				}
			}
			else//第一次点击时 寻找到其窗口 并发送click信号
			{
				printf("touch up = 0\n");
				rootWin* dw = mw->locateWin(x,y);
				printf("%d\n",*stat);
				*stat = 1;
				printf("%d\n",*stat);
				if(dw != NULL)
				{
					curClickWin = dw;
					message* msg = new message();
					msg->type = MSG_CLICK;
					msg->data1 = x;
					msg->data2 = y;
					msg->destWin = curClickWin;
					msg->fromWin = NULL;
					printf("%d\n",*stat);
					if(xQueueSendToBack(queue,msg,QUE_WAIT_TIME) == pdPASS)
					{printf("delete--\n");delete msg;}
				}
			}
		}
	}
	else if(getTouchUP() != 0 && (*stat) == 1)//之前按下的 松开
	{
		//获取 松开后的下x,y
		printf("touch up = 1\n");
		*stat = 0;
		GUIGetPoint(&x,&y);
			message* msg =  new message();
			msg->type = MSG_RELEASECLICK;
			msg->data1 = x;
			msg->data2 = y;
			msg->destWin = curClickWin;
			msg->fromWin = NULL;
			if(xQueueSendToBack(queue,msg,0) == pdPASS)
			{delete msg;}
		curClickWin = NULL;
	}
}

