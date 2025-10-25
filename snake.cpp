#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include <ctime>
#include <cstdio>
#include <cstring>
#include <windows.h>
#include <conio.h>
#include <io.h>
#include <fcntl.h>
// #include "start_info.h"

using namespace std;
// 鍙傛暟瀹氫箟

int draw_state = 0;
int difficulty = 0;
int menu_index = 0;
const int MENU_ITEMS = 6; // 鍙傛暟瀹氫箟

int map_s[20][20];      // 鍦板浘:0:锟??, 1:锟??,2:铔囪韩锟??3:铔囧ご锟??4:椋熺墿锟??5锛氭槦锟??
bool game_over = false; // 娓告垙缁撴潫鏍囧織
char key_in = '\0';
bool is_eat = false;     // 锟??鍚﹀悆鍒帮拷?锟界墿
int speed = 400;         // 铔囩Щ鍔ㄩ�熷害
bool speed_up = false;   // 锟??鍚﹀姞锟??
bool eat_bene = false;   // 锟??鍚﹀悆鍒扮壒娈婏拷?锟界墿
bool auto_run = false;   // 锟??鍚﹁嚜鍔拷?锟借矾
bool bene_exist = false; // 锟??鍚﹀瓨鍦ㄧ壒娈婏拷?锟界墿
int cnt = 0;
int bene_cnt = 0;
int auto_cnt = 0;
float clk = 0;
const int start_length = 2; // 鍒濓拷?锟介暱锟??
int max_length = 324;       // 鍒濓拷?锟介暱锟??鍑嗭拷??

void setCursorPosition(int x, int y)
{
    static HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD coord = {(SHORT)y, (SHORT)x};
    SetConsoleCursorPosition(hOut, coord);
}

void hideCursor()
{
    CONSOLE_CURSOR_INFO cursor_info = {1, 0};
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &cursor_info);
}

struct PathNode
{
    int x, y;
}; // 锟??鍔拷?锟借矾鍑嗭拷??

class Food
{
public:
    int x;
    int y;
    int type;
    Food(int x, int y, int type = 0) : x(x), y(y), type(type) {}
};
Food *food_ptr = nullptr;

class Benefit
{
public:
    int x;
    int y;
    int type; // 0:*锛堣嚜鍔拷?锟借矾30s锟??, 1:$鍔狅拷?30s
    Benefit(int x, int y) : x(x), y(y)
    {
        x = x;
        y = y;
    };
};
Benefit *bene_ptr = nullptr;

void init_map()
{
    for (int i = 1; i < 19; i++)
    {
        for (int j = 1; j < 19; j++)
        {
            map_s[i][j] = 0;
        }
    }
    for (int i = 0; i < 20; i++)
    {
        map_s[0][i] = 1;
        map_s[19][i] = 1;
        map_s[i][0] = 1;
        map_s[i][19] = 1;
    }
    if (difficulty == 1)
    {
        for (int i = 4; i < 9; i++)
        {
            map_s[9][i] = 1;
            map_s[10][19 - i] = 1;
            map_s[i][10] = 1;
            map_s[19 - i][9] = 1;
        }
        max_length = 324 - 16; // 鍒濓拷?锟介暱锟??鍑嗭拷??
    }
    else if (difficulty == 2)
    {
        for (int i = 1; i < 8; i++)
        {
            map_s[9][i] = 1;
            map_s[10][19 - i] = 1;
            map_s[i][10] = 1;
            map_s[19 - i][9] = 1;
        }
        max_length = 324 - 32; // 鍒濓拷?锟介暱锟??鍑嗭拷??
    }
}

struct Snake_block
{
    int x;
    int y;
    Snake_block *next;
};

class Snake
{
private:
    int x;
    int y;
    int direction; // 0: up, 1: down, 2: left, 3: right

public:
    Snake_block *head;
    Snake_block *tail;
    int length;
    int getDirection() const { return direction; }
    Snake_block *getHead() const { return head; }
    int find_path(int target_x, int target_y)
    {
        // BFS鐩稿叧鏁版嵁
        bool visited[20][20] = {false};
        PathNode parent[20][20];
        int queue_x[400], queue_y[400]; // 20x20鍦板浘鐨勬渶澶у彲鑳介槦锟??
        int queue_head = 0, queue_tail = 0;

        // 鍒濓拷?锟藉寲闃熷垪
        queue_x[queue_tail] = head->x;
        queue_y[queue_tail] = head->y;
        queue_tail++;
        visited[head->x][head->y] = true;

        // 鏂瑰悜鏁扮粍锛氫笂锛屼笅锛屽乏锛屽彸
        int dirs[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

        while (queue_head < queue_tail)
        {
            int curr_x = queue_x[queue_head];
            int curr_y = queue_y[queue_head];
            queue_head++;

            // 鎵惧埌锟??锟??
            if (curr_x == target_x && curr_y == target_y)
            {
                // 鍥炴函锟??寰勬壘鍒帮拷??涓�锟??
                while (parent[curr_x][curr_y].x != head->x ||
                       parent[curr_x][curr_y].y != head->y)
                {
                    int px = parent[curr_x][curr_y].x;
                    int py = parent[curr_x][curr_y].y;
                    curr_x = px;
                    curr_y = py;
                }

                // 璁＄畻绉诲姩鏂瑰悜
                if (curr_x < head->x)
                    return 0; // 锟??
                if (curr_x > head->x)
                    return 1; // 锟??
                if (curr_y < head->y)
                    return 2; // 锟??
                return 3;     // 锟??
            }

            // 鎺㈢储鍥涗釜鏂瑰悜
            for (int i = 0; i < 4; i++)
            {
                int new_x = curr_x + dirs[i][0];
                int new_y = curr_y + dirs[i][1];

                // 妫�鏌ヨ竟鐣屽拰锟??閫氾拷?锟斤拷?
                if (new_x >= 0 && new_x < 20 && new_y >= 0 && new_y < 20 &&
                    !visited[new_x][new_y] &&
                    (map_s[new_x][new_y] == 0 || // 绌哄湴
                     map_s[new_x][new_y] == 4 || // 锟??閫氾拷?锟界墿
                     map_s[new_x][new_y] == 5 || // 鏄熸槦
                     map_s[new_x][new_y] == 6 || // 鍔狅拷?
                     (new_x == target_x && new_y == target_y)))
                {
                    visited[new_x][new_y] = true;
                    parent[new_x][new_y] = {curr_x, curr_y};

                    queue_x[queue_tail] = new_x;
                    queue_y[queue_tail] = new_y;
                    queue_tail++;
                }
            }
        }
        return -1; // 锟??鎵惧埌锟??锟??
    };

    Snake(int start_x, int start_y, int start_direction = 3)
    {
        // 仅初始化头节点，不生成默认身体
        head = new Snake_block;
        head->x = start_x;
        head->y = start_y;
        head->next = nullptr;
        tail = head;
        direction = start_direction;
        length = 1; // 初始长度设为1

        // 仅更新头部地图标记
        map_s[start_x][start_y] = 3;
    }

    void move()
    {
        Snake_block *new_head = new Snake_block;
        if (direction == 0)
        {
            new_head->x = head->x - 1; // 锟??
            new_head->y = head->y;
        }
        else if (direction == 1)
        {
            new_head->x = head->x + 1; // 锟??
            new_head->y = head->y;
        }
        else if (direction == 2)
        {
            new_head->x = head->x;
            new_head->y = head->y - 1; // 锟??
        }
        else if (direction == 3)
        {
            new_head->x = head->x;
            new_head->y = head->y + 1; // 锟??
        }

        // 妫�娴嬫挒澧欐垨锟??锟??
        if (map_s[new_head->x][new_head->y] == 1 || map_s[new_head->x][new_head->y] == 2)
        {
            game_over = true;
            delete new_head;
            return;
        }

        // 妫�娴嬫槸鍚﹀悆鍒帮拷?锟界墿
        if (map_s[new_head->x][new_head->y] == 4)
        {
            length++;
            is_eat = true;
        }

        if (map_s[new_head->x][new_head->y] == 5)
        {

            auto_run = true;
            eat_bene = true;
            auto_cnt = 0;
        }

        if (map_s[new_head->x][new_head->y] == 6)
        {
            eat_bene = true; // 鍚冨埌鐗规畩椋熺墿
            speed_up = true; // 鍔狅拷?
        }
        // 鏇存柊鍦板浘
        map_s[head->x][head->y] = 2;         // 鏃ц泧澶村彉鎴愯泧锟??
        map_s[new_head->x][new_head->y] = 3; // 鏂拌泧锟??

        // 鎻掑叆鏂拌泧锟??
        new_head->next = head;
        head = new_head;

        if (is_eat)
        {
            return;
        }
        if (eat_bene)
        {
            // 濡傛灉鍚冨埌鐗规畩椋熺墿锛岃泧锟??涓嶅彉
            eat_bene = false;
            return;
        }
        // 鍒犻櫎铔囧熬锛堟浛鎹㈠師鏈夐�昏緫锟??
        Snake_block *prev = nullptr;
        Snake_block *curr = head;
        while (curr->next != nullptr)
        {
            prev = curr;
            curr = curr->next;
        }
        map_s[curr->x][curr->y] = 0;
        if (prev)
        {
            prev->next = nullptr;
        }
        else
        {
            head = nullptr;
        }
        delete curr;
        tail = prev;
    }

    void turn(char key_in)
    {
        if (key_in == 'w' && direction != 1)
        {
            direction = 0;
        }
        else if (key_in == 's' && direction != 0)
        {
            direction = 1;
        }
        else if (key_in == 'a' && direction != 3)
        {
            direction = 2;
        }
        else if (key_in == 'd' && direction != 2)
        {
            direction = 3;
        }
    }
};
Snake *snake_ptr = nullptr;

void map_draw()
{
    setCursorPosition(0, 0); // 灏嗗厜鏍囩Щ鍔ㄥ埌鎺у埗鍙板乏涓婏拷??
    for (int i = 0; i < 20; i++)
    {
        for (int j = 0; j < 20; j++)
        {
            if (map_s[i][j] == 1)
            {
                cout << "\033[33m墙\033[0m"; // 锟??
            }
            else if (map_s[i][j] == 2)
            {
                if (difficulty == 2 && (i >= 8 && i <= 11) && (j >= 8 && j <= 11))
                {
                    cout << "\033[36m水\033[0m"; // 锟??
                }
                else
                {
                    if (auto_run)
                        cout << "\033[35m蛇\033[0m"; // 鏂拌泧锟??
                    else
                        cout << "\033[32m蛇\033[0m"; // 铔囪韩
                }
            }
            else if (map_s[i][j] == 3)
            {
                if (difficulty == 2 && (i >= 8 && i <= 11) && (j >= 8 && j <= 11))
                {
                    cout << "\033[36m水\033[0m"; // 锟??
                }
                else
                    cout << "\033[31m头\033[0m"; // 铔囧ご
            }
            else if (map_s[i][j] == 4)
            {
                cout << "\033[34m果\033[0m"; // 椋熺墿
            }
            else if (map_s[i][j] == 5)
            {
                cout << "\033[35m星\033[0m"; // 鏄熸槦
            }
            else if (map_s[i][j] == 6)
            {
                cout << "快"; // 鍔狅拷?
            }
            else
            {
                if (difficulty == 2 && (i >= 8 && i <= 11) && (j >= 8 && j <= 11))
                {
                    cout << "\033[36m水\033[0m"; // 锟??
                }
                else
                    cout << "  "; // 绌烘牸
            }
        }
        cout << endl;
    }
}

void saveGame()
{
    // 清空文件内容
    ofstream truncateFile("save.txt", ios::trunc);
    truncateFile.close();

    FILE *file = fopen("save.txt", "wb");
    if (file)
    {
        fwrite(&difficulty, sizeof(int), 1, file);
        fwrite(map_s, sizeof(int), 400, file);

        int length = snake_ptr->length;
        int direction = snake_ptr->getDirection();
        fwrite(&length, sizeof(int), 1, file);
        fwrite(&direction, sizeof(int), 1, file);

        Snake_block *curr = snake_ptr->getHead();
        while (curr != nullptr)
        {
            fwrite(&curr->x, sizeof(int), 1, file);
            fwrite(&curr->y, sizeof(int), 1, file);
            curr = curr->next;
        }

        fwrite(&food_ptr->x, sizeof(int), 1, file);
        fwrite(&food_ptr->y, sizeof(int), 1, file);

        bool beneExists = (bene_ptr != nullptr && bene_exist);
        fwrite(&beneExists, sizeof(bool), 1, file);
        if (beneExists)
        {
            fwrite(&bene_ptr->x, sizeof(int), 1, file);
            fwrite(&bene_ptr->y, sizeof(int), 1, file);
            fwrite(&bene_ptr->type, sizeof(int), 1, file);
        }

        fwrite(&speed, sizeof(int), 1, file);
        fwrite(&speed_up, sizeof(bool), 1, file);
        fwrite(&auto_run, sizeof(bool), 1, file);
        fwrite(&clk, sizeof(float), 1, file);
        fwrite(&game_over, sizeof(bool), 1, file);
        fwrite(&is_eat, sizeof(bool), 1, file);
        fwrite(&bene_exist, sizeof(bool), 1, file);
        fwrite(&bene_cnt, sizeof(int), 1, file);
        fwrite(&auto_cnt, sizeof(int), 1, file);
        fwrite(&cnt, sizeof(int), 1, file);

        fclose(file);
    }
}

bool loadGame()
{

    FILE *file = fopen("save.txt", "rb");
    if (!file)
        return false;

    // 清空当前地图和蛇对象
    fseek(file, 0, SEEK_SET);
    clearerr(file);

    // 读取难度
    if (fread(&difficulty, sizeof(int), 1, file) != 1)
    {
        fclose(file);
        return false;
    }

    // 读取地图
    if (fread(map_s, sizeof(int), 400, file) != 400)
    {
        fclose(file);
        return false;
    }

    // 读取蛇信息
    int length, direction;
    if (fread(&length, sizeof(int), 1, file) != 1 ||
        fread(&direction, sizeof(int), 1, file) != 1)
    {
        fclose(file);
        return false;
    }

    int head_x, head_y;
    if (fread(&head_x, sizeof(int), 1, file) != 1 ||
        fread(&head_y, sizeof(int), 1, file) != 1)
    {
        fclose(file);
        return false;
    }

    // 创建新蛇对象前清理旧对象
    delete snake_ptr;
    snake_ptr = new Snake(head_x, head_y, direction);
    snake_ptr->length = length;

    // 清除构造函数自动生成的第二个节点
    if (snake_ptr->getHead()->next)
    {
        delete snake_ptr->getHead()->next;
        snake_ptr->getHead()->next = nullptr;
    }

    // 清除默认生成的节点
    Snake_block *curr = snake_ptr->getHead();
    while (curr->next != nullptr)
    {
        Snake_block *temp = curr->next;
        delete curr;
        curr = temp;
    }

    // 重建蛇身
    Snake_block *new_head = new Snake_block{head_x, head_y, nullptr};
    snake_ptr->head = new_head;
    curr = new_head;

    for (int i = 1; i < length; ++i)
    {
        int x, y;
        if (fread(&x, sizeof(int), 1, file) != 1 ||
            fread(&y, sizeof(int), 1, file) != 1)
        {
            fclose(file);
            delete snake_ptr;
            snake_ptr = nullptr;
            return false;
        }
        curr->next = new Snake_block{x, y, nullptr};
        curr = curr->next;
        map_s[x][y] = (i == 0) ? 3 : 2;
    }

    int food_x, food_y;
    fread(&food_x, sizeof(int), 1, file);
    fread(&food_y, sizeof(int), 1, file);
    food_ptr = new Food(food_x, food_y);

    bool beneExists;
    fread(&beneExists, sizeof(bool), 1, file);
    if (beneExists)
    {
        int x, y, type;
        fread(&x, sizeof(int), 1, file);
        fread(&y, sizeof(int), 1, file);
        fread(&type, sizeof(int), 1, file);
        bene_ptr = new Benefit(x, y);
        bene_ptr->type = type;
        map_s[x][y] = (type == 0) ? 5 : 6;
    }

    fread(&speed, sizeof(int), 1, file);
    fread(&speed_up, sizeof(bool), 1, file);
    fread(&auto_run, sizeof(bool), 1, file);
    fread(&clk, sizeof(float), 1, file);
    fread(&game_over, sizeof(bool), 1, file);
    fread(&is_eat, sizeof(bool), 1, file);
    fread(&bene_exist, sizeof(bool), 1, file);
    fread(&bene_cnt, sizeof(int), 1, file);
    fread(&auto_cnt, sizeof(int), 1, file);
    fread(&cnt, sizeof(int), 1, file);

    fclose(file);
    return true;
}

// 来自start_info.h的文件内容
void Draw_print()
{
    system("cls"); // 清屏
    cout << "贪贪贪贪  吃    吃     蛇        游游  戏   戏" << endl;
    cout << "贪        吃吃  吃   蛇  蛇    游      戏  戏  " << endl;
    cout << "贪贪贪贪  吃 吃 吃  蛇    蛇  游       戏戏    " << endl;
    cout << "      贪  吃  吃吃  蛇蛇蛇蛇   游      戏  戏  " << endl;
    cout << "贪贪贪贪  吃    吃  蛇    蛇     游游  戏   戏" << endl;
    cout << endl
         << "                   作者：许传岳" << endl;
    cout << endl;
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
                if (_access("save.txt", 0) == -1)
                { // 检查存档文件是否存在
                    cout << "当前没有存档！" << endl;
                    Sleep(1000);
                    menu_index = 0; // 返回主菜单
                    Draw_print();
                    info_print();
                    continue;
                }
                if (!loadGame())
                { // 尝试加载存档
                    cout << "存档损坏或加载失败！" << endl;
                    Sleep(1000);
                    menu_index = 0;
                    Draw_print();
                    info_print();
                    continue;
                }
                cout << "正在继续游戏..." << endl;
                Sleep(1000);
                return 1; // 进入游戏循环
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
                menu_index = 0; // 重置菜单索引
                Draw_print();
                info_print();
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
                ifstream fin("history_score.txt");
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
// 来自start_info.h的文件内容

int main()
{
    game_over = false;
    speed_up = false;
    auto_run = false;

    int start_mode = wait_start();
    hideCursor();

    if (start_mode == 0)
    { // 锟斤拷锟斤拷戏
        init_map();
        srand(time(0));
        int food_x = rand() % 18 + 1;
        int food_y = rand() % 18 + 1;
        food_ptr = new Food(food_x, food_y);
        snake_ptr = new Snake(3, 3);
        map_s[food_x][food_y] = 4;
    }
    else
    {
        if (!loadGame())
        {
            system("cls");
            cout << "存档损坏或加载失败！" << endl;
            Sleep(2000);
            return 0;
        }
        // 额外校验蛇头位置合法性
        Snake_block *head = snake_ptr->getHead();
        if (head->x <= 0 || head->x >= 19 ||
            head->y <= 0 || head->y >= 19 ||
            map_s[head->x][head->y] != 3)
        {
            system("cls");
            cout << "存档数据异常！" << endl;
            Sleep(2000);
            return 0;
        }
    }
    while (true)
    {
        if (snake_ptr->length == 324)
        {
            system("cls");
            map_draw();
            cout << "得胜" << endl;
            break;
        }
        if (game_over)
        {
            system("cls");
            map_draw();
            cout << "游戏结束，得分: " << snake_ptr->length << endl;
            break;
        }
        if (auto_run)
        {
            // 锟??鍔拷?锟借矾妯″紡
            int food_x = food_ptr->x, food_y = food_ptr->y;
            int dir = snake_ptr->find_path(food_x, food_y);
            if (dir != -1)
            {
                // 锟??锟??涓烘柟鍚戝瓧锟??
                key_in = 'd';
                if (dir == 0)
                    key_in = 'w';
                else if (dir == 1)
                    key_in = 's';
                else if (dir == 2)
                    key_in = 'a';
                auto_cnt++;
                if (auto_cnt >= 60)
                {
                    auto_run = false;
                    auto_cnt = 0;
                }
            }
            else
            {
                cout << "无路可走" << endl;
                auto_run = false;
            }
        }
        else if (_kbhit())
        {
            char input = _getch();
            if (input == 27) // ESC锟斤拷锟斤拷
            {
                system("cls");
                map_draw();
                cout << "游戏结束，得分: " << snake_ptr->length << endl;
                break;
            }
            if (input == 'T' || input == 't')
            {
                saveGame();
                setCursorPosition(24, 0);
                cout << "游戏存档成功！" << endl;
                Sleep(1000);
            }
            if (input == 'p' || input == 'P') // 锟斤拷锟斤拷P锟斤拷锟斤拷停
            {
                system("cls");
                map_draw();
                cout << "暂停成功，按任意键继续..." << endl;
                _getch(); // 锟饺达拷锟矫伙拷锟斤拷锟斤拷
                system("cls");
                map_draw();
            }

            if (input == 'w' || input == 's' || input == 'a' || input == 'd')
            {
                key_in = input;
            }
        }

        if (key_in == 'w' || key_in == 's' || key_in == 'a' || key_in == 'd')
        {
            snake_ptr->turn(key_in);
        }
        snake_ptr->move();

        // 杈撳嚭娓告垙鐘讹拷?
        clk += speed / 1000.0;
        // 澶勭悊椋熺墿鍜岀壒娈婏拷?锟界墿

        if (speed_up)
        {
            speed = 200;
            cnt++;
            if (cnt == 30)
            {
                speed = 400;
                cnt = 0;
                speed_up = false;
            }
        }
        if (eat_bene)
        {
            eat_bene = false;
            bene_cnt = 0;
            bene_exist = false;
        }
        // 澶勭悊鐗规畩椋熺墿
        if (bene_exist)
        {
            bene_cnt++;
            if (bene_cnt >= 30)
            {
                map_s[bene_ptr->x][bene_ptr->y] = 0; // 娓呴櫎椋熺墿
                bene_exist = false;
                bene_cnt = 0;
            }
        }
        else
        {
            if (bene_ptr == nullptr)
            {
                bene_ptr = new Benefit(0, 0); // 初始位置会被后续循环覆盖
            }
            while (true)
            {
                bene_ptr->x = rand() % 18 + 1;
                bene_ptr->y = rand() % 18 + 1;
                if (map_s[bene_ptr->x][bene_ptr->y] == 0)
                    break; // 鎵惧埌涓�锟??绌轰綅锟??
            }
            int bene_type = rand() % 30; // 闅忔満鐢熸垚鐗规畩椋熺墿绫诲瀷
            if (bene_type == 0)
            {
                map_s[bene_ptr->x][bene_ptr->y] = 5; // 鏄熸槦
                bene_exist = true;
            }
            else if (bene_type == 1)
            {
                map_s[bene_ptr->x][bene_ptr->y] = 6; // 鐗规畩椋熺墿
                bene_exist = true;
            }
        }

        if (is_eat)
        {
            map_s[food_ptr->x][food_ptr->y] = 0; // 娓呴櫎椋熺墿
            while (true)
            {
                food_ptr->x = rand() % 18 + 1;
                food_ptr->y = rand() % 18 + 1;
                if (map_s[food_ptr->x][food_ptr->y] == 0)
                    break; // 鎵惧埌涓�锟??绌轰綅锟??
            }
            map_s[food_ptr->x][food_ptr->y] = 4; // 鏂帮拷?锟界墿
            is_eat = false;
        }
        map_draw();
        Sleep(speed); // 鍔ㄩ�熷害
        setCursorPosition(21, 0);
        cout << "分: " << snake_ptr->length << "                  ";
        setCursorPosition(21, 20);
        cout << "时: " << fixed << setprecision(0) << clk << "                  ";
        setCursorPosition(22, 0);

        cout << "按P暂停，T存档，ESC退出,自动寻路期间请勿按键" << endl;
        setCursorPosition(24, 0);
        if (int(clk) % 10 == 0)
        {
            cout << "                      ";
        }
    }
    int max_score = 0, score;
    ifstream fin("history_score.txt");
    ofstream fout("history_score.txt", ios::app);
    if (fin.is_open())
    {
        while (fin >> score)
        {
            if (score > max_score)
                max_score = score;
        }
        if (snake_ptr->length > max_score)
        {
            fout << snake_ptr->length << endl;
            cout << "恭喜你创造了新的历史最高分！" << endl;
        }
        fin.close();
    }
    delete snake_ptr;
    delete food_ptr;
    delete bene_ptr;
    system("pause");
    return 0;
}