#pragma once // 防止头文件覆盖
// 此处采用链表存储用户数据
typedef struct node
{
    int score;
    time_t time;
    struct node *next;
} node;
int len = 0;
node *head = NULL;

void query();        // 声明查询
void add(int score); // 声明函数添加成绩
void about();
