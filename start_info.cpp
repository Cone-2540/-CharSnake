#include <bits/stdc++.h>
#include <windows.h>
#include <conio.h>
#include "start_info.h"
using namespace std;

// 修改后的绘制函数
void Draw_print()
{
    system("cls"); // 清屏
    cout << "贪贪贪贪  吃    吃     蛇        游游  戏   戏" << endl;
    cout << "贪        吃吃  吃   蛇  蛇    游      戏  戏  " << endl;
    cout << "贪贪贪贪  吃 吃 吃  蛇    蛇  游       戏戏    " << endl;
    cout << "      贪  吃  吃吃  蛇蛇蛇蛇   游      戏  戏  " << endl;
    cout << "贪贪贪贪  吃    吃  蛇    蛇     游游  戏   戏" << endl;
    cout << endl
         << endl;
    return;
}

// 带光标效果的菜单显示
void info_print()
{
    const string menu[] = {
        "新 的 游 戏",
        "继 续 游 戏",
        "游 戏 规 则",
        "难 度 选 择",
        "历 史 记 录",
        "退 出 游 戏"};

    for (int i = 0; i < MENU_ITEMS; ++i)
    {
        if (i == menu_index)
        {
            cout << "                >> " << menu[i] << " <<" << endl;
        }
        else
        {
            cout << "                   " << menu[i] << "   " << endl;
        }
    }
}

// 处理键盘输入
int HandleInput()
{
    if (_kbhit() && draw_state == 0)
    {
        char input = _getch();
        if (input == 's' || input == 'S')
        { // 下移
            menu_index = (menu_index + 1) % MENU_ITEMS;
        }
        else if (input == 'w' || input == 'W')
        { // 上移
            menu_index = (menu_index - 1 + MENU_ITEMS) % MENU_ITEMS;
        }
        else if (input == '\r' || input == '\n')
        { // 确认选择
            switch (menu_index)
            {
            case 0:
                return 1;
                break; // 新游戏
            case 1:
                return 2; // 继续游戏
                break;
            case 2:
                return 3; // 游戏规则
                break;
            case 3:
                return 4; // 难度选择
                break;
            case 4:
                return 5; // 历史记录
                break;
            case 5:
                return 6; // 退出游戏
                break;
            }
        }
    }
    return 0; // 默认返回0
}

int wait_start()
{
    Draw_print();
    info_print();
    while (true)
    {
        if (_kbhit())
        {
            draw_state = HandleInput();

            Draw_print();
            info_print();

            if (draw_state == 1)
            {
                system("cls");
                cout << "新游戏即将开始！" << endl;
                Sleep(1000);
                return 0; // 返回到游戏主循环
            }
            else if (draw_state == 2)
            {
                system("cls");
                cout << "即将继续游戏！ " << endl;
                Sleep(1000);
                return 1;
            }
            else if (draw_state == 3)
            {
                system("cls");
                cout << "游戏规则：" << endl;
                cout << "1. 使用WASD键控制蛇的移动方向。" << endl;
                cout << "2. 吃到食物会增加分数，星星能让你自动寻路一段时间。" << endl;
                cout << "3. 碰到墙壁或自己的身体会结束游戏。" << endl;
                cout << "4. 游戏有三种难度：简单、中等、困难。" << endl;
                cout << "简单模式：开阔场地" << endl;
                cout << "中等模式：存在障碍" << endl;
                cout << "困难模式：存在水域和障碍" << endl;
                cout << "按任意键返回菜单..." << endl;
                _getch();
            }
            else if (draw_state == 4)
            {
                int diff_index = difficulty; // 当前选中的难度索引
                bool in_difficulty_menu = true;

                while (in_difficulty_menu)
                {
                    system("cls");
                    // 绘制带光标的难度菜单
                    cout << "难度选择：当前难度：";
                    switch (difficulty)
                    {
                    case 0:
                        cout << "简单\n\n";
                        break;
                    case 1:
                        cout << "中等\n\n";
                        break;
                    case 2:
                        cout << "困难\n\n";
                        break;
                    }

                    cout << (diff_index == 0 ? "  >> 1. 简单 <<" : "    1. 简单") << endl;
                    cout << (diff_index == 1 ? "  >> 2. 中等 <<" : "    2. 中等") << endl;
                    cout << (diff_index == 2 ? "  >> 3. 困难 <<" : "    3. 困难") << endl;
                    cout << "\n操作提示：W/S-选择  Enter-确认  ESC-返回";
                    Sleep(500); // 短暂延时，避免过快刷新
                    // 输入处理
                    if (_kbhit())
                    {
                        char input1 = _getch();
                        if (input1 == 's' || input1 == 'S')
                        {
                            diff_index = (diff_index + 1) % 3;
                        }
                        if (input1 == 'w' || input1 == 'W')
                        {
                            diff_index = (diff_index - 1 + 3) % 3;
                        }
                        if (input1 == '\r' || input1 == '\n')
                        {
                            difficulty = diff_index;    // 更新难度
                            in_difficulty_menu = false; // 退出难度选择菜单
                            system("cls");
                            cout << "已选择难度：";
                            switch (difficulty)
                            {
                            case 0:
                                cout << "简单" << endl;
                                break;
                            case 1:
                                cout << "中等" << endl;
                                break;
                            case 2:
                                cout << "困难" << endl;
                                break;
                            }
                            cout << "按任意键返回菜单..." << endl;
                            _getch(); // 等待用户输入
                        }
                        else if (_kbhit() && _getch() == 27) // ESC键
                        {
                            in_difficulty_menu = false; // 返回主菜单
                        }
                    }
                    draw_state = 0; // 返回主菜单
                }
                menu_index = 0; // 重置菜单索引
                Draw_print();
                info_print();
            }
            else if (draw_state == 5)
            {
                system("cls");
                cout << "历史记录：" << endl;
                // 读取并显示历史最高分
                ifstream fin("C:\\Users\\59088\\Desktop\\history_score.txt");
                if (fin.is_open())
                {
                    int max_score = 0, score;
                    while (fin >> score)
                    {
                        if (score > max_score)
                            max_score = score;
                    }
                    fin.close();
                    cout << "历史最高分：" << max_score << endl;
                }
                else
                {
                    cout << "暂无历史记录。" << endl;
                }
                cout << "按任意键返回菜单..." << endl;
                _getch();
                menu_index = 0; // 重置菜单索引
                Draw_print();
                info_print();
            }
            else if (draw_state == 6)
            {
                system("cls");
                cout << "退出游戏！" << endl;
                exit(0);
            }
            Sleep(10);
        }
    }
}
/*
int main()
{
    wait_start();
    return 0;
}
    */