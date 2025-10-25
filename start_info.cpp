#include <bits/stdc++.h>
#include <windows.h>
#include <conio.h>
#include "start_info.h"
using namespace std;

// �޸ĺ�Ļ��ƺ���
void Draw_print()
{
    system("cls"); // ����
    cout << "̰̰̰̰  ��    ��     ��        ����  Ϸ   Ϸ" << endl;
    cout << "̰        �Գ�  ��   ��  ��    ��      Ϸ  Ϸ  " << endl;
    cout << "̰̰̰̰  �� �� ��  ��    ��  ��       ϷϷ    " << endl;
    cout << "      ̰  ��  �Գ�  ��������   ��      Ϸ  Ϸ  " << endl;
    cout << "̰̰̰̰  ��    ��  ��    ��     ����  Ϸ   Ϸ" << endl;
    cout << endl
         << endl;
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
                cout << "����������Ϸ�� " << endl;
                Sleep(1000);
                return 1;
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
/*
int main()
{
    wait_start();
    return 0;
}
    */