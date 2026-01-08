#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <windows.h>
#include <conio.h>

#include "snake.h"
#include "user.h"

// 自定义屏幕清除函数，使用Windows API更可靠
void clearScreen()
{
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coordScreen = {0, 0};
    DWORD cCharsWritten;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD dwConSize;

    // 获取控制台缓冲区信息
    GetConsoleScreenBufferInfo(hConsole, &csbi);
    dwConSize = csbi.dwSize.X * csbi.dwSize.Y;

    // 用空格填充整个缓冲区
    FillConsoleOutputCharacter(hConsole, ' ', dwConSize, coordScreen, &cCharsWritten);

    // 获取当前文本属性
    GetConsoleScreenBufferInfo(hConsole, &csbi);

    // 用当前文本属性填充整个缓冲区
    FillConsoleOutputAttribute(hConsole, csbi.wAttributes, dwConSize, coordScreen, &cCharsWritten);

    // 将光标位置设置到左上角
    SetConsoleCursorPosition(hConsole, coordScreen);
}

// 定义蛇的结构体->初始化蛇和食物->开始游戏(蛇和墙的碰撞，蛇和自身碰撞，蛇和食物碰撞)
int main()
{
    printf("=================================\n");
    printf("欢迎来到贪吃蛇游戏！\n");
    printf("请选择你要进行的操作：\n");
    printf("1.开始游戏\n");
    printf("2.查看分数\n");
    printf("3.关于我们\n");
    printf("4.游戏规则\n");
    printf("5.退出系统\n");
    printf("=================================\n");
    printf("请输入要进行的序号(0-5):");

    while (1)
    {
        // 清除之前可能残留的输入
        fflush(stdin);

        char choice = getchar();

        // 忽略换行符
        if (choice == '\n')
        {
            continue;
        }

        switch (choice)
        {
        case '1':
            clearScreen();
            start();
            break;
        case '2':
            query();
            break;
        case '3':
            about();
            break;
        case '4':
            printf("游戏规则：\n");
            printf("1.移动：使用方向键(W上,S下,A左,D右)控制蛇的移动方向。\n");
            printf("2.吃食物(#)：吃食物可以增加蛇的长度。\n");
            printf("3.陷阱(X)：碰撞陷阱将会导致游戏失败。\n");
            printf("祝你玩得开心！");
            printf("按任意键继续...");
            _getch();
            clearScreen();
            main();
            break;
        case '5':
            exit(0);
            break;
        default:
            printf("输入错误！请重新输入：");
        }
    }
}

void start()
{
    clearScreen();
    score = 0; // 防止上一轮分数未归零

    CONSOLE_CURSOR_INFO cci;  // 声明cci结构体
    cci.dwSize = sizeof(cci); // 设置大小
    cci.bVisible = FALSE;     // 隐藏光标
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cci);

    SetConsoleOutputCP(65001); // 设置控制台为UTF-8编码，解决中文乱码
    srand(time(NULL));         // 设置随机种子

    // 调用函数部分
    initFood();  // 调用食物
    initSnake(); // 调用蛇
    //showUI();//显示界面
    initWall(); // 调用墙
    playGame(); // 开始玩游戏

    // 清除标准输入缓冲区中的残留字符
    int c;
    while ((c = getchar()) != '\n' && c != EOF)
    {
        // 读取并丢弃字符
    }
}

void initFood()
{
    int valid = 0;
    while (!valid)
    {
        // 确保食物生成在墙内（留出墙的位置）
        food[0] = 1 + rand() % (high - 1); // x: 1 到 high-1 (列坐标)
        food[1] = 1 + rand() % (wide - 1); // y: 1 到 wide-1 (行坐标)

        // 检查食物是否与蛇身重叠
        valid = 1;
        for (int i = 0; i < snake.size; i++)
        {
            if (snake.body[i].x == food[0] && snake.body[i].y == food[1])
            {
                valid = 0; // 重叠了，重新生成
                break;
            }
        }
    }
}

void initSnake()
{
    // 第一节身体
    snake.size = 2;      // 开局初始长度为2(开局俩个头，后期全靠吃bushi)
    snake.body[0].x = 5; // 设置合适的初始x位置 (列坐标，应该小于high)
    snake.body[0].y = 5; // 设置合适的初始y位置 (行坐标，应该小于wide)

    // 第二节身体
    snake.body[1].x = snake.body[0].x - 1;
    snake.body[1].y = snake.body[0].y;
}

void showUI()
{
    COORD coord;
    // 更新尾巴
    coord.X = lx;
    coord.Y = ly;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    putchar(' ');
    // 显示蛇的位置
    for (int i = 0; i < snake.size; i++)
    {
        coord.X = snake.body[i].x;
        coord.Y = snake.body[i].y;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
        if (i == 0)
        {
            putchar('@'); // 蛇头
        }
        else
        {
            putchar('*'); // 蛇身
        }
    }
    // 显示食物位置
    coord.X = food[0];
    coord.Y = food[1];
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    putchar('#'); // 食物

    // 显示陷阱
    for (int i = 0; i < trapNum; i++)
    {
        coord.X = traps[i].x;
        coord.Y = traps[i].y;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
        putchar('X');
    }
}

void playGame()
{
    char key = 'D';
    dx = 1; // 初始向右移动
    dy = 0;
    int gameOver = 0;
    // 游戏不能等待用户输入才进行下一步，给以默认值

    while (!gameOver && snake.body[0].x >= 1 && snake.body[0].x < high && snake.body[0].y >= 1 && snake.body[0].y < wide)
    {             // 外循环与墙碰撞判断，确保在墙内移动
        showUI(); // 显示界面
        while (_kbhit())
        {
            key = _getch();
        }
        switch (key)
        {
        case 'D':
        case 'd':
            dx = 1;
            dy = 0;
            break;
        case 'A':
        case 'a':
            dx = -1;
            dy = 0;
            break;
        case 'W':
        case 'w':
            dx = 0;
            dy = -1;
            break;
        case 'S':
        case 's':
            dx = 0;
            dy = 1;
            break;
        }

        // 方向控制
        //  先记录尾巴坐标
        lx = snake.body[snake.size - 1].x;
        ly = snake.body[snake.size - 1].y;

        // 移动
        for (int i = snake.size - 1; i > 0; i--)
        {
            snake.body[i].x = snake.body[i - 1].x;
            snake.body[i].y = snake.body[i - 1].y;
        }

        // 更新蛇头位置
        snake.body[0].x += dx;
        snake.body[0].y += dy;

        // 检查s是否和墙碰撞
        if (snake.body[0].x < 1 || snake.body[0].x >= high || snake.body[0].y < 1 || snake.body[0].y >= wide)
        {
            gameOver = 1;
            break;
        }

        // 检查是否与自己碰撞
        for (int i = 1; i < snake.size; i++)
        {
            if (snake.body[0].x == snake.body[i].x && snake.body[0].y == snake.body[i].y)
            {
                gameOver = 1;
                break;
            }
        }

        // 判断是否与陷阱发生碰撞
        for (int i = 0; i < trapNum; i++)
        {
            if (snake.body[0].x == traps[i].x && snake.body[0].y == traps[i].y)
            {
                gameOver = 1;
                break;
            }
        }

        if (gameOver)
            break;

        // 蛇和食物碰撞（移动后）
        if (snake.body[0].x == food[0] && snake.body[0].y == food[1])
        {
            // 先保存新身体段的坐标为当前尾巴的位置
            snake.body[snake.size].x = lx;
            snake.body[snake.size].y = ly;

            initFood();   // 刷新食物
            snake.size++; // 长度加1
            score++;
            initTrap(score); // 随着分数增加，生成更多陷阱
        }
        Sleep(300);
        // system("cls");
    }

    // 游戏结束提示
    COORD coord;
    coord.X = high / 2 - 8; // X坐标（列）基于high(60)
    coord.Y = wide / 2;     // Y坐标（行）基于wide(20)
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    printf("Game Over!");

    coord.Y++;
    coord.X = high / 2 - 6;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    printf("你的分数是：%d\n", score);
    add(score);
    printf("请按任意键退回至主菜单...");
    _getch();
    clearScreen();
    main();

    Sleep(2000);

    // 清除输入缓冲区中的残留字符
    while (_kbhit())
    {
        _getch();
    }
}

void initWall()
{
    for (int i = 0; i <= wide; i++)
    {
        for (int j = 0; j <= high; j++)
        {
            // 绘制完整的矩形墙
            if (i == 0 || i == wide || j == 0 || j == high)
                putchar('=');
            else
                putchar(' ');
        }
        putchar('\n');
    }
}

void initTrap(int score)
{
    // 清除旧的陷阱显示
    COORD coord;
    for (int i = 0; i < trapNum; i++) {
        coord.X = traps[i].x;
        coord.Y = traps[i].y;
        SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
        putchar(' ');
    }
    
    trapNum = score;
    // 陷阱与食物的生成逻辑大致相同·，这里复写食物的代码
    for (int j = 0; j < trapNum; j++)
    {
        int valid = 0;
        while (!valid)
        {
            // 确保陷阱生成在墙内（留出墙的位置）
            traps[j].x = rand() % (high - 2) + 1;
            traps[j].y = rand() % (wide - 2) + 1;
            // 检查陷阱是否与蛇身重叠
            valid = 1;
            for (int i = 0; i < snake.size; i++)
            {
                if (snake.body[i].x == traps[j].x && snake.body[i].y == traps[j].y)
                {
                    valid = 0; // 重叠了，重新生成
                    break;
                }
            }

            if (valid)
            {
                // 检查陷阱是否与食物重叠
                if (traps[j].x == food[0] && traps[j].y == food[1])
                {
                    valid = 0; // 与食物重叠，重新生成
                }
            }

            if (valid)
            {
                // 检查陷阱是否与其他陷阱重叠
                for (int i = 0; i < j; i++)
                {
                    if (traps[i].x == traps[j].x && traps[i].y == traps[j].y)
                    {
                        valid = 0; // 与其他陷阱重叠，重新生成
                        break;
                    }
                }
            }
        }
    }
}

// 添加成绩
void add(int score)
{
    // 得到时间戳
    time_t t = time(NULL);
    struct tm *tm = localtime(&t);
    // 将分数与时间记录至链表中
    node *p = (node *)malloc(sizeof(node));
    if (p == NULL)
    {
        printf("内存分配失败！请重新运行程序！");
        return;
    }

    p->score = score;
    p->time = mktime(tm);
    p->next = head;
    head = p;
    len++;
}

// 查询成绩
void query()
{
    if (head == NULL)
    {
        printf("暂无成绩记录！\n");
        printf("请按任意键返回主菜单...");
        _getch();
        clearScreen();
        // return;
        // 不能使用return回调至主菜单，否则是接续上方代码运行，应当直接在方法里面调用main方法
        main();
    }

    printf("历史成绩记录：\n");
    printf("%-5s %-10s %-20s\n", "序号", "分数", "时间");
    printf("----------------------------------------\n");

    node *current = head;
    int index = 1;

    while (current != NULL)
    {

        // 解析时间戳
        struct tm *tm_info = localtime(&current->time);
        char time_str[20];
        strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", tm_info);

        printf("%-5d %-10d %-20s\n", index, current->score, time_str);
        current = current->next;
        index++;
    }

    printf("----------------------------------------\n");
    printf("共 %d 条记录\n", len);

    printf("按任意键返回主菜单...");
    _getch();
    clearScreen();
    main();
}

void about()
{
    printf("about us");
    printf("==========================================================\n");
    printf("这里是一些关于项目的一些链接，输入序号或者Ctrl+单机链接即可跳转\n");
    printf("1.GitHub地址：https://github.com/xunfen\n");
    printf("2.Gitee地址：https://gitee.com/xunfen250\n");
    printf("3.个人主页：https://www.594sb.top\n");
    printf("4.博客：https://www.xunfen.cloud\n");
    printf("输入其他以回到主菜单\n");
    printf("==========================================================\n");
    char choice;
    while (1)
    {
        choice = _getch();
        switch (choice)
        {
        case '1':
            system("start https://github.com/xunfen");
            break;
        case '2':
            system("start https://gitee.com/xunfen250");
            break;
        case '3':
            system("start https://www.594sb.top");
            break;
        case '4':
            system("start https://www.xunfen.cloud");
            break;
        default:
            clearScreen();
            main();
        }
    }
}

/***
 *      ┌─┐       ┌─┐
 *   ┌──┘ ┴───────┘ ┴──┐
 *   │                 │
 *   │       ───       │
 *   │  ─┬┘       └┬─  │
 *   │                 │
 *   │       ─┴─       │
 *   │                 │
 *   └───┐         ┌───┘
 *       │         │
 *       │         │
 *       │         │
 *       │         └──────────────┐
 *       │                        │
 *       │                        ├─┐
 *       │                        ┌─┘
 *       │                        │
 *       └─┐  ┐  ┌───────┬──┐  ┌──┘
 *         │ ─┤ ─┤       │ ─┤ ─┤
 *         └──┴──┘       └──┴──┘
 *                神兽保佑
 *               代码无BUG!
 */
