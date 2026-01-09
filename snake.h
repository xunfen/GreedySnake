#pragma once//防止头文件覆盖
#define wide 20
#define high 60

struct body {
    int x, y;//蛇的坐标
};

struct snake {
    int size;//蛇的长度
    struct body body[wide * high];//嵌套蛇的身子
}snake;

//初始化食物
int food[2] = { 0 };//food[0]为x坐标，food[1]为y坐标

//初始化陷阱
typedef struct trap {
    int x, y;
} trap;
//陷阱数量
int trapNum = 0;

//全局陷阱数组
trap traps[wide * high];

//函数声明部分
void initFood();//初始化食物
void initSnake();//初始化蛇 
void showUI();//显示界面
void playGame();//开始游戏
void start();//在选择菜单为开始游戏引导代码声明
int score = 0;
//偏移坐标
int dx = 0;
int dy = 0;
void initWall();//初始化墙
//记录尾巴坐标
int lx = 0;
int ly = 0;
void initTrap(int score);

void save();
void load();