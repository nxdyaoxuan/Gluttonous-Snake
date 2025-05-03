#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include <easyx.h>
#include <vector>
#include <cstdlib>
#include <random>
#include <ctime>
#include <string>
#include <chrono>
#include<algorithm>
#include<Windows.h>
#include<mmsystem.h>
#include <thread>
#include "GeneralFunction.h"
#include<fstream>
#pragma comment(lib, "Winmm.lib") // 链接音频库
#define MAPMAXW 60
//音乐部分头文件搜集到素材后加上即可


using namespace std;
//本框架使用多字节字符集
// ----------------------------------数据设计----------------------------
//背景素材
IMAGE BackGound;//背景图片
IMAGE GameIntro;

// 
//地图尺寸设计
int MapW = 20; int MapH = 20;
int temp_MapW = 20;
int temp_MapH = 20;

//其中1表示边界，0表示空地，其他数字表示食物。
int** map = new int* [MapH];

//音乐状态
bool isPressedMusic = 1;
bool isPressedSound = 1;

bool musicOfAll = 1;//背景音乐默认开
bool soundEffect = 1; //默认音效开

//蛇的状态
bool IsDie_snake = false;


//本局分数
int score = 0;
//游戏时间
int GameTime = 0;


//蛇的速度
double SnakeSpeed = 1;
double temp_SnakeSpeed = 1; //根据不同选项修改1、1.5、2

//蛇的长度
int SnakeLong = 5;//根据不同选项修改5、7、10
int temp_SnakeLong = 5;

//蛇当前的方向
char SnakeDir = 'D';

int ChangeSnakeX[4] = { 0 ,0 ,1 ,-1 };
int ChangeSnakeY[4] = { -1,1 ,0 , 0 };

/*特殊状态*/
bool BigHead = 0;
bool AbsorbFood = 0;
bool BigMap = 0;


//消息结构体变量
ExMessage msg = { 0 };
int MOUX = 0, MOUY = 0;//鼠标坐标

//蛇身数组坐标
int Snake[20 * 20][2];


//蛇头的坐标数组
int SnakeHead[10][2];//SnakeHead[0][0]表示原蛇头x，SnakeHead[0][1]表示原蛇头y,-1表示闲置


//食物
struct Food {
	int Foodx, Foody;//记录食物的位置
}food;
//食物当前状态
int FoodState = 0;//奇数为无，偶数为有

// 全局变量，用于控制游戏计时的暂停状态
static bool isGameRunning = true;

int colorR = 0, colorB = 0, colorG = 200;



//---------------------------------- 数据设计----------------------------


//-----------------------------------service-----------------------------
/* 地图初始化
	负责人：2

	简要说明：根据函数changeMapHW()，将地图、蛇、食物初始化


	for(....){
	0表示空
	1表示墙
   }


	传入类型：void
	返回值：void
*/
void initialGame();




/* 获取鼠标消息
	负责人：0

	简要说明：获取并修改鼠标消息

	如果点击鼠标左键，则获取此时鼠标的坐标，用于与按钮联动
	if (peekmessage(&msg, EX_MOUSE)) {
		  switch (msg.message)
			{
			case WM_MOUSEMOVE:   鼠标移动

			case WM_LBUTTONDOWN:   鼠标是否按下左键



			default:
				break;
			}

		MOUX=msg.x;
		MOUY=msg.y;
	}


	 传入类型：void
	 返回值：void
*/
void getMouseInfo();



/*速度选择
	负责人：4

	简要说明：
		鼠标点击对应按钮后，获取对应坐标，根据不同按钮改变蛇的速度SnakeSpeed
		按钮（蛇速度1）SnakeSpeed=1
		按钮（蛇速度2）SnakeSpeed=1.5
		按钮（蛇速度3）SnakeSpeed=2

	传入类型：void
	返回值：void
*/
void selectSnakeSpeed(int option);



/* 蛇初始长度选择
	负责人：1

	简要说明：
		鼠标点击对应按钮后，获取对应坐标，根据不同按钮改变食物生成的速度FoodSpeed
		按钮（初始长度1）SnakeLong=5
		按钮（初始长度2）SnakeLong=7
		按钮（初始长度3）SnakeLong=10

	传入类型：void
	返回值：void

*/
void selectSnakeLong(int option);




/*地图选择
	负责人：2
	默认做一个20*20的地图
	简要说明：根据按钮选择不同地图大小


	传入类型：void
	返回值：void

*/
void changeMapHW();




/*恢复默认皮肤
	负责人：3

	简要说明：鼠标点击对应按钮后，将蛇的皮肤更为默认皮肤，将掩码图也更换为默认皮肤的掩码图
	SnakeSkin=SnakeSkinArr[0][0];
	SnakeSkin_mask=SnakeSkinArr[1][0];

	传入类型：void
	返回值：void

*/
void restoreDefailtSkin();




/*选择皮肤
	负责人：3

	简要说明：鼠标点击对应按钮后，将蛇的皮肤更为鼠标所点击的皮肤
	如：
	SnakeSkin=SnakeSkinArr[0][1];
	SnakeSkin_mask=SnakeSkinArr[1][1];

	传入类型：void
	返回值：void
*/
void selectSkin(int option);




/*游戏时间变化t
	负责人：3

	简要说明：从游戏开始时记录时间（类似秒表）
	每秒使GameTime++即可
	每过一秒将时间更新,暂停时停止增加
	例如：
	#include <chrono>
	#include <thread>
	int count = 0;
	while (true) {
		count++;
		cout << "运行了 " << count << " 秒" << std::endl;

		if (count == 2) {
			cout << ">>> 已到达2秒 <<<" << endl;
		}

		// 固定等待1秒
		this_thread::sleep_for(chrono::seconds(1));
	传入类型：void
	返回值：void
*/
void gameTime();



/*获取方向键
	负责人：4

	简要说明：获取wasd方向消息
	注意：若同时按下多个键则返回N

	if(msg.message==WM_KEYDOWM){
		switch(msg.vkcode){
		同时按多个键->return N
			W->return W
			A->return A
			S->return S
			D->return D
		}
	}
	传入类型：void
	返回值：char
*/
char getKeyDir();




/* 改变方向
	负责人：4

	简要说明：根据获取的方向判断是否改变方向，若改变方向则更改当前方向
	if(getKeyDir()==N)直接return;

	if(getKeyDir()==SnakeDir)->不改变方向：return ;
	相反：if(getKeyDir()==SnakeDir的相反值)->不改变方向：return
	if(getKeyDir()！=SnakeDir)->改变方向：SnakeDir=getKeyDir(),return;
	传入类型：char
	返回值：void
*/
void canChangeDir(char getKeyDir);




/* 改变速度
	负责人：4

	简要说明：根据获取的按键判断是否加速，加速则返回1，不加速则返回0,若果速度达到2再按G，则将速度调至1
	if(msg.message==WM_KEYDOWM){
		if(msg.vkcode=='G'){
			SnakeSpeed++;
		   return;
		}
	}
	return;
	传入类型：void
	返回值：void

*/
void speedUpOrNo();




/* 暂停t *
	负责人：3

	简要说明：根据获取的按键判断是否加速，加速则返回1，不加速则返回0

	if(msg.message==WM_KEYDOWM){
		if(msg.vkcode=='Z'){
		   bool paused = false;
		   while (true) {
		   if (_kbhit()) { // 检测是否有按键按下
			char ch = _getch();
			if (ch == 'Z') { // 按 z 键暂停
				paused = !paused;
				cout << (paused ? "暂停中..." : "继续运行") << endl;
			}
		}
		if (!paused) {
			// 主循环任务
			cout << "Running..." << endl;
			Sleep(100); // Windows API 延时
		}
	}

		}
	}
	#include <conio.h>
	#include <windows.h>
	传入类型：void
	返回值：bool
*/
bool stopOrNo();




/*暂停后操作
	负责人：3

	简要说明：根据鼠标点击的按钮返回不同的值
	继续游戏：return 1；
	结束游戏：return 0；

	 传入类型：void
	返回值：bool
*/
void actAfterStop();



/* 更改蛇各个点的坐标

  负责人：2

	简要说明：先移动蛇头，再更改蛇身的各点的坐标，以达到移动的效果,,吃到食物则蛇身不动，蛇头向前移动
	如
		SnakeHead[0]=该方向的下一格的x坐标
		SnakeHead[1]=该方向的下一格的y坐标
		if（吃到食物）->蛇头移动，身子不动，在蛇头与身子连接处加一个蛇身坐标

		Snake[i][0]=上一块蛇身的坐标的x //贪吃蛇第i节x轴坐标
		Snake[i][1]=上一块蛇身的坐标的y //贪吃蛇第i节y轴坐标

		移动成功则返回1，
		否则返回0，


	  传入类型：void
		返回值：bool

*/
bool changePositionOfSnakeBody();



/* 蛇的定时移动
	负责人：4

	简要说明：蛇没死则根据蛇的速度和方向移动

	if(！bool IsDie();){

	 changePositionOfSnakeBody();
}
	  传入类型：void
	返回值：void
*/
void snakeMove();




/*蛇的死亡判断
	 负责人：0

	简要说明：蛇撞到墙或自身时会死亡
	蛇头与墙/自身重合则返回1，否则返回0
	if(蛇头的坐标==蛇身坐标||蛇头的坐标==墙的坐标)
	if(soundEffect==1)
	播放死亡音乐;
	调用结束函数
	return1;

	return0;


	传入类型：void
	返回值：bool
*/
bool IsDie();



/* 蛇吃到食物判断
	负责人：1

	简要说明：
	根据不同食物产生不同效果， 吃到了返回1，没吃到返回0

	if(蛇头的坐标==食物坐标){
		if(食物==2)->增长 void snakeChangelength();
		if(食物==3)->扩大地图 void expandMap();
		if(食物==4)->头变大void headGrow();
		if（食物==5）->吸附蛇头为中心3*3范围的食物

		if(soundEffect==1)
		播放吃东西音乐
	将食物坐标的数字改为0
	蛇长++
	FoodState++；//食物状态改变
	return foodType;
	}
	return -1;

	 传入类型：void
	返回值：int
*/
bool eatFood();


/*扩大地图
负责人：2

	简要说明：
	吃到对应食物时扩大地图
	20*20->60*60->MAPMAX*MAPMAX
	如果地图未达到最大(宏定义长宽)，则继续扩大

	map=TempMap;
	delete[] map;

	 传入类型：void
	返回值：void
*/
void expandMap();


/*头变大*
 负责人：0

	简要说明：
	以头坐标为左列中点向外扩展，1*1变成3*3
   （在头部周围加上蛇身坐标即可）

	将增加的蛇头坐标加入蛇头坐标数组中

	 传入类型：void
	 返回值：void


*/
void headGrow();


/*吸附蛇头为中心3*3范围的食物*
	负责人：2

	简要说明：
	以头坐标为左列中点向外扩展,3*3范围内存在食物则自动吸附

	当蛇头3*3范围内有食物时 ，直接食物状态变化FoodState++；
	然后根据不同食物类型执行对应函数

	 if(蛇头3*3的坐标==食物坐标){
		if(食物==2)->增长 void snakeChangelength();
		if(食物==3)->扩大地图 void expandMap();
		if(食物==4)->头变大 void headGrow();
		if（食物==5）->吸附蛇头为中心3*3范围的食物 bool snakeAbsorbFood();

	将食物坐标的数字改为0
	FoodState++；//食物状态改变
	return 1;
	}
	return 0;

	 传入类型：void
	返回值：bool

*/
bool snakeAbsorbFood();


/*游戏得分变化
	负责人：0

	简要说明：根据蛇是否吃到食物判断得分是否增加
	if(bool eatFood()||bool snakeAbsorbFood();){
	score++;
	return ;
	}
	return;


	 传入类型：bool//蛇吃到食物判断
	 返回值：void
*/
void scoreChange(bool /*(IsEatFood || snakeAbsorbFood)*/);



/*吃特殊食物过段时间后复原**
	bool BigHead = 0;
	bool AbsorbFood = 0;
	bool BigMap = 0;
	 负责人：0

	简要说明：根据特殊状态全局变量查看是否存在特殊状态，若蛇的大头和吸附效果超过五秒，则复原
	if(!BigHead&&BigHeadTime>=5){//修复大头
		建立一个新的蛇数组，将原数组的原头坐标及之后的坐标一一赋值给新蛇数组，再将新蛇数组的地址传给原数组
		int (*TempSnake)[2] = new int[20*20][2];

		for(.....){
	.....//坐标赋值
	}

		for(.....){
	....地址赋值
	}
	BigHead=0;
	delete[] TempSnake
	}

	if(!AbsorbFood&&AbsorbFoodTime>=5){//修复吸收食物        //可以单纯用时间判断,时间结束停止吸附即可

		AbsorbFood=0；


	}




	 传入类型：void
	 返回值：void

*/
void recoverSnake();



/* 蛇的增长

	负责人：1

	简要说明：根据蛇是否吃到食物判断蛇的长度是否增加

	if(eatFood()){
	头部向前移一格，蛇身不动，并在蛇头与蛇身的连接出加一格蛇身

	return ;
	}

	return;


	 传入类型：void
	 返回值：void
*/
bool snakeChangelength();




/* 判断食物当前状态FoodState
	负责人：1

	简要说明：根据FoodState判断食物状态

	如果FoodState为奇数则return 0;
	如果FoodState为偶数则return 1;
	奇数为无，偶数为有

	传入类型：void
	返回值：bool
*/
bool  nowFoodState();



/* 生成食物
	负责人：1


	 简要说明：根据FoodState判断是否生成食物，若是则随机生成食物（食物不能与蛇和墙重合），并将食物的xy值返回到Food中，否则return；

	 //需要限制食物的产生位置，距离边界五格内的位置无法产生食物


	if（nowFoodState()）{
		return;
	}
		creatfood:

		随机生成Food.x,Food.y
		if(（Food.x,Food.y）==墙的坐标/蛇的坐标)
		goto   creatfood;
		根据坐表改变数组内数字为食物数字
			int type = rand()%4 表示3种食物
			其中0为普通食物：蛇长+1 用 map[a][b]=2表示
			  1 为扩大地图食物： x*=1.5；y*=1.5;  用 map[a][b]=3表示
			  2为头变大食物：   用map[a][b]=4表示
			  3 一段时间内为吸附蛇头为中心3*3范围的食物   用map[a][b]=5;表示
			   注意：只有地图空地0处才可能产生食物。
		FoodState++;
		return;


		传入类型：void
		返回值：void;
*/
void creatFood();



/* 游戏结束判断
	负责人：0

	简要说明：
		蛇死亡或者结束游戏则返回1，否则返回0；

		 传入类型：bool
		 返回值：bool
*/
bool gameIsOver(bool /*(蛇死亡&& 结束游戏)*/);



/* 计算排行榜排名
	负责人：0

	简要说明：

		从文档中取出排名数据，存入数组并与当前分数进行排序，排序后return此局分数排名下标即可；


		传入类型：void
		返回值：int
*/
int getRankList();
//-----------------------------------service-----------------------------

//-------------------------------------view------------------------------应加入适当的关闭窗口
//(注：跳转指关闭原窗口，打开新窗口)
/*
菜单界面
	负责人：5

	简要说明：
		该函数用于展示菜单界面，包含按钮(排行榜、退出游戏、开始游戏、设置)、背景等元素
		initgraph
while（1）{

		.....
		按下开始游戏跳转到模式选择界面 void modeSelectionView();
		closegraph
		按下排行榜跳转到排行榜界面 void rankListView();
		closegraph
		按下设置跳转到设置界面 void settingView();
		closegraph
		按下退出游戏按钮退出游戏exit(1)
		closegraph
	}

		  传入类型：void
			返回值：void
*/
void menuView();




/*
设置界面
	负责人：5
	设置界面：

	简要说明：
		该函数用于展示设置界面，包含按钮（恢复默认、皮肤设置、自定义值设置、返回菜单界面、背景音乐 开/关、吃食物音效 开/关、死亡音效 开/关）、背景
		initgraph
while(1){

		按下恢复默认将皮肤设置成默认皮肤 void restoreDefailtSkin();
		按下皮肤设置展示皮肤设置界面 void skinDefineView();
		closegraph
		按下自定义设置展示自定义设置界面 void userDefineView();
		closegraph
		按下返回按钮时return,closegraph返回菜单界面--
		【背景音乐 开/关】【吃食物音效 开/关】【死亡音效 开/关】

	}


	 传入类型：void
	返回值：void
 */
void settingView();




/*
模式选择界面
	负责人：5

	简要说明：
		该函数用于展示模式选择界面，包含按钮（普通模式、自定义模式（不计入排行榜）、返回菜单界面）、背景
		initgraph
	while(1){
		按下普通模式时跳转到难度选择界面 void difficultySelectionView();
		closegraph
		按下自定义模式时跳转到自定义界面 void userDefineView();
		closegraph
		按下返回按钮时return,closegraph返回菜单界面


	}

	传入类型：void
	返回值：void
*/
void modeSelectionView();




/*难度选择界面
	负责人：6

	简要说明：
		该函数用于展示难度选择界面，包含按钮（初始速度设置、开始）、背景

	initgraph
	按下初始速度设置跳转到初始速度设置界面 void speedDefinedView();
	closegraph
	按下开始跳转到游戏界面 void gameView();
	closegraph
	按下返回按钮时return,closegraph返回菜单界面


	 传入类型：void
	返回值：void
*/
void difficultySelectionView();




/*自定义界面
	负责人：6


	简要说明：
		该函数用于展示自定义界面，包含按钮（初始速度设置、初始长度设置、食物生成速度设置、地图设置、音乐设置、开始）、背景
 initgraph
 while(1){

	按下初始速度设置跳转到初始速度设置界面 void speedDefinedView();
	closegraph
	按下初始长度设置跳转到初始长度设置界面 void longDefineView();
	closegraph

	按下地图设置跳转到地图设置界面 void mapDefineView();
	closegraph
	按下开始跳转到游戏界面 void gameView();
	closegraph
	按下返回按钮时return,closegraph返回菜单界面

	}


	 传入类型：void
	返回值：void
*/
void userDefineView();


/*音乐设置界面
	负责人：6


	简要说明：
		该函数用于展示音乐设置，根据鼠标坐标相应按钮，包含按钮（背景音乐 开/关、吃食物音效 开/关、死亡音效 开/关、返回）、背景
 initgraph
	while(1){
		背景音乐 开/关（）;musicOfAll=1/0;
		吃东西音乐 开/关（）;soundEffect=1/0;
		死亡音乐 开/关（）;soundEffect=1/0;


	}

 closegraph

*/

/*初始速度设置界面
	负责人：7

	简要说明：展示初始速度设置界面，包含按钮（速度1、速度2、速度3、确认）、背景

	按下对应按钮更改速度
	initgraph
	while(1){
		按下对应按钮更改速度  void selectSnakeSpeed();
		按下确认return,closegraph返回自定义界面
	}


	传入类型：void
	返回值：void
*/
void speedDefinedView();




/*初始长度设置界面
	负责人：7

	简要说明：展示初始长度设置界面，包含按钮（长度1、长度2、长度3、确认）、背景

   initgraph

	while(1){
			 按下对应按钮更改长度 void selectSnakeLong();
			 按下确认return,closegraph返回自定义界面
	}

	 传入类型：void
	返回值：void
*/
void longDefineView();



/* 地图设置界面
	负责人：7

	简要说明：展示地图设置设置界面，包含按钮（确认）、长度输入栏，宽度输入栏、背景

	initgraph
   while(1){

	根据对应数据更改地图长宽 void changeMapHW();
	按下确认return,closegraph返回自定义界面

	}

	传入类型：void
	返回值：void
*/
void mapDefineView();







/*皮肤设置界面
	负责人：7

	简要说明：展示地图设置设置界面，包含按钮（皮肤1(图片按钮)、皮肤2（图片按钮）、确认）、背景
   initgraph
 while(1){
	按下对应按钮更改皮肤 void selectSkin();
	按下确认return,closegraph返回自定义界面



	}

	 传入类型：void
	返回值：void
*/
void skinDefineView();



/* 游戏界面
	负责人：8

	简要说明：展示游戏界面（边界和蛇）、背景、键盘交换（WASD改变方向/Z暂停/G加速）、显示游戏时间、得分、蛇定期移动、蛇增长、食物刷新
	 initgraph
	 while(1){
		时间开始++  GameTime++
		蛇移动函数 void snakeMove();
		判断蛇死亡函数,死亡则展示结束界面  bool IsDie(); void gameOverView();
		蛇方向改变函数 void canChangeDir(char getKeyDir);
		蛇增长函数 void snakeChangelength(bool isEatFood);
		食物刷新函数  void creatFood();
		显示时间  GameTime
		显示得分 score
		按下Z:展示暂停界面 void stopView();
		按下G:加速 void speedUpOrNo();


	}
	 传入类型：void
	返回值：void
*/
void gameView();



/* 暂停界面
   负责人：8

	简要说明：展示暂停界面，包含按钮（返回游戏、退出游戏）、背景
	initgraph
	进行暂停后操作 bool actAfterStop();
	继续游戏：return
	closegraph
	结束游戏: void gameOverView();
	closegraph
	 传入类型：void
	返回值：void
*/
void stopView();



/*排行榜界面
	负责人：9

	简要说明：展示排行榜界面，根据排行榜文档展示排行榜前十名

	initgraph
	获取文档中的数据并展示即可

	传入类型：void
	返回值：void
*/
void rankListView();



/* 结束界面
	负责人：9

	简要说明：展示结束界面，包含本局分数、本局排名、按钮（重新开始、排行榜、返回菜单）
	initgraph
	while(1){
	展示当前排名int getRankList();
	按下重新开始时跳转到游戏界面 void gameView();
	closegraph
	按下排行榜时跳转到排行榜界面 void rankListView();
	closegraph
	按下返回菜单时跳转到菜单界面 void menuView();
	closegraph

	}

	传入类型：void
	返回值：void
*/
void gameOverView();

void gameIntroView();


void showSnake(int WIDTH, int HEIGHT, int GRID_SIZE);

void drawBase();

void loadGameResource();


//-------------------------------------view------------------------------

void setPaused(bool paused);


/*
s---------------------------------------------------------------------------------------
*/
int main()
{

	srand(time(NULL));
	menuView();
	return 0;

}


void initialGame()
{
	IsDie_snake = false;//蛇是否死亡
	SnakeDir = 'D';



	MapW = temp_MapW;  MapH = temp_MapH;
	SnakeLong = temp_SnakeLong;


	// //蛇的速度
	SnakeSpeed = temp_SnakeSpeed;



	//本局分数
	score = 0;
	//游戏时间
	GameTime = 0;
	//蛇的速度
	/*SnakeSpeed = 1;*/
	//蛇的长度



	for (int i = 0; i < 400; i++)
	{
		Snake[i][0] = -1;
		Snake[i][1] = -1;
	}



	BigHead = false;
	AbsorbFood = false;
	BigMap = false;



	FoodState = 0;//奇数为无，偶数为有


	//动态分配二维数组内存
	map = new int* [MapH];

	for (int i = 0; i < MapH; i++)
	{

		map[i] = new int[MapW]; //!!!!在游戏结束最后阶段要加delete[]map
	}

	for (int i = 0; i < MapH; i++)//初始化地图
	{
		for (int j = 0; j < MapW; j++)
		{
			if (i == 0 || i == MapH - 1 || j == 0 || j == MapW - 1)
			{
				map[i][j] = 1; continue;
			}
			map[i][j] = 0;

		}
	}

	for (int i = 0; i < SnakeLong; i++)//初始化蛇身体位置
	{
		Snake[i][0] = SnakeLong - i;
		Snake[i][1] = 1;

	}

	SnakeHead[0][0] = SnakeLong + 1;//初始化原蛇头位置；
	SnakeHead[0][1] = 1;

	for (int i = 1; i < 10; i++)
	{
		SnakeHead[i][0] = -1;
		SnakeHead[i][1] = -1;
	}

	creatFood();
}





void getMouseInfo()
{
	if (peekmessage(&msg, EX_MOUSE)) {
		switch (msg.message) {

		case WM_LBUTTONDOWN: // 鼠标左键按下
			MOUX = msg.x; // 更新鼠标X坐标
			MOUY = msg.y; // 更新鼠标Y坐标
			break;

		default:
			break;
		}
	}
}

void selectSnakeSpeed(int option)
{
	if (option == 1)temp_SnakeSpeed = 1;
	else if (option == 2)temp_SnakeSpeed = 1.5;
	else temp_SnakeSpeed = 2;

}

void selectSnakeLong(int option)
{
	if (option == 1)
		temp_SnakeLong = 5;
	else if (option == 2)
		temp_SnakeLong = 7;
	else
		temp_SnakeLong = 10;
}



//void changeMapHW()
//{
//	initialGame();////view中 按钮里面已经把mapW和mapH改了，初始化就行了；
//}

void restoreDefailtSkin()
{
	// 检查鼠标是否在按钮范围内
	colorR = 0; colorB = 0; colorG = 200;

}

void selectSkin(int option)
{
	// 假设通过某种方式获取了鼠标点击的选项

	if (option == 1) // 皮肤1按钮
	{
		colorR = 0; colorB = 100; colorG = 100;
	}
	else if (option == 2) // 皮肤2按钮
	{
		colorR = 100; colorB = 0; colorG = 100;
	}
	else if (option == 3) // 皮肤3按钮
	{
		colorR = 120; colorB = 100; colorG = 200;
	}
}

void gameTime()
{
	static std::chrono::time_point<std::chrono::steady_clock> lastTime;
	static bool initialized = false;

	// 初始化计时器
	if (!initialized) {
		lastTime = std::chrono::steady_clock::now();
		initialized = true;
	}

	// 只有在游戏运行状态下才计时
	if (isGameRunning) {
		auto currentTime = std::chrono::steady_clock::now();
		auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastTime).count();

		// 每过一秒增加游戏时间
		if (elapsedTime >= 1) {
			GameTime += static_cast<int>(elapsedTime);
			lastTime = currentTime;
			cout << "游戏已运行" << GameTime << "秒" << std::endl;
		}
	}
}

char getKeyDir()
{
	if (peekmessage(&msg, EX_KEY)) {
		switch (msg.vkcode) {
		case 'W':return 'W';
		case 'w':return 'W';
		case 'A':return 'A';
		case 'a':return 'A';
		case 'S':return 'S';
		case 's':return 'S';
		case 'D':return 'D';
		case 'd':return 'D';
		default:return 'N';
		}
	}
	return 0;
}

void canChangeDir(char getKeyDir)
{
	if (getKeyDir == 'N')return;
	if (getKeyDir == SnakeDir)return;
	else if ((getKeyDir == 'A' && SnakeDir == 'D') || (getKeyDir == 'D' && SnakeDir == 'A'))return;  //四个方向4种相反的情况列出
	else if ((getKeyDir == 'W' && SnakeDir == 'S') || (getKeyDir == 'S' && SnakeDir == 'W'))return;
	else SnakeDir = getKeyDir;  // 排除相同相反，只剩下90°可移动。
}


void speedUpOrNo()
{
	if (SnakeSpeed == 1)SnakeSpeed = 1.5;          //速度循环变化方案
	else if (SnakeSpeed == 1.5)SnakeSpeed = 2;
	else if (SnakeSpeed == 2)SnakeSpeed = 1;
}

bool stopOrNo()
{
	static bool paused = false;

	// 检测Z键是否被按下
	if (GetAsyncKeyState('Z') & 0x0001) { // 使用GetAsyncKeyState检测按键，只在按下瞬间触发一次
		// 切换暂停状态
		paused = !paused;

		// 输出当前状态信息
		std::cout << (paused ? "游戏已暂停" : "游戏已继续") << std::endl;

		// 通知计时器暂停/继续
		setPaused(paused);
	}

	return paused;
}

void actAfterStop()
{
	setPaused(true);
	// 显示暂停界面
	stopView();

	// 根据鼠标点击位置判断用户选择
	//while (true) {
	//	if (peekmessage(&msg, EM_MOUSE))
	//	{
	//		if (msg.message == WM_LBUTTONDOWN) {
	//			getMouseInfo();
	//			// 点击返回游戏按钮
	//			if (MOUX >= 250 && MOUX <= 350 && MOUY >= 125 && MOUY <= 225) {
	//				closegraph();
	//				// 重启计时器，恢复游戏运行状态
	//				setPaused(false);
	//				return false; // 继续游戏
	//			}
	//			// 点击结束游戏按钮
	//			else if (MOUX >= 250 && MOUX <= 350 && MOUY >= 275 && MOUY <= 375) {
	//				gameOverView();
	//				return true; // 结束游戏
	//			}
	//		}
	//	}
	//	
	//}
}

bool changePositionOfSnakeBody()
{

	for (int i = SnakeLong - 1; i > 0; i--)//蛇身
	{
		Snake[i][0] = Snake[i - 1][0];
		Snake[i][1] = Snake[i - 1][1];
	}



	int y_tempx = (SnakeHead[0][0] + Snake[0][0]);
	int y_tempy = (SnakeHead[0][1] + Snake[0][1]);

	if ((y_tempx % 2 == 0) && (y_tempy % 2 == 0))//未转向
	{
		Snake[0][0] = y_tempx / 2;
		Snake[0][1] = y_tempy / 2;
	}
	else//转向
	{
		if (SnakeDir == 'W' || SnakeDir == 'w' || SnakeDir == 'S' || SnakeDir == 's')//上下转，snake[0][0]及x坐标与snakehead[0][0]坐标相同
		{
			Snake[0][0] = SnakeHead[0][0];
			//SnakeHead[0][1]y坐标不变
		}

		else//上下转，snake[0][1]及y坐标与snakehead[0][1]坐标相同
		{
			Snake[0][1] = SnakeHead[0][1];
			//SnakeHead[0][0]x坐标不变
		}
	}
	return false;
}


void snakeMove()
{
	int i = 0;
	int k = 0;
	if (BigHead)k = 1;
	switch (SnakeDir) {  //蛇头移动
	case 'W':
		i = 0;  //上
		SnakeHead[0][0] += ChangeSnakeX[i];
		SnakeHead[0][1] += ChangeSnakeY[i];
		if (k) {
			for (int n = 1; n <= 9; n += 3) {
				SnakeHead[n][0] = SnakeHead[0][0] - 1;
				SnakeHead[n + 1][0] = SnakeHead[0][0];
				SnakeHead[n + 2][0] = SnakeHead[0][0] + 1;
			}
			for (int n = 1; n <= 3; n++) {
				SnakeHead[n][1] = SnakeHead[0][1] - 1;
				SnakeHead[n + 3][1] = SnakeHead[0][1] - 2;
				SnakeHead[n + 6][1] = SnakeHead[0][1] - 3;
			}
		}
		break;
	case 'S':
		i = 1;  //下
		SnakeHead[0][0] += ChangeSnakeX[i];
		SnakeHead[0][1] += ChangeSnakeY[i];
		if (k) {
			for (int n = 1; n <= 9; n += 3) {
				SnakeHead[n][0] = SnakeHead[0][0] - 1;
				SnakeHead[n + 1][0] = SnakeHead[0][0];
				SnakeHead[n + 2][0] = SnakeHead[0][0] + 1;
			}
			for (int n = 1; n <= 3; n++) {
				SnakeHead[n][1] = SnakeHead[0][1] + 1;
				SnakeHead[n + 3][1] = SnakeHead[0][1] + 2;
				SnakeHead[n + 6][1] = SnakeHead[0][1] + 3;
			}
		}
		break;

	case 'A':
		i = 3;  //左
		SnakeHead[0][0] += ChangeSnakeX[i];
		SnakeHead[0][1] += ChangeSnakeY[i];
		if (k) {
			for (int n = 1; n <= 9; n += 3) {
				SnakeHead[n][1] = SnakeHead[0][1] - 1;
				SnakeHead[n + 1][1] = SnakeHead[0][1];
				SnakeHead[n + 2][1] = SnakeHead[0][1] + 1;
			}
			for (int n = 1; n <= 3; n++) {
				SnakeHead[n][0] = SnakeHead[0][0] - 1;
				SnakeHead[n + 3][0] = SnakeHead[0][0] - 2;
				SnakeHead[n + 6][0] = SnakeHead[0][0] - 3;
			}
		}
		break;
	case 'D':
		i = 2;  //右
		SnakeHead[0][0] += ChangeSnakeX[i];
		SnakeHead[0][1] += ChangeSnakeY[i];
		if (k) {
			for (int n = 1; n <= 9; n += 3) {
				SnakeHead[n][1] = SnakeHead[0][1] - 1;
				SnakeHead[n + 1][1] = SnakeHead[0][1];
				SnakeHead[n + 2][1] = SnakeHead[0][1] + 1;
			}
			for (int n = 1; n <= 3; n++) {
				SnakeHead[n][0] = SnakeHead[0][0] + 1;
				SnakeHead[n + 3][0] = SnakeHead[0][0] + 2;
				SnakeHead[n + 6][0] = SnakeHead[0][0] + 3;
			}
		}
		break;
	}
	if (!IsDie_snake) {
		mciSendString("close ../GameMaterials/eatsound.mp3", NULL, 0, NULL);
		if (!eatFood()) {
			changePositionOfSnakeBody();    // 未吃到食物变长调用蛇身移动函数
		}
	}
}

bool IsDie() {                                                                            //map[headY][headX]矩阵先行后列，他没考虑蛇头扩大
	// 检查蛇头是否撞墙
	loadGameResource();
	if (SnakeHead[0][0] <= 0 || SnakeHead[0][0] >= MapW - 1 || SnakeHead[0][1] <= 0 || SnakeHead[0][1] >= MapH - 1) {
		if (soundEffect) {
			// 播放死亡音乐
			PlaySound(TEXT("../GameMaterials/deathsound.wav"), NULL, SND_FILENAME | SND_ASYNC);
		}
		IsDie_snake = true;
		return true; // 碰到边界，返回死亡
	}
	;
	// 检查蛇头是否撞到自己
	for (int i = 0; i < SnakeLong; i++) {
		if (SnakeHead[0][0] == Snake[i][0] && SnakeHead[0][1] == Snake[i][1]) {
			if (soundEffect) {
				// 播放死亡音乐
				PlaySound(TEXT("../GameMaterials/deathsound.wav"), NULL, SND_FILENAME | SND_ASYNC);
			}
			IsDie_snake = true;
			return true; // 碰到自己，返回死亡
		}
	}

	// 如果蛇头处于大头状态，检查大头部分是否撞到自己
	if (BigHead) {
		for (int i = 1; i < 10; i++) { // 检查SnakeHead数组中的大头部分
			if (SnakeHead[i][0] <= 0 || SnakeHead[i][0] >= MapW - 1 || SnakeHead[i][1] <= 0 || SnakeHead[i][1] >= MapH - 1) {
				if (soundEffect) {
					// 播放死亡音乐
					PlaySound(TEXT("../GameMaterials/deathsound.wav"), NULL, SND_FILENAME | SND_ASYNC);
				}
				IsDie_snake = true;
				return true; // 碰到边界，返回死亡
			}
		}
		for (int i = 1; i < 10; i++) {
			for (int j = 0; j < SnakeLong; j++) {
				if (Snake[j][0] == SnakeHead[i][0] && Snake[j][1] == SnakeHead[i][1]) {
					if (soundEffect) {
						// 播放死亡音乐
						PlaySound(TEXT("../GameMaterials/deathsound.wav"), NULL, SND_FILENAME | SND_ASYNC);
					}
					IsDie_snake = true;
					return true; // 碰到自己，返回死亡
				}
			}
		}
	}
	IsDie_snake = false;
	return false; // 没有死亡
}




bool eatFood()//判断是否吃到食物，吃到执行对应效果
{
	
	int foodType;
	bool flag = false;//用于判断食物是否与蛇头重合（吃到）
	//loadGameResource();
	if (BigHead)
	{
		for (int i = 0; i < 10; i++)
			if (SnakeHead[i][0] == food.Foodx && SnakeHead[i][1] == food.Foody)
			{
				flag = true;//true表重合
				break;
			}
	}
	else if (AbsorbFood)
	{
		bool judge[10];//用于判断延伸坐标是否合理
		for (int i = 0; i < 10; i++)
			judge[i] = true;//初始化
		int a[10][2];//存放延伸坐标
		a[0][0] = SnakeHead[0][0];//存放根头坐标
		a[0][1] = SnakeHead[0][1];
		switch (SnakeDir)//判断目前蛇头方向
		{
		case 'W'://蛇头朝上
		case 'w':
			for (int i = 0; i < 3; i++)//推断根头的前面三个坐标
			{
				a[i + 1][0] = a[0][0];
				a[i + 1][1] = a[0][1] - i - 1;
				if (a[i + 1][1] <= 0)//等于0为边界,纵坐标<=0,超出边界
					judge[i + 1] = false;
			}
			for (int i = 0; i < 3; i++)//推断根头左边前面三个坐标
			{
				a[i + 4][0] = a[0][0] - 1;
				a[i + 4][1] = a[0][1] - i - 1;
				if (a[i + 4][1] <= 0 || a[i + 4][0] <= 0)//等于0为边界,纵坐标<=0或横坐标≤0,超出边界   
					judge[i + 4] = false;
			}
			for (int i = 0; i < 3; i++)//推断根头右边前面三个坐标
			{
				a[i + 7][0] = a[0][0] + 1;
				a[i + 7][1] = a[0][1] - i - 1;
				if (a[i + 7][1] <= 0 || a[i + 7][0] >= MapW - 1)//等于0和MapW-1为边界,纵坐标<=0,超出边界，横坐标>=MapW-1,超出边界
					judge[i + 7] = false;
			}
			break;
		case 'S'://蛇头朝下
		case 's':
			for (int i = 0; i < 3; i++)//推断根头的下面三个坐标
			{
				a[i + 1][0] = a[0][0];
				a[i + 1][1] = a[0][1] + i + 1;
				if (a[i + 1][1] >= MapH - 1)//等于MapH-1为边界,纵坐标>= MapH - 1,超出边界
					judge[i + 1] = false;
			}
			for (int i = 0; i < 3; i++)//推断根头左边下面三个坐标
			{
				a[i + 4][0] = a[0][0] - 1;
				a[i + 4][1] = a[0][1] + i + 1;
				if (a[i + 4][1] >= MapH - 1 || a[i + 4][0] <= 0)//等于MapH - 1为边界,纵坐标>= MapH - 1或横坐标≤0,超出边界   
					judge[i + 4] = false;
			}
			for (int i = 0; i < 3; i++)//推断根头右边下面三个坐标
			{
				a[i + 7][0] = a[0][0] + 1;
				a[i + 7][1] = a[0][1] + i + 1;
				if (a[i + 7][1] >= MapH - 1 || a[i + 7][0] >= MapW - 1)//等于MapH - 1和MapW-1为边界,纵坐标>= MapH - 1,超出边界，横坐标>=MapW-1,超出边界
					judge[i + 7] = false;
			}
			break;
		case 'A'://蛇头朝左
		case 'a':
			for (int i = 0; i < 3; i++)//推断根头的前面三个坐标
			{
				a[i + 1][0] = a[0][0] - i - 1;
				a[i + 1][1] = a[0][1];
				if (a[i + 1][0] <= 0)//等于0为边界,横坐标<=0,超出边界
					judge[i + 1] = false;
			}
			for (int i = 0; i < 3; i++)//推断根头下面的前面三个坐标
			{
				a[i + 4][0] = a[0][0] - i - 1;
				a[i + 4][1] = a[0][1] + 1;
				if (a[i + 4][1] >= MapH - 1 || a[i + 4][0] <= 0)//等于0和MapH - 1为边界,纵坐标>=MapH - 1或横坐标≤0,超出边界   
					judge[i + 4] = false;
			}
			for (int i = 0; i < 3; i++)//推断根头上面的前面三个坐标
			{
				a[i + 7][0] = a[0][0] - i - 1;
				a[i + 7][1] = a[0][1] - 1;
				if (a[i + 7][1] <= 0 || a[i + 7][0] <= 0)//等于0为边界,纵坐标<=0,超出边界，横坐标<= 0,超出边界
					judge[i + 7] = false;
			}
			break;
		case 'D'://蛇头朝右
		case 'd':
			for (int i = 0; i < 3; i++)//推断根头的前面三个坐标
			{
				a[i + 1][0] = a[0][0] + i + 1;
				a[i + 1][1] = a[0][1];
				if (a[i + 1][0] >= MapW - 1)//等于MapW - 1为边界,横坐标>=MapW - 1,超出边界
					judge[i + 1] = false;
			}
			for (int i = 0; i < 3; i++)//推断根头下面的前面三个坐标
			{
				a[i + 4][0] = a[0][0] + i + 1;
				a[i + 4][1] = a[0][1] + 1;
				if (a[i + 4][1] >= MapH - 1 || a[i + 4][0] >= MapW - 1)//等于MapW - 1和MapH - 1为边界,纵坐标>=MapH - 1或横坐标>=MapW - 1,超出边界   
					judge[i + 4] = false;
			}
			for (int i = 0; i < 3; i++)//推断根头上面的前面三个坐标
			{
				a[i + 7][0] = a[0][0] + i + 1;
				a[i + 7][1] = a[0][1] - 1;
				if (a[i + 7][1] <= 0 || a[i + 7][0] >= MapW - 1)//等于0和MapW - 1为边界,纵坐标<=0,超出边界，横坐标>= MapW - 1,超出边界
					judge[i + 7] = false;
			}
			break;
		}
		for (int i = 0; i < 10; i++)//遍历10个坐标
		{
			if (judge[i])//该坐标是否合理
				if (a[i][0] == food.Foodx && a[i][1] == food.Foody)//如果合理，是否与食物重合
				{
					flag = true;//true表重合
					break;
				}
		}
	}
	else if (SnakeHead[0][0] == food.Foodx && SnakeHead[0][1] == food.Foody) {
		flag = true;
	}


	if (flag)//判断是否吃到食物
	{//判断食物类型
		
		foodType = map[food.Foody][food.Foodx];
		if (foodType == 2);//正常食物,效果为长度++;
		else if (foodType == 3)//地图扩大食物（附带正常食物效果）
		{
			expandMap();
			BigMap = true;

		}
		else if (foodType == 4)//头部增大食物（附带正常食物效果）
		{
			headGrow();
			BigHead = true;
			std::thread(recoverSnake).detach();
			
		}
		else if (foodType == 5)//吸附食物（附带正常食物效果）
		{
			AbsorbFood = true;
			std::thread(recoverSnake).detach();
		}


		if (snakeChangelength())//以上结果均导致蛇长度增加,如果有位置，则增大长度//********************************************此处改动了snakeChangelength（）返回值，原为void，现为bool
			SnakeLong++;//蛇长增加
			score++;
		//**********************************************************************************************************************************************
		//调用游戏结束函数,蛇已经不能再增大了（未实现）
			
		
		map[food.Foody][food.Foodx] = 0;//重置地图
		food.Foodx = food.Foody = 0;//吃完后食物坐标变（0,0）

		/*
		解释：
		int FoodStateint = 0;奇数为无，偶数为有;
		FoodState偶数，createfood函数不生成食物   特例：foodstate为0，代表刚开始，此时会生成食物，此时不会foodstate++）
		FoodState奇数，createfood函数生成食物，foodstate++
		在无食物时，foodstate肯定为奇数，生成食物后foodstate++，foodstate为偶数
		在有食物时，foodstate肯定为偶数，吃了之后foodstate++，导致foodstate变为奇数
		*/
		if (soundEffect)//播放吃东西音乐
		{
			
			mciSendString("play ../GameMaterials/eatsound.mp3", 0, 0, 0);
		}
		FoodState++;//食物状态增加
		creatFood();
		return true; //吃到食物  
	}
	return false;//没吃到食物
}


void expandMap()
{
	if (MapW == MAPMAXW)//如果当前地图宽度等于最大限制宽度，return,退出函数
	{
		BigMap = true;
		return;
	}
	int y_tempfood = map[food.Foodx][food.Foody];//记录地图扩大前食物的值(种类)（2,3,4,5）

	for (int i = 0; i < MapH; i++)
	{
		delete[]map[i];
	}
	delete[]map;//释放原来分配的内存

	MapH = MapH + 10;//扩大
	MapW = MapW + 10;//扩大

	map = new int* [MapH];//重新动态分配二维数组内存

	for (int i = 0; i < MapH; i++)
	{
		map[i] = new int[MapW]; //!!!!在游戏结束最后阶段要加delete[]map
	}

	for (int i = 0; i < MapH; i++)//初始化地图
	{
		for (int j = 0; j < MapW; j++)
		{
			if (i == 0 || i == MapH - 1 || j == 0 || j == MapW - 1)
			{
				map[i][j] = 1; continue;
			}
			map[i][j] = 0;

		}
	}

	for (int i = 0; i < SnakeLong; i++)//蛇身的位置变换
	{
		Snake[i][0] = Snake[i][0] + 5;
		Snake[i][1] = Snake[i][1] + 5;
	}
	SnakeHead[0][0] += 5;
	SnakeHead[0][1] += 5;
	if (BigHead)
	{
		for (int i = 1; i < 10; i++)//蛇头的位置变换
		{
			SnakeHead[i][0] += 5;
			SnakeHead[i][1] += 5;
		}
	}

	food.Foodx += 5;//食物坐标变化
	food.Foody += 5;
	map[food.Foodx][food.Foody] = y_tempfood;//食物值

}



void headGrow() {
    int headX = SnakeHead[0][0]; // 当前蛇头的X坐标
    int headY = SnakeHead[0][1]; // 当前蛇头的Y坐标

    // 根据蛇头的方向，计算扩展的9个格子的坐标
    if (SnakeDir == 'w' || SnakeDir == 'W') { // 向上
        // 第一列
        SnakeHead[1][0] = headX - 1; SnakeHead[1][1] = headY - 3;
        SnakeHead[2][0] = headX - 1; SnakeHead[2][1] = headY - 2;
        SnakeHead[3][0] = headX - 1; SnakeHead[3][1] = headY - 1;
        // 第二列
        SnakeHead[4][0] = headX; SnakeHead[4][1] = headY - 3;
        SnakeHead[5][0] = headX; SnakeHead[5][1] = headY - 2;
        SnakeHead[6][0] = headX; SnakeHead[6][1] = headY - 1;
        // 第三列
        SnakeHead[7][0] = headX + 1; SnakeHead[7][1] = headY - 3;
        SnakeHead[8][0] = headX + 1; SnakeHead[8][1] = headY - 2;
        SnakeHead[9][0] = headX + 1; SnakeHead[9][1] = headY - 1;
    }
    else if (SnakeDir == 'a' || SnakeDir == 'A') { // 向左
        // 第一行
        SnakeHead[1][0] = headX - 3; SnakeHead[1][1] = headY - 1;
        SnakeHead[2][0] = headX - 2; SnakeHead[2][1] = headY - 1;
        SnakeHead[3][0] = headX - 1; SnakeHead[3][1] = headY - 1;
        // 第二行
        SnakeHead[4][0] = headX - 3; SnakeHead[4][1] = headY;
        SnakeHead[5][0] = headX - 2; SnakeHead[5][1] = headY;
        SnakeHead[6][0] = headX - 1; SnakeHead[6][1] = headY;
        // 第三行
        SnakeHead[7][0] = headX - 3; SnakeHead[7][1] = headY + 1;
        SnakeHead[8][0] = headX - 2; SnakeHead[8][1] = headY + 1;
        SnakeHead[9][0] = headX - 1; SnakeHead[9][1] = headY + 1;
    }
    else if (SnakeDir == 's' || SnakeDir == 'S') { // 向下
        // 第一列
        SnakeHead[1][0] = headX - 1; SnakeHead[1][1] = headY + 1;
        SnakeHead[2][0] = headX - 1; SnakeHead[2][1] = headY + 2;
        SnakeHead[3][0] = headX - 1; SnakeHead[3][1] = headY + 3;
        // 第二列
        SnakeHead[4][0] = headX; SnakeHead[4][1] = headY + 1;
        SnakeHead[5][0] = headX; SnakeHead[5][1] = headY + 2;
        SnakeHead[6][0] = headX; SnakeHead[6][1] = headY + 3;
        // 第三列
        SnakeHead[7][0] = headX + 1; SnakeHead[7][1] = headY + 1;
        SnakeHead[8][0] = headX + 1; SnakeHead[8][1] = headY + 2;
        SnakeHead[9][0] = headX + 1; SnakeHead[9][1] = headY + 3;
    }
    else if (SnakeDir == 'd' || SnakeDir == 'D') { // 向右
        // 第一行
        SnakeHead[1][0] = headX + 1; SnakeHead[1][1] = headY - 1;
        SnakeHead[2][0] = headX + 2; SnakeHead[2][1] = headY - 1;
        SnakeHead[3][0] = headX + 3; SnakeHead[3][1] = headY - 1;
        // 第二行
        SnakeHead[4][0] = headX + 1; SnakeHead[4][1] = headY;
        SnakeHead[5][0] = headX + 2; SnakeHead[5][1] = headY;
        SnakeHead[6][0] = headX + 3; SnakeHead[6][1] = headY;
        // 第三行
        SnakeHead[7][0] = headX + 1; SnakeHead[7][1] = headY + 1;
        SnakeHead[8][0] = headX + 2; SnakeHead[8][1] = headY + 1;
        SnakeHead[9][0] = headX + 3; SnakeHead[9][1] = headY + 1;
    }

    BigHead = true; // 标记大头状态
}

bool snakeAbsorbFood()
{
	headGrow(); 
	BigHead = false;
	AbsorbFood = true;
	return false;
}

void scoreChange(bool IsEatFood)
{
	if (IsEatFood) {
		score++; // 如果蛇吃到食物，增加得分
		return;
	}
	return;
}

void recoverSnake() 
{
	std::this_thread::sleep_for(std::chrono::seconds(5));
	if (AbsorbFood) AbsorbFood = false;
	else if (BigHead) BigHead = false;
}

///改变可以吃食物了
bool snakeChangelength()//************************************************************************************************此处改动返回值
{
	if (SnakeLong == 400)//蛇长到达数组最大长度，无法存储
		return false;

	/*
	蛇头与蛇身体的连接分为8种情况   w表蛇头   s表蛇身   ？表待处理的连接部分

	w  s             w  w
	?  ?  w?s  s?w  s?  ?s  s?  ?s
	s  w                     w  w

	*/

	for (int i = SnakeLong - 1; i >= 0; i--)//蛇身坐标全部往后移动一位，为待处理的？腾出位置
	{
		Snake[i + 1][0] = Snake[i][0];
		Snake[i + 1][1] = Snake[i][1];
	}

	int y_tempx = (SnakeHead[0][0] + Snake[0][0]);
	int y_tempy = (SnakeHead[0][1] + Snake[0][1]);

	if ((y_tempx % 2 == 0) && (y_tempy % 2 == 0))//未转向
	{
		Snake[0][0] = y_tempx / 2;
		Snake[0][1] = y_tempy / 2;
	}
	else//转向
	{
		if (SnakeDir == 'W' || SnakeDir == 'w' || SnakeDir == 'S' || SnakeDir == 's')//上下转，snake[0][0]及x坐标与snakehead[0][0]坐标相同
		{
			Snake[0][0] = SnakeHead[0][0];
			//SnakeHead[0][1]y坐标不变
		}

		else//上下转，snake[0][1]及y坐标与snakehead[0][1]坐标相同
		{
			Snake[0][1] = SnakeHead[0][1];
			//SnakeHead[0][0]x坐标不变
		}
	}

	return true;
}

//bool snakeChangelength()//************************************************************************************************此处改动返回值
//{
//	if (SnakeLong == 400)//蛇长到达数组最大长度，无法存储
//		return false;
//
//	/*
//	蛇头与蛇身体的连接分为8种情况   w表蛇头   s表蛇身   ？表待处理的连接部分
//
//	w  s             w  w
//	?  ?  w?s  s?w  s?  ?s  s?  ?s
//	s  w                     w  w
//
//	*/
//
//	for (int i = SnakeLong - 1; i >= 0; i--)//蛇身坐标全部往后移动一位，为待处理的？腾出位置
//	{
//		Snake[i + 1][0] = Snake[i][0];
//		Snake[i + 1][1] = Snake[i][1];
//	}
//
//	//此处假设蛇头数组的第一个坐标为与蛇身相连的部分
//	//由于移动了数组，所以蛇身第一个坐标下标为1
//	if (Snake[1][0] == SnakeHead[0][0])//第一、二种情况，横坐标相同
//	{
//		Snake[0][0] == Snake[1][0];//确定？横坐标
//		if (Snake[1][1] > SnakeHead[0][1])//第一种情况，蛇身体纵坐标大于蛇头
//			Snake[0][1] = Snake[1][1] - 1;//确定？纵坐标
//		else//第二种情况
//			Snake[0][1] = Snake[1][1] + 1;//确定？纵坐标            
//	}
//	else if (Snake[1][1] == SnakeHead[0][1])//第三、四种情况，纵坐标相同
//	{
//		Snake[0][1] == Snake[1][1];//确定？纵坐标
//		if (Snake[1][0] > SnakeHead[0][0])//第一种情况，蛇身体横坐标大于蛇头
//			Snake[0][0] = Snake[1][0] - 1;//确定？横坐标
//		else//第二种情况
//			Snake[0][0] = Snake[1][0] + 1;//确定？横坐标
//	}
//	else
//	{
//		Snake[0][0] = SnakeHead[0][0];
//		Snake[0][1] = Snake[1][1];
//	}
//	return true;
//}


void loadGameResource()
{
	loadimage(&BackGound, "../GameMaterials/BackGround.jpg", 640, 480);
	loadimage(&GameIntro, "../GameMaterials/GameIntro.png", 640, 480);
	mciSendString("open ../GameMaterials/eatsound.mp3", 0, 0, 0);
	mciSendString("open ../GameMaterials/deathsound.wav", 0, 0, 0);
	mciSendString("open ../GameMaterials/backmusic.wav", 0, 0, 0);
}

bool nowFoodState()
{
	/*
	注意，我这里与架构注释写的相反，因为偶数表示有食物，奇数表示没有，那么生成食物应该是在为奇数的时候生成
	*/
	if (FoodState & 1)//奇数返回true
		return true;
	else
		return false;//偶数返回false
}


void creatFood()
{
	if (nowFoodState() || FoodState == 0)//为0时特殊处理，需要生成食物
	{
		int x, y;
		int type;
		if (BigHead || AbsorbFood)
			type = rand() % 2 + 2;//生成2--3两种食物
		else
			type = rand() % 4 + 2;//生成2--5四种食物

		std::vector<vector<int>> temp_map;
		for (int i = 0; i < MapH; i++)//创建一个临时地图，用10标记蛇头和蛇身
			temp_map.push_back(std::vector<int>(map[i], map[i] + MapW));

		int end = 1;

		if (BigHead)//大头状态遍历蛇头数组
			end = 10;

		for (int i = 0; i < end; i++)//蛇头
			temp_map[SnakeHead[i][1]][SnakeHead[i][0]] = 10;
		for (int i = 0; i < SnakeLong; ++i)//蛇身
			temp_map[Snake[i][1]][Snake[i][0]] = 10;

		int count = 0;//由于头部增大食物范围比正常食物范围小，可能生成的一直与蛇重合。头部食物每次生成时，count++，到1000时，放弃生成该食物，改为生成普通食物（2）

		if (type == 4)//头部扩大食物,需限制生成范围
		{
			while (1)
			{
				if (count >= 1000)
				{
					type = 2;//改为普通食物
					break;
				}
				x = rand() % (MapW - 12) + 6;//该范围保证不会横坐标生成为地图左6个格子和右6个格子
				y = rand() % (MapH - 12) + 6;//该范围保证不会纵坐标生成为地图上6个格子和下6个格子
				count++;
				if (temp_map[y][x] == 10)//与蛇重合，重新生成
					continue;
				else
					break;
			}
		}
		if (type != 4)//其他食物
		{
			while (1)
			{
				x = rand() % (MapW - 4) + 2;//该范围保证不会生成在地图边界
				y = rand() % (MapH - 4) + 2;//该范围保证不会生成在地图边界
				if (temp_map[y][x] == 10)//与蛇重合，重新生成
					continue;
				else
					break;
			}
		}
		food.Foodx = x;
		food.Foody = y;
		map[food.Foody][food.Foodx] = type;//显示食物
		if (FoodState != 0)
			FoodState++;//食物创建成功，且foodstate不为0时foodstate++
	}
}




bool gameIsOver(bool IsOver) {                                                       //还要用到 actAfterStop() 函数 暂停后的操作 结束游戏返回0
	if (IsOver) {
		// 游戏结束逻辑
		return true;
	}
	return false;
}

int getRankList() {
	vector<int> ranks;
	int rankIndex = -1;// 排名数初始化为-1表示排名未进入前十，进入前十会更新
	// 从文档中读取排名数据
	ifstream file("Rank.txt");
	if (file.is_open()) {
		int score;
		while (file >> score) {
			ranks.push_back(score);
		}
		file.close();
	}

	// 将当前分数加入排名数组
	ranks.push_back(score);

	// 对数组进行排序
	sort(ranks.begin(), ranks.end(), greater<int>());

	// 返回当前分数的排名
	for (size_t i = 0; i < ranks.size(); ++i) {
		if (ranks[i] == score && i <= 9) {
			rankIndex = i + 1; // 返回排名（从1开始）
		}
	}

	// 如果当前分数进入排行榜，更新文件
	if (rankIndex <= 10) {
		ofstream outputFile("Rank.txt");
		if (outputFile.is_open()) {
			for (int i = 0; i < 10 && i < ranks.size(); i++) {
				outputFile << ranks[i] << endl;
			}
			outputFile.close();
		}
	}

	return rankIndex; // 如果未进入前十，则返回初始值-1，若进入前十返回对应排名
}


// 设置游戏暂停状态的函数
void setPaused(bool paused) {
	isGameRunning = !paused;
}
/*
s---------------------------------------------------------------------------------------
*/

/*
v---------------------------------------------------------------------------------------
*/
void menuView()
{
	// 初始化图形窗口（仅一次）
	initgraph(640, 480, SWP_HIDEWINDOW);
	HWND hwndConsole = GetConsoleWindow();
	HWND hwndGraphics = GetForegroundWindow();
	HWND hwnd = GetForegroundWindow();
	SetWindowPos(hwndGraphics, HWND_TOPMOST, 400, 200, 0, 0, SWP_NOSIZE);
	loadGameResource();


	// 启用双缓冲绘图模式
	BeginBatchDraw();

	// 按钮坐标定义
	const RECT btnStart = { 220, 75, 420, 125 };
	const RECT btnRank = { 220, 145, 420, 195 };
	const RECT btnSettings = { 220, 215, 420, 265 };
	const RECT btnGameIntro = { 220, 285, 420, 335 };
	const RECT btnExit = {220, 355, 420, 405};
	//playBackMusic(musicOfAll);

	while (true) {

		cleardevice();
		putimage(0, 0, &BackGound);
		// ---- 绘制界面 ----
		setbkcolor(HSVtoRGB(160, 0.6, 1));

		// 绘制标题
		settextcolor(RGB(253, 255, 115));
		settextstyle(48, 0, ("宋体"), 0, 0, 600, 0, 1, 0);
		outtextxy(200, 30, ("贪吃蛇游戏"));
		// ---- 消息处理 ----


		ExMessage msg = getmessage();
		settextstyle(48, 0, "宋体", 0, 0, 300, 0, 0, 0);
		// 绘制按钮
		DrawButton(btnStart, GREEN, ("开始游戏"), checkIsHover(msg.x, msg.y, btnStart));
		DrawButton(btnRank, CYAN, ("排行榜"), checkIsHover(msg.x, msg.y, btnRank));
		DrawButton(btnSettings, YELLOW, ("游戏设置"), checkIsHover(msg.x, msg.y, btnSettings));
		DrawButton(btnGameIntro, RGB(230, 230, 250), "游戏说明", checkIsHover(msg.x, msg.y, btnGameIntro));
		DrawButton(btnExit, RED, ("退出游戏"), checkIsHover(msg.x, msg.y, btnExit));
		

		FlushBatchDraw();
		if (msg.message == WM_LBUTTONDOWN) {
			if (CheckButtonClick(msg.x, msg.y, btnStart)) {

				closegraph();
				modeSelectionView();

				return; // 直接退出当前函数
			}
			else if (CheckButtonClick(msg.x, msg.y, btnRank)) {

				closegraph();
				rankListView();
				return;
			}
			else if (CheckButtonClick(msg.x, msg.y, btnSettings)) {

				closegraph();
				settingView();
				return;
			}
			else if (CheckButtonClick(msg.x,msg.y, btnGameIntro))
			{
				closegraph();
				gameIntroView();
				return;
			}
			else if (CheckButtonClick(msg.x, msg.y, btnExit)) {

				closegraph();
				exit(0);
			}
		}


		// 适当延迟，降低CPU占用
		Sleep(1);
	}

	// 双缓冲
	EndBatchDraw();
}

void settingView()
{
	while (true) {
		
		initgraph(640, 480, SWP_HIDEWINDOW);
		loadGameResource();
		bool shouldExit = false;
		HWND hwndConsole = GetConsoleWindow();
		HWND hwndGraphics = GetForegroundWindow();
		SetWindowPos(hwndGraphics, HWND_TOPMOST, 400, 200, 0, 0, SWP_NOSIZE);
		bool isHover = FALSE;
		// 按钮定义
		const RECT btnRestore = { 220, 50, 420, 100 };
		const RECT btnSkin = { 220, 120, 420, 170 };
		const RECT btnCustom = { 220, 190, 420, 240 };
		const RECT btnMusic = { 220, 260, 420, 310 };
		const RECT btnSoundEffect = { 220, 330, 420, 380 };
		const RECT btnBack = { 220, 380, 420, 450 };
		ToggleSwitch soundSwitch =
		{
			true,
			380, 330, 60,
			RGB(0, 120, 215),
			RGB(150, 150, 150)
		};
		ToggleSwitch musicSwitch =
		{
			true,
			380, 280, 60,
			RGB(0, 120, 215),
			RGB(150, 150, 150)
		};

		while (!shouldExit) {
			ExMessage msg = getmessage();
			if (msg.message == WM_LBUTTONDOWN) {
				if (CheckButtonClick(msg.x, msg.y, btnRestore)) {
					restoreDefailtSkin();

				}
				else if (CheckButtonClick(msg.x, msg.y, btnSkin)) {
					closegraph();
					skinDefineView();

				}
				else if (CheckButtonClick(msg.x, msg.y, btnCustom)) {
					closegraph();
					userDefineView();
					shouldExit = true;

				}
				else if (checkToggleClick(soundSwitch, msg.x, msg.y)) {
					soundSwitch.isOn = !soundSwitch.isOn;
					soundEffect = !soundEffect;
					isPressedSound = !isPressedSound;
				}
				else if (checkToggleClick(musicSwitch, msg.x, msg.y)) {
					musicSwitch.isOn = !musicSwitch.isOn;
					musicOfAll = !musicOfAll;
					isPressedMusic = !isPressedMusic;

				}
				else if (CheckButtonClick(msg.x, msg.y, btnBack)) {
					closegraph();
					shouldExit = true;

					menuView();

				}

			}

			BeginBatchDraw();
			// 绘制界面
			cleardevice();
			setbkcolor(HSVtoRGB(200, 0.3, 1));

			// 绘制标题
			settextcolor(BLACK);
			settextstyle(32, 0, ("宋体"));
			outtextxy(240, 10, ("游戏设置"));
			putimage(0, 0, &BackGound);
			// 绘制按钮
			DrawButton(btnRestore, LIGHTGRAY, ("恢复默认"), checkIsHover(msg.x, msg.y, btnRestore));
			DrawButton(btnSkin, LIGHTMAGENTA, ("皮肤设置"), checkIsHover(msg.x, msg.y, btnSkin));
			DrawButton(btnCustom, LIGHTCYAN, ("自定义设置"), checkIsHover(msg.x, msg.y, btnCustom));
			DrawButton(btnBack, RED, ("返回菜单"), checkIsHover(msg.x, msg.y, btnBack));
			drawTextLabel(("音效开关："), 250, soundSwitch.posY);
			drawTextLabel(("背景音乐："), 250, musicSwitch.posY);
			drawToggleSwitch(musicSwitch, isPressedMusic);
			drawToggleSwitch(soundSwitch, isPressedSound);
			EndBatchDraw();
			FlushBatchDraw();
		}
	}

}

void modeSelectionView()
{

	while (true) {
		initgraph(640, 480, SWP_HIDEWINDOW);
		loadGameResource();
		HWND hwndConsole = GetConsoleWindow();
		HWND hwndGraphics = GetForegroundWindow();
		SetWindowPos(hwndGraphics, HWND_TOPMOST, 400, 200, 0, 0, SWP_NOSIZE);
		bool shouldExit = false;
		bool isHover = false;
		// 按钮定义
		const RECT btnNormal = { 220, 100, 420, 150 };
		const RECT btnCustom = { 220, 170, 420, 220 };
		const RECT btnBack = { 220, 240, 420, 290 };

		while (!shouldExit) {
			ExMessage msg = getmessage();

			if (msg.message == WM_LBUTTONDOWN) {
				if (CheckButtonClick(msg.x, msg.y, btnNormal)) {
					closegraph();
					difficultySelectionView();
					shouldExit = true;
					isHover = true;
				}
				else if (CheckButtonClick(msg.x, msg.y, btnCustom)) {
					closegraph();
					userDefineView();
					shouldExit = true;
					isHover = true;
				}
				else if (CheckButtonClick(msg.x, msg.y, btnBack)) {
					closegraph();
					shouldExit = true;
					isHover = true;
					menuView();

				}
			}


			// 绘制界面
			cleardevice();
			putimage(0, 0, &BackGound);
			setbkcolor(HSVtoRGB(120, 0.4, 1));

			// 绘制标题
			settextcolor(YELLOW);
			settextstyle(48, 0, ("宋体"), 0, 0, 500, 0, 1, 0);
			outtextxy(240, 30, ("选择模式"));

			settextstyle(30, 0, ("宋体"), 0, 0, 500, 0, 0, 0);

			// 绘制按钮
			DrawButton(btnNormal, GREEN, ("普通模式"), checkIsHover(msg.x, msg.y, btnNormal));
			DrawButton(btnCustom, BLUE, ("自定义模式"), checkIsHover(msg.x, msg.y, btnCustom));
			DrawButton(btnBack, RED, ("返回菜单"), checkIsHover(msg.x, msg.y, btnBack));

			FlushBatchDraw();
		}
	}

}

void difficultySelectionView()
{
	initgraph(640, 480, SWP_HIDEWINDOW);
	HWND hwndGraphics = GetForegroundWindow();
	SetWindowPos(hwndGraphics, HWND_TOPMOST, 400, 200, 0, 0, SWP_NOSIZE);
	loadGameResource();
	setbkcolor(YELLOW);
	cleardevice();
	BeginBatchDraw();
	putimage(0, 0, &BackGound);
	int w = 200, h = 100;
	setfillcolor(GREEN);
	solidrectangle(220, 135, 220 + w, 135 + h);
	solidrectangle(0, 0, 85, 60);
	solidrectangle(220, 245, 220 + w, 245 + h);

	settextstyle(36, 0, "微软雅黑");
	settextcolor(RED);
	setbkmode(TRANSPARENT);
	outtextxy(220 + (w - textwidth("初始速度设置")) / 2, 135 + (h - textheight("初始速度设置")) / 2, "初始速度设置");
	outtextxy(220 + (w - textwidth("开始游戏")) / 2, 245 + (h - textheight("开始游戏")) / 2, "开始游戏");
	outtextxy(0 + (85 - textwidth("退出")) / 2, 0 + (60 - textheight("退出")) / 2, "退出");
	EndBatchDraw();

	while (1) {
		if (peekmessage(&msg, EX_MOUSE)) {
			if (msg.x > 220 && msg.x < 420 && msg.y>135 && msg.y < 235) {
				setlinecolor(0x7EE07E);
				setlinestyle(PS_SOLID, 3);
				rectangle(220, 135, 220 + w, 135 + h);

				if (msg.message == WM_LBUTTONDOWN) {
					closegraph();
					speedDefinedView();
					return;
				}
			}
			else {
				setlinecolor(GREEN);
				setlinestyle(PS_SOLID, 3);
				rectangle(220, 135, 220 + w, 135 + h);
			}
			if (msg.x > 220 && msg.x < 420 && msg.y>245 && msg.y < 345) {
				setlinecolor(0x7EE07E);
				setlinestyle(PS_SOLID, 3);
				rectangle(220, 245, 220 + w, 245 + h);

				if (msg.message == WM_LBUTTONDOWN) {
					closegraph();
					initialGame();
					gameView();
					return;
				}
			}
			else {
				setlinecolor(GREEN);
				setlinestyle(PS_SOLID, 3);
				rectangle(220, 245, 220 + w, 245 + h);
			}
			if (msg.x > 0 && msg.x < 85 && msg.y>0 && msg.y < 60) {
				setlinecolor(0x7EE07E);
				setlinestyle(PS_SOLID, 3);
				rectangle(0, 0, 85, 60);

				if (msg.message == WM_LBUTTONDOWN) {
					closegraph();
					menuView();
					return;
				}
			}
			else {
				setlinecolor(GREEN);
				setlinestyle(PS_SOLID, 3);
				rectangle(0, 0, 85, 60);
			}
		}

	}

	FlushBatchDraw();

}

void userDefineView()
{
	initgraph(640, 480, SWP_HIDEWINDOW);
	HWND hwndGraphics = GetForegroundWindow();
	SetWindowPos(hwndGraphics, HWND_TOPMOST, 400, 200, 0, 0, SWP_NOSIZE);
	setbkcolor(YELLOW);
	cleardevice();
	BeginBatchDraw();
	loadGameResource();
	putimage(0, 0, &BackGound);
	int w = 200, h = 100;
	setfillcolor(GREEN);
	solidrectangle(240, 25, 440, 125);
	solidrectangle(0, 0, 85, 60);
	solidrectangle(240, 135, 240 + w, 135 + h);
	solidrectangle(240, 245, 240 + w, 245 + h);
	solidrectangle(240, 355, 240 + w, 355 + h);

	settextstyle(36, 0, "微软雅黑");
	settextcolor(RED);
	setbkmode(TRANSPARENT);
	outtextxy(240 + (w - textwidth("初始速度设置")) / 2, 25 + (h - textheight("初始速度设置")) / 2, "初始速度设置");
	outtextxy(240 + (w - textwidth("开始游戏")) / 2, 135 + (h - textheight("开始游戏")) / 2, "开始游戏");
	outtextxy(0 + (85 - textwidth("退出")) / 2, 0 + (60 - textheight("退出")) / 2, "退出");
	outtextxy(240 + (w - textwidth("初始长度设置")) / 2, 245 + (h - textheight("初始长度设置")) / 2, "初始长度设置");
	outtextxy(240 + (w - textwidth("地图设置")) / 2, 355 + (h - textheight("地图设置")) / 2, "地图设置");
	EndBatchDraw();
	while (1) {
		if (peekmessage(&msg, EX_MOUSE)) {
			if (msg.x > 240 && msg.x < 440 && msg.y>25 && msg.y < 125) {
				setlinecolor(0x7EE07E);
				setlinestyle(PS_SOLID, 3);
				rectangle(240, 25, 440, 125);

				if (msg.message == WM_LBUTTONDOWN) {
					closegraph();
					speedDefinedView();
				}
			}
			else {
				setlinestyle(PS_SOLID, 3);
				setlinecolor(GREEN);
				rectangle(240, 25, 440, 125);
			}
			if (msg.x > 240 && msg.x < 440 && msg.y>135 && msg.y < 235) {
				setlinecolor(0x7EE07E);
				setlinestyle(PS_SOLID, 3);
				rectangle(240, 135, 240 + w, 135 + h);

				if (msg.message == WM_LBUTTONDOWN) {
					closegraph();
					initialGame();
					gameView();
				}
			}
			else {
				setlinecolor(GREEN);
				setlinestyle(PS_SOLID, 3);
				rectangle(240, 135, 240 + w, 135 + h);
			}
			if (msg.x > 0 && msg.x < 85 && msg.y>0 && msg.y < 60) {
				setlinecolor(0x7EE07E);
				setlinestyle(PS_SOLID, 3);
				rectangle(0, 0, 85, 60);

				if (msg.message == WM_LBUTTONDOWN) {
					closegraph();
					menuView();
				}
			}
			else {
				setlinecolor(GREEN);
				setlinestyle(PS_SOLID, 3);
				rectangle(0, 0, 85, 60);
			}
			if (msg.x > 240 && msg.x < 440 && msg.y>245 && msg.y < 345) {
				setlinecolor(0x7EE07E);
				setlinestyle(PS_SOLID, 3);
				rectangle(240, 245, 240 + w, 245 + h);

				if (msg.message == WM_LBUTTONDOWN) {
					closegraph();
					longDefineView();
				}
			}
			else {
				setlinecolor(GREEN);
				setlinestyle(PS_SOLID, 3);
				rectangle(240, 245, 240 + w, 245 + h);
			}
			if (msg.x > 240 && msg.x < 440 && msg.y>355 && msg.y < 455) {
				setlinecolor(0x7EE07E);
				setlinestyle(PS_SOLID, 3);
				rectangle(240, 355, 240 + w, 355 + h);

				if (msg.message == WM_LBUTTONDOWN) {
					closegraph();
					mapDefineView();
				}
			}
			else {
				setlinecolor(GREEN);
				setlinestyle(PS_SOLID, 3);
				rectangle(240, 355, 240 + w, 355 + h);
			}

		}
		FlushBatchDraw();
	}



}

void speedDefinedView()
{
	/*初始速度设置界面
	负责人：7

	简要说明：展示初始速度设置界面，包含按钮（速度1、速度2、速度3、确认）、背景

	按下对应按钮更改速度
	initgraph
	while(1){
		按下对应按钮更改速度  void selectSnakeSpeed();
		按下确认return,closegraph返回自定义界面
	}


	传入类型：void
	返回值：void
*/
	int Graphx = 640, Graphy = 480;
	initgraph(Graphx, Graphy, 0);
	HWND hwndConsole = GetForegroundWindow();
	HWND hwndGraphics = GetForegroundWindow();
	HWND hwnd = GetForegroundWindow();
	SetWindowPos(hwndGraphics, HWND_TOPMOST, 400, 200, 0, 0, SWP_NOSIZE);
	cleardevice();
	loadGameResource();
	putimage(0, 0, &BackGound);
	int TextHOfti = 40;

	settextstyle(TextHOfti, 0, "宋体");
	settextcolor(BLACK);
	setbkmode(TRANSPARENT);
	outtextxy((Graphx - textwidth("初始速度设置界面")) / 2, 10, ("初始速度设置界面"));

	/*按钮*/
	int BonH = 100, BonW = 150;
	int TextHOfBo = 20;
	while (1) {
		MOUX = 0;
		MOUY = 0;
		while (peekmessage(&msg, EX_MOUSE))
		{
			if (msg.message == WM_LBUTTONDOWN)
			{
				MOUX = msg.x;
				MOUY = msg.y;
			}
		}


		setfillcolor(YELLOW);

		if ((msg.x > (Graphx - BonW) / 2) && (msg.x < ((Graphx - BonW) / 2 + BonW)) && (msg.y > 100) && (msg.y < 100 + BonH)) {
			setfillcolor(GREEN);
			if (msg.message == WM_LBUTTONDOWN)
			{
				setfillcolor(WHITE);
			}
			if (msg.message == WM_LBUTTONUP) {
				setfillcolor(YELLOW);
			}
		}


		BeginBatchDraw();
		fillrectangle((Graphx - BonW) / 2, 100, (Graphx - BonW) / 2 + BonW, 100 + BonH);
		int TextHOftext = 20;
		settextstyle(TextHOftext, 0, "宋体");
		outtextxy((Graphx - BonW / 2) / 2, 140, ("速度1"));


		setfillcolor(YELLOW);
		EndBatchDraw();
		if ((msg.x > (Graphx - BonW) / 2) && (msg.x < ((Graphx - BonW) / 2 + BonW)) && (msg.y > 260) && (msg.y < 260 + BonH)) {
			setfillcolor(GREEN);
			if (msg.message == WM_LBUTTONDOWN)
			{
				setfillcolor(WHITE);
			}
			if (msg.message == WM_LBUTTONUP) {
				setfillcolor(YELLOW);
			}
		}
		BeginBatchDraw();
		fillrectangle((Graphx - BonW) / 2, 260, (Graphx - BonW) / 2 + BonW, 260 + BonH);
		settextstyle(TextHOftext, 0, "宋体");
		outtextxy((Graphx - BonW / 2) / 2, 300, ("速度2"));

		setfillcolor(YELLOW);
		EndBatchDraw();
		if ((msg.x > (Graphx - BonW) / 2) && (msg.x < ((Graphx - BonW) / 2 + BonW)) && (msg.y > 420) && (msg.y < 420 + BonH)) {
			setfillcolor(GREEN);
			if (msg.message == WM_LBUTTONDOWN)
			{
				setfillcolor(WHITE);
			}
			if (msg.message == WM_LBUTTONUP) {
				setfillcolor(YELLOW);
			}
		}
		BeginBatchDraw();
		fillrectangle((Graphx - BonW) / 2, 420, (Graphx - BonW) / 2 + BonW, 420 + BonH);
		settextstyle(TextHOftext, 0, "宋体");
		outtextxy((Graphx - BonW / 2) / 2, 460, ("速度3"));
		EndBatchDraw();


		BeginBatchDraw();
		setfillcolor(RED);
		fillrectangle(20, 20, 20 + 30, 20 + 20);
		settextstyle(15, 0, "宋体");
		outtextxy(20, 25, ("返回"));
		EndBatchDraw();


		if ((MOUX > (Graphx - BonW) / 2) && (MOUX < ((Graphx - BonW) / 2 + BonW)) && (MOUY > 100) && (MOUY < 100 + BonH)) {
			selectSnakeSpeed(1);
		}
		else  if ((MOUX > (Graphx - BonW) / 2) && (MOUX < ((Graphx - BonW) / 2 + BonW)) && (MOUY > 260) && (MOUY < 260 + BonH)) {
			selectSnakeSpeed(2);
		}

		else  if ((MOUX > (Graphx - BonW) / 2) && (MOUX < ((Graphx - BonW) / 2 + BonW)) && (MOUY > 420) && (MOUY < 420 + BonH)) {
			selectSnakeSpeed(3);
		}
		else  if ((MOUX > 20) && (MOUX < 50) && (MOUY > 20) && (MOUY < 40)) {
			closegraph();
			userDefineView();

		}
	}

}



void longDefineView()
{/*
	初始长度设置界面
		负责人：7

		简要说明：展示初始长度设置界面，包含按钮（长度1、长度2、长度3、确认）、背景

		initgraph

		while (1) {
			按下对应按钮更改长度 void selectSnakeLong();
			按下确认return, closegraph返回自定义界面
		}

	传入类型：void
		返回值：void*/
	int Graphx = 640, Graphy = 480;
	initgraph(Graphx, Graphy, 0);
	HWND hwndConsole = GetForegroundWindow();
	HWND hwndGraphics = GetForegroundWindow();
	HWND hwnd = GetForegroundWindow();
	SetWindowPos(hwndGraphics, HWND_TOPMOST, 400, 200, 0, 0, SWP_NOSIZE);
	cleardevice();
	loadGameResource();
	putimage(0, 0, &BackGound);
	int TextHOfti = 40;

	settextstyle(TextHOfti, 0, "宋体");
	settextcolor(BLACK);
	setbkmode(TRANSPARENT);
	outtextxy((Graphx - textwidth("初始长度设置界面")) / 2, 10, ("初始长度设置界面"));

	/*按钮*/
	int BonH = 100, BonW = 150;
	int TextHOfBo = 20;
	while (1) {
		MOUX = 0;
		MOUY = 0;
		while (peekmessage(&msg, EX_MOUSE))
		{
			if (msg.message == WM_LBUTTONDOWN)
			{
				MOUX = msg.x;
				MOUY = msg.y;
			}
		}


		setfillcolor(YELLOW);

		if ((msg.x > (Graphx - BonW) / 2) && (msg.x < ((Graphx - BonW) / 2 + BonW)) && (msg.y > 100) && (msg.y < 100 + BonH)) {
			setfillcolor(GREEN);
			if (msg.message == WM_LBUTTONDOWN)
			{
				setfillcolor(WHITE);
			}
			if (msg.message == WM_LBUTTONUP) {
				setfillcolor(YELLOW);
			}
		}


		BeginBatchDraw();
		fillrectangle((Graphx - BonW) / 2, 100, (Graphx - BonW) / 2 + BonW, 100 + BonH);
		int TextHOftext = 20;
		settextstyle(TextHOftext, 0, "宋体");
		outtextxy((Graphx - BonW / 2) / 2, 140, ("长度1"));
		EndBatchDraw();

		setfillcolor(YELLOW);

		if ((msg.x > (Graphx - BonW) / 2) && (msg.x < ((Graphx - BonW) / 2 + BonW)) && (msg.y > 260) && (msg.y < 260 + BonH)) {
			setfillcolor(GREEN);
			if (msg.message == WM_LBUTTONDOWN)
			{
				setfillcolor(WHITE);
			}
			if (msg.message == WM_LBUTTONUP) {
				setfillcolor(YELLOW);
			}
		}
		BeginBatchDraw();
		fillrectangle((Graphx - BonW) / 2, 260, (Graphx - BonW) / 2 + BonW, 260 + BonH);
		settextstyle(TextHOftext, 0, "宋体");
		outtextxy((Graphx - BonW / 2) / 2, 300, ("长度2"));
		EndBatchDraw();
		setfillcolor(YELLOW);

		if ((msg.x > (Graphx - BonW) / 2) && (msg.x < ((Graphx - BonW) / 2 + BonW)) && (msg.y > 420) && (msg.y < 420 + BonH)) {
			setfillcolor(GREEN);
			if (msg.message == WM_LBUTTONDOWN)
			{
				setfillcolor(WHITE);
			}
			if (msg.message == WM_LBUTTONUP) {
				setfillcolor(YELLOW);
			}
		}
		BeginBatchDraw();
		fillrectangle((Graphx - BonW) / 2, 420, (Graphx - BonW) / 2 + BonW, 420 + BonH);
		settextstyle(TextHOftext, 0, "宋体");
		outtextxy((Graphx - BonW / 2) / 2, 460, ("长度3"));
		EndBatchDraw();


		BeginBatchDraw();
		setfillcolor(RED);
		fillrectangle(20, 20, 20 + 30, 20 + 20);
		settextstyle(15, 0, "宋体");
		outtextxy(20, 25, ("返回"));
		EndBatchDraw();


		if ((MOUX > (Graphx - BonW) / 2) && (MOUX < ((Graphx - BonW) / 2 + BonW)) && (MOUY > 100) && (MOUY < 100 + BonH)) {
			selectSnakeLong(1);
		}
		else  if ((MOUX > (Graphx - BonW) / 2) && (MOUX < ((Graphx - BonW) / 2 + BonW)) && (MOUY > 260) && (MOUY < 260 + BonH)) {
			selectSnakeLong(2);
		}

		else  if ((MOUX > (Graphx - BonW) / 2) && (MOUX < ((Graphx - BonW) / 2 + BonW)) && (MOUY > 420) && (MOUY < 420 + BonH)) {
			selectSnakeLong(3);
		}
		else  if ((MOUX > 20) && (MOUX < 50) && (MOUY > 20) && (MOUY < 40)) {
			closegraph();
			userDefineView();
		}
	}

}

void mapDefineView()
{
	/* 地图设置界面*
	负责人：7

	简要说明：展示地图设置设置界面,选择地图大小1、大小2、大小3

	initgraph
   while(1){

	根据对应数据更改地图长宽 void changeMapHW();
	按下确认return,closegraph返回自定义界面

	}

	传入类型：void
	返回值：void
*/
	int Graphx = 640, Graphy = 480;
	initgraph(Graphx, Graphy, 0);
	HWND hwndConsole = GetForegroundWindow();
	HWND hwndGraphics = GetForegroundWindow();
	HWND hwnd = GetForegroundWindow();
	SetWindowPos(hwndGraphics, HWND_TOPMOST, 400, 200, 0, 0, SWP_NOSIZE);
	cleardevice();
	loadGameResource();
	putimage(0, 0, &BackGound);
	int TextHOfti = 40;

	settextstyle(TextHOfti, 0, "宋体");
	settextcolor(BLACK);
	setbkmode(TRANSPARENT);
	outtextxy((Graphx - textwidth("初始地图设置界面")) / 2, 10, ("初始地图设置界面"));

	/*按钮*/
	int BonH = 100, BonW = 150;
	int TextHOfBo = 20;
	while (1) {
		MOUX = 0;
		MOUY = 0;
		while (peekmessage(&msg, EX_MOUSE))
		{
			if (msg.message == WM_LBUTTONDOWN)
			{
				MOUX = msg.x;
				MOUY = msg.y;
			}
		}


		setfillcolor(YELLOW);

		if ((msg.x > (Graphx - BonW) / 2) && (msg.x < ((Graphx - BonW) / 2 + BonW)) && (msg.y > 100) && (msg.y < 100 + BonH)) {
			setfillcolor(GREEN);
			if (msg.message == WM_LBUTTONDOWN)
			{
				setfillcolor(WHITE);
			}
			if (msg.message == WM_LBUTTONUP) {
				setfillcolor(YELLOW);
			}
		}


		BeginBatchDraw();
		fillrectangle((Graphx - BonW) / 2, 100, (Graphx - BonW) / 2 + BonW, 100 + BonH);
		int TextHOftext = 20;
		settextstyle(TextHOftext, 0, "宋体");
		outtextxy((Graphx - BonW / 2) / 2, 140, ("地图1"));
		EndBatchDraw();

		setfillcolor(YELLOW);

		if ((msg.x > (Graphx - BonW) / 2) && (msg.x < ((Graphx - BonW) / 2 + BonW)) && (msg.y > 260) && (msg.y < 260 + BonH)) {
			setfillcolor(GREEN);
			if (msg.message == WM_LBUTTONDOWN)
			{
				setfillcolor(WHITE);
			}
			if (msg.message == WM_LBUTTONUP) {
				setfillcolor(YELLOW);
			}
		}
		BeginBatchDraw();
		fillrectangle((Graphx - BonW) / 2, 260, (Graphx - BonW) / 2 + BonW, 260 + BonH);
		settextstyle(TextHOftext, 0, "宋体");
		outtextxy((Graphx - BonW / 2) / 2, 300, ("地图2"));
		EndBatchDraw();
		setfillcolor(YELLOW);

		if ((msg.x > (Graphx - BonW) / 2) && (msg.x < ((Graphx - BonW) / 2 + BonW)) && (msg.y > 420) && (msg.y < 420 + BonH)) {
			setfillcolor(GREEN);
			if (msg.message == WM_LBUTTONDOWN)
			{
				setfillcolor(WHITE);
			}
			if (msg.message == WM_LBUTTONUP) {
				setfillcolor(YELLOW);
			}
		}
		BeginBatchDraw();
		fillrectangle((Graphx - BonW) / 2, 420, (Graphx - BonW) / 2 + BonW, 420 + BonH);
		settextstyle(TextHOftext, 0, "宋体");
		outtextxy((Graphx - BonW / 2) / 2, 460, ("地图3"));
		EndBatchDraw();


		BeginBatchDraw();
		setfillcolor(RED);
		fillrectangle(20, 20, 20 + 30, 20 + 20);
		settextstyle(15, 0, "宋体");
		outtextxy(20, 25, ("返回"));
		EndBatchDraw();


		if ((MOUX > (Graphx - BonW) / 2) && (MOUX < ((Graphx - BonW) / 2 + BonW)) && (MOUY > 100) && (MOUY < 100 + BonH)) {
			temp_MapH = 20;
			temp_MapW = 20;
		}
		else  if ((MOUX > (Graphx - BonW) / 2) && (MOUX < ((Graphx - BonW) / 2 + BonW)) && (MOUY > 260) && (MOUY < 260 + BonH)) {
			temp_MapH = 60;
			temp_MapW = 60;
		}

		else  if ((MOUX > (Graphx - BonW) / 2) && (MOUX < ((Graphx - BonW) / 2 + BonW)) && (MOUY > 420) && (MOUY < 420 + BonH)) {
			temp_MapH = 100;
			temp_MapW = 100;
		}
		else  if ((MOUX > 20) && (MOUX < 50) && (MOUY > 20) && (MOUY < 40)) {
			closegraph();
			userDefineView();
		}
	}

}




void skinDefineView()
{
	/*皮肤设置界面
	负责人：7

	简要说明：展示地图设置设置界面，包含按钮（皮肤1(图片按钮)、皮肤2（图片按钮）、确认）、背景
   initgraph
 while(1){
	按下对应按钮更改皮肤 void selectSkin();
	按下确认return,closegraph返回自定义界面



	}

	 传入类型：void
	返回值：void
*/
	int Graphx = 640, Graphy = 480;
	initgraph(Graphx, Graphy, 0);
	HWND hwndConsole = GetForegroundWindow();
	HWND hwndGraphics = GetForegroundWindow();
	HWND hwnd = GetForegroundWindow();
	SetWindowPos(hwndGraphics, HWND_TOPMOST, 400, 200, 0, 0, SWP_NOSIZE);
	cleardevice();
	loadGameResource();
	putimage(0, 0, &BackGound);
	int TextHOfti = 40;

	settextstyle(TextHOfti, 0, "宋体");
	settextcolor(BLACK);
	setbkmode(TRANSPARENT);
	outtextxy((Graphx - textwidth("皮肤设置界面")) / 2, 10, ("皮肤设置界面"));

	/*按钮*/
	int BonH = 100, BonW = 150;
	int TextHOfBo = 20;
	while (1) {
		MOUX = 0;
		MOUY = 0;
		while (peekmessage(&msg, EX_MOUSE))
		{
			if (msg.message == WM_LBUTTONDOWN)
			{
				MOUX = msg.x;
				MOUY = msg.y;
			}
		}


		setfillcolor(YELLOW);

		if ((msg.x > (Graphx - BonW) / 2) && (msg.x < ((Graphx - BonW) / 2 + BonW)) && (msg.y > 100) && (msg.y < 100 + BonH)) {
			setfillcolor(GREEN);
			if (msg.message == WM_LBUTTONDOWN)
			{
				setfillcolor(WHITE);
			}
			if (msg.message == WM_LBUTTONUP) {
				setfillcolor(YELLOW);
			}
		}


		BeginBatchDraw();
		fillrectangle((Graphx - BonW) / 2, 100, (Graphx - BonW) / 2 + BonW, 100 + BonH);
		int TextHOftext = 20;
		settextstyle(TextHOftext, 0, "宋体");
		outtextxy((Graphx - BonW / 2) / 2, 140, ("皮肤1"));
		EndBatchDraw();

		setfillcolor(YELLOW);

		if ((msg.x > (Graphx - BonW) / 2) && (msg.x < ((Graphx - BonW) / 2 + BonW)) && (msg.y > 260) && (msg.y < 260 + BonH)) {
			setfillcolor(GREEN);
			if (msg.message == WM_LBUTTONDOWN)
			{
				setfillcolor(WHITE);
			}
			if (msg.message == WM_LBUTTONUP) {
				setfillcolor(YELLOW);
			}
		}
		BeginBatchDraw();
		fillrectangle((Graphx - BonW) / 2, 260, (Graphx - BonW) / 2 + BonW, 260 + BonH);
		settextstyle(TextHOftext, 0, "宋体");
		outtextxy((Graphx - BonW / 2) / 2, 300, ("皮肤2"));
		EndBatchDraw();
		setfillcolor(YELLOW);

		if ((msg.x > (Graphx - BonW) / 2) && (msg.x < ((Graphx - BonW) / 2 + BonW)) && (msg.y > 420) && (msg.y < 420 + BonH)) {
			setfillcolor(GREEN);
			if (msg.message == WM_LBUTTONDOWN)
			{
				setfillcolor(WHITE);
			}
			if (msg.message == WM_LBUTTONUP) {
				setfillcolor(YELLOW);
			}
		}
		BeginBatchDraw();
		fillrectangle((Graphx - BonW) / 2, 420, (Graphx - BonW) / 2 + BonW, 420 + BonH);
		settextstyle(TextHOftext, 0, "宋体");
		outtextxy((Graphx - BonW / 2) / 2, 460, ("皮肤3"));
		EndBatchDraw();


		BeginBatchDraw();
		setfillcolor(RED);
		fillrectangle(20, 20, 20 + 30, 20 + 20);
		settextstyle(15, 0, "宋体");
		outtextxy(20, 25, ("返回"));
		EndBatchDraw();


		if ((MOUX > (Graphx - BonW) / 2) && (MOUX < ((Graphx - BonW) / 2 + BonW)) && (MOUY > 100) && (MOUY < 100 + BonH)) {
			selectSkin(1);
		}
		else  if ((MOUX > (Graphx - BonW) / 2) && (MOUX < ((Graphx - BonW) / 2 + BonW)) && (MOUY > 260) && (MOUY < 260 + BonH)) {
			selectSkin(2);
		}

		else  if ((MOUX > (Graphx - BonW) / 2) && (MOUX < ((Graphx - BonW) / 2 + BonW)) && (MOUY > 420) && (MOUY < 420 + BonH)) {
			selectSkin(3);
		}
		else  if ((MOUX > 20) && (MOUX < 50) && (MOUY > 20) && (MOUY < 40)) {
			closegraph();
			settingView();
		}
	}


}


void fillRectangleArea(int startX, int startY, int endX, int endY, int WIDTH, int HEIGHT, int GRID_SIZE, bool isBody, int colorR, int colorG, int colorB) {
	// 计算每个网格的宽度和高度
	int gridWidth = WIDTH / GRID_SIZE;
	int gridHeight = HEIGHT / GRID_SIZE;

	// 计算矩形区域的实际坐标
	int realStartX = startX * gridWidth;
	int realStartY = startY * gridHeight;
	int realEndX = (endX + 1) * gridWidth;
	int realEndY = (endY + 1) * gridHeight;

	// 设置填充颜色
	if (isBody)
	{
		setfillcolor(RGB(colorR, colorG, colorB));
	}
	else
	{
		setfillcolor(RED);
	}

	// 填充矩形区域
	fillrectangle(realStartX, realStartY, realEndX, realEndY);
}

//游戏界面展示蛇的函数
void showSnake(int WIDTH, int HEIGHT, int GRID_SIZE) {


	// 设置背景颜色
	setbkcolor(RGB(0x7f, 0x8c, 0x8d));
	cleardevice();
	drawBase();



	// 绘制竖线
	for (int x = WIDTH / GRID_SIZE; x < WIDTH; x += WIDTH / GRID_SIZE) {

		line(x, HEIGHT / GRID_SIZE, x, HEIGHT - HEIGHT / GRID_SIZE);

	}
	// 绘制横线
	for (int y = HEIGHT / GRID_SIZE; y < HEIGHT; y += HEIGHT / GRID_SIZE) {
		
		line(WIDTH / GRID_SIZE, y, WIDTH - WIDTH / GRID_SIZE, y);
	}

	//绘制边界
	setfillcolor(RED);
	setlinecolor(RED);
	fillrectangle(0, 0, 600, 600 / GRID_SIZE);
	fillrectangle(0, 0, 600 / GRID_SIZE, 600);
	fillrectangle(600 - 600 / GRID_SIZE, 0, 600, 600);
	fillrectangle(0, 600 - 600 / GRID_SIZE, 600, 600);

	setlinecolor(WHITE);
	


	for (int i = 0; i < 400; i++)
	{
		if (Snake[i][0] != -1) {
			fillRectangleArea(Snake[i][0], Snake[i][1], Snake[i][0], Snake[i][1], WIDTH, HEIGHT, GRID_SIZE, true, colorR, colorG, colorB);
		}
		else
		{
			break;
		}
	}
	if (BigHead) {
		for (int i = 0; i < 10; i++) {
			if (SnakeHead[i][0] >= 0 && SnakeHead[i][0] <= MapH - 1 && SnakeHead[i][1] >= 0 && SnakeHead[i][1] <= MapH - 1)
			{
				fillRectangleArea(SnakeHead[i][0], SnakeHead[i][1], SnakeHead[i][0], SnakeHead[i][1], WIDTH, HEIGHT, GRID_SIZE, false, colorR, colorG, colorB);
			}

		}
	}
	else
	{
		fillRectangleArea(SnakeHead[0][0], SnakeHead[0][1], SnakeHead[0][0], SnakeHead[0][1], WIDTH, HEIGHT, GRID_SIZE, false, colorR, colorG, colorB);
	}


}


void drawBase() {
	settextstyle(16, 0, "微软雅思");
	//设置文字颜色
	settextcolor(YELLOW);
	//设置背景模式
	setbkmode(TRANSPARENT);

	//设置三行文本显示游戏数据
	char timeText[20] = "";
	sprintf(timeText, "游戏时长：%d:%d", GameTime / 60, GameTime % 60); // 使用 sprintf 将整数格式化为字符串
	outtextxy(600 / MapH, 600 / MapH, timeText);

	char scoreText[20] = "";
	sprintf(scoreText, "当前得分：%d", score);
	outtextxy(600 / MapH, 600 / MapH + 50, scoreText);


	char lengthText[20] = "";
	sprintf(lengthText, "蛇的长度：%d", SnakeLong);
	outtextxy(600 / MapH, 600 / MapH + 100, lengthText);

	//暂停按钮设置
	char stopButtonText[20] = "暂停";
	button(500 - 600 / MapH, 600 / MapH, 100, 50, stopButtonText);

	//控制键提示按钮
	char w[] = "W";
	char s[] = "S";
	char a[] = "A";
	char d[] = "D";

	button(50 + 600 / MapH, 500 - 600 / MapH, 50, 50, w);
	button(50 + 600 / MapH, 550 - 600 / MapH, 50, 50, s);
	button(600 / MapH, 550 - 600 / MapH, 50, 50, a);
	button(100 + 600 / MapH, 550 - 600 / MapH, 50, 50, d);

	char g[] = "G";
	char z[] = "Z";

	button(480 - 600 / MapH, 540 - 600 / MapH, 50, 50, g);
	button(550 - 600 / MapH, 540 - 600 / MapH, 50, 50, z);
}

void showFood() {
	if (food.Foodx == 0 && food.Foody == 0) return; // 无食物时不绘制

	int gridSize = 600 / MapH; // 与 showSnake 一致
	int foodX = food.Foodx * gridSize + gridSize / 2;
	int foodY = food.Foody * gridSize + gridSize / 2;

	switch (map[food.Foody][food.Foodx]) {
	case 2: // 普通食物
		setfillcolor(YELLOW);
		break;
	case 3: // 地图扩大食物
		setfillcolor(BLUE);
		break;
	case 4: // 头部增大食物
		setfillcolor(RED);
		break;
	case 5: // 吸附食物
		setfillcolor(GREEN);
		break;
	}
	fillcircle(foodX, foodY, gridSize / 2 - 2);
}



//加一个button函数
void gameView()
{
	loadGameResource();

	//设置背景颜色
	initgraph(600, 600);
	setbkcolor(RGB(0x7f, 0x8c, 0x8d));
	cleardevice();
	drawBase();
	
	if (musicOfAll)
	{
		mciSendString("play ../GameMaterials/backmusic.wav", 0, 0, 0);
	}
	
	// 开启批量绘图模式
	BeginBatchDraw();
	while (true) {

		cleardevice();
		drawBase();

		//蛇开始移动
		snakeMove();
		showSnake(600, 600, MapH);
		showFood();
		FlushBatchDraw();
		
		
		
		double spaceTime = 200 / SnakeSpeed;
		// 非阻塞式睡眠(让蛇以一定速度移动，同时不干扰其他功能)
		DWORD startTime = GetTickCount();
		while (GetTickCount() - startTime < spaceTime)
		{
			//时间累加
			gameTime();

			while (peekmessage(&msg, EM_MOUSE | EM_KEY)) {
				switch (msg.message) {
				case WM_KEYDOWN:
					switch (msg.vkcode) {
					case 'W':
						canChangeDir('W');
						break;
					case 'S':
						canChangeDir('S');
						break;
					case 'A':
						canChangeDir('A');
						break;
					case 'D':
						canChangeDir('D');
						break;
					case 'G':
					case 'g':
						speedUpOrNo();
						break;
					case 'Z':
					case 'z':
						actAfterStop();
						break;
					}
					break;
				case WM_LBUTTONDOWN:
					if (msg.x >= 500 && msg.x <= 600 && msg.y <= 50 && msg.y >= 0) {
						mciSendString("close ../GameMaterials/backmusic.wav", NULL, 0, NULL);
						actAfterStop();
					}
					break;
				}
			}
			
			IsDie_snake = IsDie();
			if (IsDie_snake)
			{
				mciSendString("close ../GameMaterials/backmusic.wav", NULL, 0, NULL);
				closegraph();
				gameOverView();
			}
		}

	}
	EndBatchDraw();

}

void stopView()
{
	initgraph(600, 480);
	setbkcolor(RGB(0x7f, 0x8c, 0x8d));
	cleardevice();

	//计时器暂停
	setPaused(true);

	//开启双缓冲绘图
	BeginBatchDraw();
	loadGameResource();
	putimage(0, 0, &BackGound);
	char returnButton[20] = "返回游戏";
	char closeButton[20] = "结束游戏";
	button(200,135,200,80,returnButton);
	button(200,265,200,80,closeButton);
	FlushBatchDraw();

	ExMessage mouMsg;
	while (true) {
		// 获取消息
		while (peekmessage(&mouMsg, EX_MOUSE)) {
			switch (mouMsg.message) {
			case WM_LBUTTONDOWN:
				if (mouMsg.x >= 200 && mouMsg.x <= 400 && mouMsg.y >= 135 && mouMsg.y <= 215) {
					closegraph();
					setPaused(false); // 重启计时器
					gameView();
					return;
				}
				else if (mouMsg.x >= 200 && mouMsg.x <= 400 && mouMsg.y >= 265 && mouMsg.y <= 345) {
					gameOverView();
					return;
				}
				break;
			}
		}
	}

	
	EndBatchDraw();
}

void rankListView()
{
	initgraph(600, 480, SWP_HIDEWINDOW);
	HWND hwndConsole = GetConsoleWindow();
	HWND hwndGraphics = GetForegroundWindow();
	HWND hwnd = GetForegroundWindow();
	SetWindowPos(hwndGraphics, HWND_TOPMOST, 400, 200, 0, 0, SWP_NOSIZE);
	
	FILE* file;
	int num;
	file = fopen("Rank.txt", "r");
	if (file == nullptr) {
		cout << "无法打开文件" << endl;
		return;
	}
	//cout << "文件成功被打开" << endl;
	vector<int> vec;
	while (fscanf(file, "%d", &num) == 1) {
		vec.push_back(num);
	}
	fclose(file);
	BeginBatchDraw();
	loadGameResource();
	putimage(0, 0, &BackGound);
	setbkcolor(BLUE);
	settextcolor(RED);
	settextstyle(30, 0, "宋体");
	for (int i = 0; i <= 432; i += 48) {
		string S;
		int m = i / 48;
		S += to_string(m + 1);
		S += ".";
		if (m < vec.size())
			S += to_string(vec[m]);
		outtextxy(100, i, (S.c_str()));
	}
	const RECT exit = { 500,400,600,480 };
	settextstyle(30, 0, "宋体");
	DrawButton(exit, YELLOW, "返回");
	cout << "打印成功完成" << endl;
	FlushBatchDraw();
	while (1) {
		if (peekmessage(&msg, EX_MOUSE)) {
			switch (msg.message)
			{
			case WM_MOUSEMOVE:
				break;
			case WM_LBUTTONDOWN:
				MOUX = msg.x;
				MOUY = msg.y;
			default:
				break;
			}
			if (MOUX >= 500 && MOUX <= 600 && MOUY >= 400 && MOUY <= 480) {
				MOUX = 0;
				MOUY = 0;
				closegraph();
				menuView();
				return;
			}
		}
	}
}


void gameOverView()
{
	
	MOUX = 0;
	MOUY = 0;
	mciSendString("close ../GameMaterials/deathsound.wav", NULL, 0, NULL);
	initgraph(600, 480, SWP_HIDEWINDOW);
	HWND hwndConsole = GetConsoleWindow();
	HWND hwndGraphics = GetForegroundWindow();
	HWND hwnd = GetForegroundWindow();
	SetWindowPos(hwndGraphics, HWND_TOPMOST, 400, 200, 0, 0, SWP_NOSIZE);
	setbkcolor(BLACK);
	settextcolor(RED);
	settextstyle(60, 0, "宋体");
	int cur = getRankList();
	const RECT rec1 = { 130,180,470,280 };
	const RECT rec2 = { 130,280,470,380 };
	const RECT rec3 = { 130,380,470,480 };
	BeginBatchDraw();
	loadGameResource();
	putimage(0, 0, &BackGound);
	//打印score，cur，重新开始，排行榜，返回菜单按钮
	string S;
	S += "本局分数：";
	S += to_string(score);
	outtextxy(130, 0, S.c_str());
	S.clear();
	S += "本局排名：";
	if (cur == -1) S += "未上榜";
	else S += to_string(cur);
	outtextxy(130, 80, S.c_str());
	DrawButton(rec1, YELLOW, "重新开始");
	DrawButton(rec2, YELLOW, "排行榜");
	DrawButton(rec3, YELLOW, "返回菜单");
	FlushBatchDraw();
	while (1) {
		if (peekmessage(&msg, EX_MOUSE)) {
			switch (msg.message)
			{
			case WM_MOUSEMOVE:
				break;
			case WM_LBUTTONDOWN:
				MOUX = msg.x;
				MOUY = msg.y;
			default:
				break;
			}
			if (MOUX >= 130 && MOUX <= 470 && MOUY >= 180 && MOUY <= 280) {//重新开始
				MOUX = 0; MOUY = 0;
				for (int i = 0; i < MapH; i++)
				{
					delete[]map[i];
				}
				closegraph();
				initialGame();
				gameView();
			}
			if (MOUX >= 130 && MOUX <= 470 && MOUY >= 280 && MOUY <= 380) {//排行榜
				MOUX = 0; MOUY = 0;
				for (int i = 0; i < MapH; i++)
				{
					delete[]map[i];
				}
				closegraph();
				rankListView();
			}
			if (MOUX >= 130 && MOUX <= 470 && MOUY >= 380 && MOUY <= 480) {//返回菜单
				MOUX = 0; MOUY = 0;
				for (int i = 0; i < MapH; i++)
				{
					delete[]map[i];
				}
				//delete[]map;//释放原来分配的内存
				temp_SnakeLong = 5;//蛇长
				temp_MapW = 20;//地图
				temp_MapH = 20;
				temp_SnakeSpeed = 1;//速度
				closegraph();
				menuView();
			}
		}
	}
}

void gameIntroView() {
	initgraph(600, 480, SWP_HIDEWINDOW);
	setbkcolor(RGB(0x7f, 0x8c, 0x8d));
	cleardevice();

	BeginBatchDraw();
	loadGameResource();
	putimage(0, 0 , &GameIntro);

	char returnBtnText[10] = "返回";
	button(20,20,100,50,returnBtnText);

	while (1) {
		FlushBatchDraw();
		// 获取消息
		ExMessage mouMsg;
		while (peekmessage(&mouMsg, EX_MOUSE)) {
			switch (mouMsg.message) {
			case WM_LBUTTONDOWN:
				if (mouMsg.x >= 20 && mouMsg.x <= 120 && mouMsg.y >= 20 && mouMsg.y <= 70) {
					closegraph();
					menuView();
					return;
				}
				break;
			}
		}
	}
	EndBatchDraw();
}

/*40*/
/*
v---------------------------------------------------------------------------------------
*/
