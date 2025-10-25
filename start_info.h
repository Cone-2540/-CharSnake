#ifndef START_INFO_H
#define START_INFO_H

// 将直接定义改为声明
extern int draw_state;       // 移除了初始化
extern int difficulty;
extern int menu_index;

const int MENU_ITEMS = 6;

void Draw_print();
void info_print();
int HandleInput();
int wait_start();

#endif