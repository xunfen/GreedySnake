#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <windows.h>
#include <conio.h>

#include "snake.h"
#include "user.h"

// 自定义屏幕清除函数，使用Windows API https://docs.pingcode.com/baike/980461
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
    SetConsoleCursorPosition(hConsole, coordScreen);//https://blog.csdn.net/baiqi123456/article/details/119753736
}

// 定义蛇的结构体->初始化蛇和食物->开始游戏(蛇和墙的碰撞，蛇和自身碰撞，蛇和食物碰撞)
int main()
{
    printf("=================================\n");
    printf("欢迎来到贪吃蛇游戏！\n");
    printf("请选择你要进行的操作：\n");
    printf("0.加载游戏\n");
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
            printf("\n==============游戏规则===============：\n");
            printf("1.移动：使用方向键(W上,S下,A左,D右)控制蛇的移动方向。\n");
            printf("2.吃食物(#)：吃食物可以增加蛇的长度。\n");
            printf("3.陷阱(X)：碰撞陷阱将会导致游戏失败。\n");
            printf("4.碰撞墙壁将会导致游戏失败。\n");
            printf("5.游戏中按住q将保存游戏数据并退出\n");
            printf("注意：为了保证游戏公平，每轮游戏最多仅允许暂停一次，否则判断失败！！！\n");
            printf("祝你玩得开心！\n");
            printf("按任意键继续...");
            _getch();
            clearScreen();
            main();
            break;
        case '5':
            exit(0);
            break;
        case '0':
            load();
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

    //隐藏光标获取良好体验https://docs.pingcode.com/baike/1162036
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

    // 游戏结束后依旧会有scanner在，此处清除
    int c;
    while ((c = getchar()) != '\n' && c != EOF)//目标时换行
    {
        // 读取并丢弃字符
    }
}

void initFood()
{
    int valid = 0;//定义flag变量以判断是否重叠
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

    // 显示分数（使用合理坐标）
    coord.X = high + 2;  // 在游戏区域右侧显示
    coord.Y = 1;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    printf("分数: %d", score);
    
    // 如果需要显示游戏规则，使用较小的坐标值
    coord.X = high + 2;  // 游戏区域右侧
    coord.Y = 3;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    printf("游戏规则:\n");
    
    coord.X = high + 2;
    coord.Y = 4;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    printf("W/S/A/D移动\n");
    
    coord.X = high + 2;
    coord.Y = 5;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    printf("#食物 +长度\n");
    
    coord.X = high + 2;
    coord.Y = 6;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    printf("X陷阱 -失败\n");
    
    coord.X = high + 2;
    coord.Y = 7;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
    printf("按Q保存退出\n");
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
        case 'q':
        case 'Q':
            save();
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
        Sleep(300);//代表帧数
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
    //printf("about us");
    printf("\n==========================================================\n");
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

void save()
{
    FILE *fp;

    //如果在同意文件夹下的二级文件夹下的文件哪怕是w只读(如果文件不存在则创建新文件)依旧会返回NULL，故此处判断是否存在，如不存在则创建文件
    FILE *check_snake = fopen("./data/snake.ini", "r");
    FILE *check_food = fopen("./data/food.ini", "r");
    FILE *check_trap = fopen("./data/trap.ini", "r");
    
    if (check_snake == NULL || check_food == NULL || check_trap == NULL) {
        // 关闭已打开的文件
        if(check_snake) fclose(check_snake);
        if(check_food) fclose(check_food);
        if(check_trap) fclose(check_trap);
        
        // 创建data目录
        system("if not exist data mkdir data");
    } else {
        // 如果文件都存在，则关闭检查用的文件指针
        fclose(check_snake);
        fclose(check_food);
        fclose(check_trap);
    }
    
    // 保存蛇
    fp = fopen("./data/snake.ini", "w");
    if (fp == NULL)
    {
        printf("保存蛇数据失败！\n");
        return;
    }
    fprintf(fp, "[Snake]\n");
    fprintf(fp, "Size=%d\n", snake.size);//大小
    fprintf(fp, "Score=%d\n", score);//目前分数
    fprintf(fp, "X=%d\n", dx);//蛇头x坐标
    fprintf(fp, "Y=%d\n", dy);//蛇头y坐标
    fprintf(fp, "[Body]\n");//循环读取身体以保存身体数据
    for (int i = 0; i < snake.size; i++)
    {
        fprintf(fp, "Part%d=%d,%d\n", i, snake.body[i].x, snake.body[i].y);
    }
    fclose(fp);
    
    // 保存食物
    fp = fopen("./data/food.ini", "w");
    if (fp == NULL)
    {
        printf("保存食物数据失败！\n");
        return;
    }
    fprintf(fp, "[Food]\n");
    fprintf(fp, "X=%d\n", food[0]);
    fprintf(fp, "Y=%d\n", food[1]);
    fclose(fp);
    
    // 保存陷阱
    fp = fopen("./data/trap.ini", "w");
    if (fp == NULL)
    {
        printf("保存陷阱数据失败！\n");
        return;
    }
    fprintf(fp, "[Traps]\n");
    fprintf(fp, "Count=%d\n", trapNum);
    for (int i = 0; i < trapNum; i++)
    {
        fprintf(fp, "Trap%d=%d,%d\n", i, traps[i].x, traps[i].y);//循环读取陷阱以保存陷阱数据
    }
    fclose(fp);
    
    printf("游戏数据保存成功！\n");
    printf("按任意键返回主菜单...");
    _getch();
    clearScreen();
    main();
}

void load() {
    //判断本地文件是否存在
    FILE *snakeD = fopen("./data/snake.ini", "r");
    FILE *foodD = fopen("./data/food.ini", "r");
    FILE *trapD = fopen("./data/trap.ini", "r");
    if (snakeD == NULL || foodD == NULL || trapD == NULL) {
        system("color 4");
        printf("游戏数据不存在！\n");
        if(snakeD) fclose(snakeD);
        if(foodD) fclose(foodD);
        if(trapD) fclose(trapD);
        printf("请按任意键返回主菜单...");
        _getch();
        clearScreen();
        system("color 7");
        main();
    }
    
    //回复颜色显示
    system("color 7");

    // 读取食物
    system("color 2");
    FILE *fp = fopen("./data/food.ini", "r");
    if (fp == NULL)
    {
        printf("读取食物数据失败！\n按任意键返回！\n");
        _getch();
        system("color 7");
        main();        
    }
    fscanf(fp, "%d %d", &food[0], &food[1]);
    fclose(fp);
    printf("食物已加载...\n");
    Sleep(500);
    
    // 读取蛇 
    system("color 6");
    fp = fopen("./data/snake.ini", "r");
    if (fp == NULL)
    {
        printf("读取蛇数据失败！\n按任意键返回！\n");
        _getch();
        system("color 7");
        main();
    }
    fscanf(fp, "%d %d %d %d", &snake.size, &score, &dx, &dy);
    for (int i = 0; i < snake.size; i++)
    {
        fscanf(fp, "%d %d", &snake.body[i].x, &snake.body[i].y);
    }
    fclose(fp);
    printf("蛇数据已加载...\n");
    Sleep(500);
    
    // 读取陷阱
    system("color 3");
    fp = fopen("./data/trap.ini", "r");
    if (fp == NULL) {
        printf("读取陷阱数据失败！\n按任意键返回！\n");
        _getch();
        system("color 7");
        main();
    }
    fscanf(fp, "%d", &trapNum); 
    printf("陷阱数量已加载...\n");
    Sleep(300);
    for (int i = 0; i < trapNum; i++) {
        fscanf(fp, "%d %d", &traps[i].x, &traps[i].y);
    }
    fclose(fp);
    printf("陷阱数据已加载...\n");
    Sleep(500);
    
    system("color 7");
    printf("游戏数据已全部加载完成！\n");
    
    // 继续游戏
    printf("按任意键继续游戏...");
    _getch();
    clearScreen();
    
    // 添加墙的初始化
    initWall();
    playGame();
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
