#include <iostream>
#include <iostream>
#include <windows.h>
#include <locale>
#include <conio.h>
#include <limits>
#include <stack>

#include "functions.h"

/* Отрисовка опоры */
void draw_balk(int x, int y, int delta = 1) {
    int x_new = x;
    int y_new = y;
    for (int i = 0; i < 10; i++) {
        console_gotoxy(x_new, y_new);
        std::cout << '|';
        y_new = y_new + delta;
    }
}

//template <typename T>
class Disk {
public:
    int x_;
    int y_;
    int length_;
    Disk(int length, int x, int y) {
        draw(length, x, y);
    };
    Disk* previous_;

    /* Отрисовка диска */
    void draw(int length, int x, int y, int delta = 1) {
        char ch = 220;
        int x_new = x+1, y_new = y;
        length_ = length;

        for (int i = 0; i < length/2; i++) {
            console_gotoxy(x_new, y_new);
            std::cout << ch;
            x_new = x_new + delta;
        }
        x_new = x;
        for (int i = 0; i < length/2; i++) {
            x_new = x_new - delta;
            console_gotoxy(x_new, y_new);
            std::cout << ch;
        }
        x_ = x; y_ = y;         // Координаты диска

    };
};

/* Класс стека */
class Stack {
    Disk* tos_ = nullptr;

public:
    int count_;
    Stack() {
        count_ = 0;
    };

    void push(int length, int x, int y) {
        if (tos_ == nullptr) {
            tos_ = new Disk(length, x, y);
        }
        else {
            Disk* ptr = new Disk(length, x, y);
            ptr->previous_ = tos_;
            tos_ = ptr;
        }
        count_++;
    };

    Disk* pop() {
        Disk* ptr = tos_;
        tos_ = ptr->previous_;
        count_--;
        return ptr;
    };

    Disk* peek() {
        return tos_;
    };
};

class Tower {
    Stack* s_ptr = nullptr;
    Disk* d_ptr = nullptr;
    int x_start = 12;
    int y_start = 15;
public:
    Tower(int num_, Stack *stek) {
        while (num_ > 0) {
            console_gotoxy(x_start, y_start);
            stek->push(num_ * 2, x_start, y_start);
            num_--;
            y_start--;
        }
    };
};

class Game {

public:
    int x_ = 12;            // Координаты начала игры
    int y_;
    int size_;
    Stack* new_tower;

    Disk* d_ptr = nullptr;
    Stack* s_ptr = nullptr;
    Game(Stack *steck1) {
        s_ptr = steck1;         // Делаем так, чтобы игра начиналась с первой пирамидки
        DrawPointer(x_);
        size_ = s_ptr->count_;
    };

    void Move(Disk *disk_ch, int x, int y) {          // disk_ch - указатель на выбранный диск
        int len = disk_ch->length_;
        disk_ch->draw(len, x, y);
    }
    void ReMove(Disk *disk_ch, int x, int y, int delta = 1) {
        int len = disk_ch->length_;
        char ch = ' ';

        int x_new = (disk_ch -> x_) + 1, y_new = disk_ch -> y_;

        for (int i = 0; i < len / 2; i++) {
            console_gotoxy(x_new, y_new);
            std::cout << ch;
            x_new = x_new + delta;
        }
        x_new = x;
        for (int i = 0; i < len / 2; i++) {
            x_new = x_new - delta;
            console_gotoxy(x_new, y_new);
            std::cout << ch;
        }

    }

    /* Создаем стрелочку, указывающая на выбранную башню */
    void DrawPointer(int x, int y = 17) {
        char ptr = 30;
        console_gotoxy(x, y);
        std::cout << ptr;
    }
        /* Убираем стрелочку */
    void RemovePointer(int x, int y = 17) {
        console_gotoxy(x, y);
        std::cout << ' ';
    }

    void Start(Stack *steck1, Stack *steck2, Stack *steck3) {
        new_tower = steck3;

        console_hide_cursor();
        WORD vk;
        bool isFinish = false;
        bool isUp = false;          // isUp означает, что диск подняли и его можно только опустить или перенести
        bool isDown = true;         // isUp - опустили

        while (!isFinish) {
            if (_kbhit()) {
                vk = console_get_vk();

                switch (vk) {
                case VK_UP:
                    /*if (d_ptr == nullptr)
                        break;*/
                    if (isUp) {         // Если не будет этой штуки с isUp, то будет ошибка
                        break;
                    }

                    if (s_ptr != nullptr) {
                        Disk* d_ch = s_ptr->peek();
                        if (d_ch == nullptr)
                            break;
                        d_ptr = s_ptr->pop();
                        ReMove(d_ptr, x_, y_);
                        y_ = 4;
                        Move(d_ptr, x_, y_);
                        //s_ptr = nullptr;
                        isUp = true;
                        isDown = false;
                    }

                    break;
                case VK_DOWN:
                    if (isDown)
                        break;
                    /* Опускаем вниз диск */
                    if (s_ptr != nullptr && d_ptr != nullptr) {
                        Disk *prev_disk = s_ptr->peek();
                        if (prev_disk == nullptr) {
                            int len = d_ptr->length_;
                            int x_new = d_ptr->x_; int y_new = d_ptr->y_;
                            ReMove(d_ptr, x_, y_);
                            s_ptr->push(len, x_, 15);
                            //s_ptr = nullptr;
                            d_ptr = nullptr;
                            isDown = true;
                            isUp = false;
                            break;
                        }
                        else {
                            if (prev_disk == nullptr)
                                break;
                            int prev_len = prev_disk->length_;
                            int len = d_ptr->length_;
                            if (prev_len > len) {
                                // опустить
                                //std::cout << "DOWN";
                                int x_new = prev_disk->x_; int y_new = prev_disk->y_;
                                ReMove(d_ptr, x_, y_);
                                x_ = x_new; y_ = y_new - 1;
                                Move(d_ptr, x_, y_);
                                s_ptr->push(len, x_, y_);
                                d_ptr = nullptr;
                                isUp = false;
                            }
                        }
                    }


                    break;
                case VK_LEFT:
                    if (d_ptr == nullptr) {
                        if (x_ > 12) {
                            RemovePointer(x_);
                            x_ = x_ - 20;
                            DrawPointer(x_);
                        }
                        switch (x_) {
                        case 12:
                            s_ptr = steck1;
                        break;

                        case 32:
                            s_ptr = steck2;
                        break;
                        }
                    }
                    else
                    {
                        if (x_ > 12) {
                            RemovePointer(x_);
                            ReMove(d_ptr, x_, y_);
                            x_ = x_ - 20;
                            DrawPointer(x_);
                        }
                        Move(d_ptr, x_, y_);
                        switch (x_) {
                        case 12:
                            s_ptr = steck1;
                        break;

                        case 32:
                            s_ptr = steck2;
                        break;
                        }
                    }

                    break;
                case VK_RIGHT:
                    if (d_ptr == nullptr) {
                        if (x_ < 52) {
                            RemovePointer(x_);
                            x_ = x_ + 20;
                            DrawPointer(x_);
                        }
                        switch (x_) {
                        case 32:
                            s_ptr = steck2;
                        break;

                        case 52:
                            s_ptr = steck3;
                        break;

                        }
                    }
                    else {
                        //std::cout << "RIGHT" << std::endl;
                        if (x_ < 52) {
                            RemovePointer(x_);
                            ReMove(d_ptr, x_, y_);
                            x_ = x_ + 20;
                            DrawPointer(x_);
                        }
                        Move(d_ptr, x_, y_);
                        switch (x_) {
                        case 32:
                            s_ptr = steck2;
                        break;

                        case 52:
                            s_ptr = steck3;
                        break;
                        }
                    }

                    break;
                case VK_ESCAPE:
                    console_gotoxy(12, 4);
                    std::cout << " \t\t\t\tGAME OVER ";
                    isFinish = true;
                break;
                }
            }
            int new_size = new_tower->count_;
            if (size_ == new_size) {
                isFinish = true;
                console_gotoxy(12, 4);
                std::cout << " \t\t\t\tYOU WIN ";
            }
        }
    }
};

int main()
{
    int disks_num;
    system("cls");
    std::cout << "Enter number of disks 3-8: ";
    std::cin >> disks_num;
    if (disks_num < 3) {
        std::cout << "Enter more disks";
        return 0;
    }
    if (disks_num > 8) {
        std::cout << "Too much";
        return 0;
    }

    /* Отрисовка балок */
    draw_balk(12, 6);
    draw_balk(32, 6);
    draw_balk(52, 6);

    /* создание стека для первой балки, пирамиды*/
    Stack *s1 = new Stack;
    Tower t_1 (disks_num, s1);       // Помещаем в первый стек исходную пирамиду

    /* создание двух других стеков для оставшихся балок*/
    Stack* s2 = new Stack;
    Stack* s3 = new Stack;

    std::cout << std::endl;
    Game game(s1);
    game.Start(s1, s2, s3);
}
