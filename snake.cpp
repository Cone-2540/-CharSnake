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
// 参数定义

int draw_state = 0;
int difficulty = 0;
int menu_index = 0;
const int MENU_ITEMS = 6; // 参数定义

int map_s[20][20];      // 地图:0:�??, 1:�??,2:蛇身�??3:蛇头�??4:食物�??5：星�??
bool game_over = false; // 游戏结束标志
char key_in = '\0';
bool is_eat = false;     // �??否吃到�?�物
int speed = 400;         // 蛇移动速度
bool speed_up = false;   // �??否加�??
bool eat_bene = false;   // �??否吃到特殊�?�物
bool auto_run = false;   // �??否自动�?�路
bool bene_exist = false; // �??否存在特殊�?�物
int cnt = 0;
int bene_cnt = 0;
int auto_cnt = 0;
float clk = 0;
const int start_length = 2; // 初�?�长�??
int max_length = 324;       // 初�?�长�??准�??

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
}; // �??动�?�路准�??

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
    int type; // 0:*（自动�?�路30s�??, 1:$加�?30s
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
        max_length = 324 - 16; // 初�?�长�??准�??
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
        max_length = 324 - 32; // 初�?�长�??准�??
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
        // BFS相关数据
        bool visited[20][20] = {false};
        PathNode parent[20][20];
        int queue_x[400], queue_y[400]; // 20x20地图的最大可能队�??
        int queue_head = 0, queue_tail = 0;

        // 初�?�化队列
        queue_x[queue_tail] = head->x;
        queue_y[queue_tail] = head->y;
        queue_tail++;
        visited[head->x][head->y] = true;

        // 方向数组：上，下，左，右
        int dirs[4][2] = {{-1, 0}, {1, 0}, {0, -1}, {0, 1}};

        while (queue_head < queue_tail)
        {
            int curr_x = queue_x[queue_head];
            int curr_y = queue_y[queue_head];
            queue_head++;

            // 找到�??�??
            if (curr_x == target_x && curr_y == target_y)
            {
                // 回溯�??径找到�??一�??
                while (parent[curr_x][curr_y].x != head->x ||
                       parent[curr_x][curr_y].y != head->y)
                {
                    int px = parent[curr_x][curr_y].x;
                    int py = parent[curr_x][curr_y].y;
                    curr_x = px;
                    curr_y = py;
                }

                // 计算移动方向
                if (curr_x < head->x)
                    return 0; // �??
                if (curr_x > head->x)
                    return 1; // �??
                if (curr_y < head->y)
                    return 2; // �??
                return 3;     // �??
            }

            // 探索四个方向
            for (int i = 0; i < 4; i++)
            {
                int new_x = curr_x + dirs[i][0];
                int new_y = curr_y + dirs[i][1];

                // 检查边界和�??通�?��?
                if (new_x >= 0 && new_x < 20 && new_y >= 0 && new_y < 20 &&
                    !visited[new_x][new_y] &&
                    (map_s[new_x][new_y] == 0 || // 空地
                     map_s[new_x][new_y] == 4 || // �??通�?�物
                     map_s[new_x][new_y] == 5 || // 星星
                     map_s[new_x][new_y] == 6 || // 加�?
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
        return -1; // �??找到�??�??
    };

    Snake(int start_x, int start_y, int start_direction = 3)
    {
        // ����ʼ��ͷ�ڵ㣬������Ĭ������
        head = new Snake_block;
        head->x = start_x;
        head->y = start_y;
        head->next = nullptr;
        tail = head;
        direction = start_direction;
        length = 1; // ��ʼ������Ϊ1

        // ������ͷ����ͼ���
        map_s[start_x][start_y] = 3;
    }

    void move()
    {
        Snake_block *new_head = new Snake_block;
        if (direction == 0)
        {
            new_head->x = head->x - 1; // �??
            new_head->y = head->y;
        }
        else if (direction == 1)
        {
            new_head->x = head->x + 1; // �??
            new_head->y = head->y;
        }
        else if (direction == 2)
        {
            new_head->x = head->x;
            new_head->y = head->y - 1; // �??
        }
        else if (direction == 3)
        {
            new_head->x = head->x;
            new_head->y = head->y + 1; // �??
        }

        // 检测撞墙或�??�??
        if (map_s[new_head->x][new_head->y] == 1 || map_s[new_head->x][new_head->y] == 2)
        {
            game_over = true;
            delete new_head;
            return;
        }

        // 检测是否吃到�?�物
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
            eat_bene = true; // 吃到特殊食物
            speed_up = true; // 加�?
        }
        // 更新地图
        map_s[head->x][head->y] = 2;         // 旧蛇头变成蛇�??
        map_s[new_head->x][new_head->y] = 3; // 新蛇�??

        // 插入新蛇�??
        new_head->next = head;
        head = new_head;

        if (is_eat)
        {
            return;
        }
        if (eat_bene)
        {
            // 如果吃到特殊食物，蛇�??不变
            eat_bene = false;
            return;
        }
        // 删除蛇尾（替换原有逻辑�??
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
    setCursorPosition(0, 0); // 将光标移动到控制台左上�??
    for (int i = 0; i < 20; i++)
    {
        for (int j = 0; j < 20; j++)
        {
            if (map_s[i][j] == 1)
            {
                cout << "\033[33mǽ\033[0m"; // �??
            }
            else if (map_s[i][j] == 2)
            {
                if (difficulty == 2 && (i >= 8 && i <= 11) && (j >= 8 && j <= 11))
                {
                    cout << "\033[36mˮ\033[0m"; // �??
                }
                else
                {
                    if (auto_run)
                        cout << "\033[35m��\033[0m"; // 新蛇�??
                    else
                        cout << "\033[32m��\033[0m"; // 蛇身
                }
            }
            else if (map_s[i][j] == 3)
            {
                if (difficulty == 2 && (i >= 8 && i <= 11) && (j >= 8 && j <= 11))
                {
                    cout << "\033[36mˮ\033[0m"; // �??
                }
                else
                    cout << "\033[31mͷ\033[0m"; // 蛇头
            }
            else if (map_s[i][j] == 4)
            {
                cout << "\033[34m��\033[0m"; // 食物
            }
            else if (map_s[i][j] == 5)
            {
                cout << "\033[35m��\033[0m"; // 星星
            }
            else if (map_s[i][j] == 6)
            {
                cout << "��"; // 加�?
            }
            else
            {
                if (difficulty == 2 && (i >= 8 && i <= 11) && (j >= 8 && j <= 11))
                {
                    cout << "\033[36mˮ\033[0m"; // �??
                }
                else
                    cout << "  "; // 空格
            }
        }
        cout << endl;
    }
}

void saveGame()
{
    // ����ļ�����
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

    // ��յ�ǰ��ͼ���߶���
    fseek(file, 0, SEEK_SET);
    clearerr(file);

    // ��ȡ�Ѷ�
    if (fread(&difficulty, sizeof(int), 1, file) != 1)
    {
        fclose(file);
        return false;
    }

    // ��ȡ��ͼ
    if (fread(map_s, sizeof(int), 400, file) != 400)
    {
        fclose(file);
        return false;
    }

    // ��ȡ����Ϣ
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

    // �������߶���ǰ����ɶ���
    delete snake_ptr;
    snake_ptr = new Snake(head_x, head_y, direction);
    snake_ptr->length = length;

    // ������캯���Զ����ɵĵڶ����ڵ�
    if (snake_ptr->getHead()->next)
    {
        delete snake_ptr->getHead()->next;
        snake_ptr->getHead()->next = nullptr;
    }

    // ���Ĭ�����ɵĽڵ�
    Snake_block *curr = snake_ptr->getHead();
    while (curr->next != nullptr)
    {
        Snake_block *temp = curr->next;
        delete curr;
        curr = temp;
    }

    // �ؽ�����
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

// ����start_info.h���ļ�����
void Draw_print()
{
    system("cls"); // ����
    cout << "̰̰̰̰  ��    ��     ��        ����  Ϸ   Ϸ" << endl;
    cout << "̰        �Գ�  ��   ��  ��    ��      Ϸ  Ϸ  " << endl;
    cout << "̰̰̰̰  �� �� ��  ��    ��  ��       ϷϷ    " << endl;
    cout << "      ̰  ��  �Գ�  ��������   ��      Ϸ  Ϸ  " << endl;
    cout << "̰̰̰̰  ��    ��  ��    ��     ����  Ϸ   Ϸ" << endl;
    cout << endl
         << "                   ���ߣ�����" << endl;
    cout << endl;
    return;
}

// �����Ч���Ĳ˵���ʾ
void info_print()
{
    const string menu[] = {
        "�� �� �� Ϸ",
        "�� �� �� Ϸ",
        "�� Ϸ �� ��",
        "�� �� ѡ ��",
        "�� ʷ �� ¼",
        "�� �� �� Ϸ"};

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

// �����������
int HandleInput()
{
    if (_kbhit() && draw_state == 0)
    {
        char input = _getch();
        if (input == 's' || input == 'S')
        { // ����
            menu_index = (menu_index + 1) % MENU_ITEMS;
        }
        else if (input == 'w' || input == 'W')
        { // ����
            menu_index = (menu_index - 1 + MENU_ITEMS) % MENU_ITEMS;
        }
        else if (input == '\r' || input == '\n')
        { // ȷ��ѡ��
            switch (menu_index)
            {
            case 0:
                return 1;
                break; // ����Ϸ
            case 1:
                return 2; // ������Ϸ
                break;
            case 2:
                return 3; // ��Ϸ����
                break;
            case 3:
                return 4; // �Ѷ�ѡ��
                break;
            case 4:
                return 5; // ��ʷ��¼
                break;
            case 5:
                return 6; // �˳���Ϸ
                break;
            }
        }
    }
    return 0; // Ĭ�Ϸ���0
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
                cout << "����Ϸ������ʼ��" << endl;
                Sleep(1000);
                return 0; // ���ص���Ϸ��ѭ��
            }
            else if (draw_state == 2)
            {
                system("cls");
                if (_access("save.txt", 0) == -1)
                { // ���浵�ļ��Ƿ����
                    cout << "��ǰû�д浵��" << endl;
                    Sleep(1000);
                    menu_index = 0; // �������˵�
                    Draw_print();
                    info_print();
                    continue;
                }
                if (!loadGame())
                { // ���Լ��ش浵
                    cout << "�浵�𻵻����ʧ�ܣ�" << endl;
                    Sleep(1000);
                    menu_index = 0;
                    Draw_print();
                    info_print();
                    continue;
                }
                cout << "���ڼ�����Ϸ..." << endl;
                Sleep(1000);
                return 1; // ������Ϸѭ��
            }
            else if (draw_state == 3)
            {
                system("cls");
                cout << "��Ϸ����" << endl;
                cout << "1. ʹ��WASD�������ߵ��ƶ�����" << endl;
                cout << "2. �Ե�ʳ������ӷ����������������Զ�Ѱ·һ��ʱ�䡣" << endl;
                cout << "3. ����ǽ�ڻ��Լ�������������Ϸ��" << endl;
                cout << "4. ��Ϸ�������Ѷȣ��򵥡��еȡ����ѡ�" << endl;
                cout << "��ģʽ����������" << endl;
                cout << "�е�ģʽ�������ϰ�" << endl;
                cout << "����ģʽ������ˮ����ϰ�" << endl;
                cout << "����������ز˵�..." << endl;
                _getch();
                menu_index = 0; // ���ò˵�����
                Draw_print();
                info_print();
            }
            else if (draw_state == 4)
            {
                int diff_index = difficulty; // ��ǰѡ�е��Ѷ�����
                bool in_difficulty_menu = true;

                while (in_difficulty_menu)
                {
                    system("cls");
                    // ���ƴ������ѶȲ˵�
                    cout << "�Ѷ�ѡ�񣺵�ǰ�Ѷȣ�";
                    switch (difficulty)
                    {
                    case 0:
                        cout << "��\n\n";
                        break;
                    case 1:
                        cout << "�е�\n\n";
                        break;
                    case 2:
                        cout << "����\n\n";
                        break;
                    }

                    cout << (diff_index == 0 ? "  >> 1. �� <<" : "    1. ��") << endl;
                    cout << (diff_index == 1 ? "  >> 2. �е� <<" : "    2. �е�") << endl;
                    cout << (diff_index == 2 ? "  >> 3. ���� <<" : "    3. ����") << endl;
                    cout << "\n������ʾ��W/S-ѡ��  Enter-ȷ��  ESC-����";
                    Sleep(500); // ������ʱ���������ˢ��
                    // ���봦��
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
                            difficulty = diff_index;    // �����Ѷ�
                            in_difficulty_menu = false; // �˳��Ѷ�ѡ��˵�
                            system("cls");
                            cout << "��ѡ���Ѷȣ�";
                            switch (difficulty)
                            {
                            case 0:
                                cout << "��" << endl;
                                break;
                            case 1:
                                cout << "�е�" << endl;
                                break;
                            case 2:
                                cout << "����" << endl;
                                break;
                            }
                            cout << "����������ز˵�..." << endl;
                            _getch(); // �ȴ��û�����
                        }
                        else if (_kbhit() && _getch() == 27) // ESC��
                        {
                            in_difficulty_menu = false; // �������˵�
                        }
                    }
                    draw_state = 0; // �������˵�
                }
                menu_index = 0; // ���ò˵�����
                Draw_print();
                info_print();
            }
            else if (draw_state == 5)
            {
                system("cls");
                cout << "��ʷ��¼��" << endl;
                // ��ȡ����ʾ��ʷ��߷�
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
                    cout << "��ʷ��߷֣�" << max_score << endl;
                }
                else
                {
                    cout << "������ʷ��¼��" << endl;
                }
                cout << "����������ز˵�..." << endl;
                _getch();
                menu_index = 0; // ���ò˵�����
                Draw_print();
                info_print();
            }
            else if (draw_state == 6)
            {
                system("cls");
                cout << "�˳���Ϸ��" << endl;
                exit(0);
            }
            Sleep(10);
        }
    }
}
// ����start_info.h���ļ�����

int main()
{
    game_over = false;
    speed_up = false;
    auto_run = false;

    int start_mode = wait_start();
    hideCursor();

    if (start_mode == 0)
    { // ����Ϸ
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
            cout << "�浵�𻵻����ʧ�ܣ�" << endl;
            Sleep(2000);
            return 0;
        }
        // ����У����ͷλ�úϷ���
        Snake_block *head = snake_ptr->getHead();
        if (head->x <= 0 || head->x >= 19 ||
            head->y <= 0 || head->y >= 19 ||
            map_s[head->x][head->y] != 3)
        {
            system("cls");
            cout << "�浵�����쳣��" << endl;
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
            cout << "��ʤ" << endl;
            break;
        }
        if (game_over)
        {
            system("cls");
            map_draw();
            cout << "��Ϸ�������÷�: " << snake_ptr->length << endl;
            break;
        }
        if (auto_run)
        {
            // �??动�?�路模式
            int food_x = food_ptr->x, food_y = food_ptr->y;
            int dir = snake_ptr->find_path(food_x, food_y);
            if (dir != -1)
            {
                // �??�??为方向字�??
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
                cout << "��·����" << endl;
                auto_run = false;
            }
        }
        else if (_kbhit())
        {
            char input = _getch();
            if (input == 27) // ESC����
            {
                system("cls");
                map_draw();
                cout << "��Ϸ�������÷�: " << snake_ptr->length << endl;
                break;
            }
            if (input == 'T' || input == 't')
            {
                saveGame();
                setCursorPosition(24, 0);
                cout << "��Ϸ�浵�ɹ���" << endl;
                Sleep(1000);
            }
            if (input == 'p' || input == 'P') // ����P����ͣ
            {
                system("cls");
                map_draw();
                cout << "��ͣ�ɹ��������������..." << endl;
                _getch(); // �ȴ��û�����
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

        // 输出游戏状�?
        clk += speed / 1000.0;
        // 处理食物和特殊�?�物

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
        // 处理特殊食物
        if (bene_exist)
        {
            bene_cnt++;
            if (bene_cnt >= 30)
            {
                map_s[bene_ptr->x][bene_ptr->y] = 0; // 清除食物
                bene_exist = false;
                bene_cnt = 0;
            }
        }
        else
        {
            if (bene_ptr == nullptr)
            {
                bene_ptr = new Benefit(0, 0); // ��ʼλ�ûᱻ����ѭ������
            }
            while (true)
            {
                bene_ptr->x = rand() % 18 + 1;
                bene_ptr->y = rand() % 18 + 1;
                if (map_s[bene_ptr->x][bene_ptr->y] == 0)
                    break; // 找到一�??空位�??
            }
            int bene_type = rand() % 30; // 随机生成特殊食物类型
            if (bene_type == 0)
            {
                map_s[bene_ptr->x][bene_ptr->y] = 5; // 星星
                bene_exist = true;
            }
            else if (bene_type == 1)
            {
                map_s[bene_ptr->x][bene_ptr->y] = 6; // 特殊食物
                bene_exist = true;
            }
        }

        if (is_eat)
        {
            map_s[food_ptr->x][food_ptr->y] = 0; // 清除食物
            while (true)
            {
                food_ptr->x = rand() % 18 + 1;
                food_ptr->y = rand() % 18 + 1;
                if (map_s[food_ptr->x][food_ptr->y] == 0)
                    break; // 找到一�??空位�??
            }
            map_s[food_ptr->x][food_ptr->y] = 4; // 新�?�物
            is_eat = false;
        }
        map_draw();
        Sleep(speed); // 动速度
        setCursorPosition(21, 0);
        cout << "��: " << snake_ptr->length << "                  ";
        setCursorPosition(21, 20);
        cout << "ʱ: " << fixed << setprecision(0) << clk << "                  ";
        setCursorPosition(22, 0);

        cout << "��P��ͣ��T�浵��ESC�˳�,�Զ�Ѱ·�ڼ����𰴼�" << endl;
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
            cout << "��ϲ�㴴�����µ���ʷ��߷֣�" << endl;
        }
        fin.close();
    }
    delete snake_ptr;
    delete food_ptr;
    delete bene_ptr;
    system("pause");
    return 0;
}