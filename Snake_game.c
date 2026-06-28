/*
    # # # # # # # # # # # # #
    #                       #
    #                       #
    #  *  0ooo              #
    #                       #
    #                       #
    #                       #
    #                       #
    #                       #
    #                       #
    #                       #
    # # # # # # # # # # # # #
*/
#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <windows.h>
#define cols 15
#define rows 15
#define SNAKE_MAX_LEN 170
#define foods 10
int x, y, foodX, foodY, gameOver = 0;
char board[cols * rows];

void Draw_board()
{
    for (y = 0; y < rows; y++)
    {
        for (x = 0; x < cols; x++)
        {
            if (x == 0 || y == 0 || x == cols - 1 || y == rows - 1)
            {
                board[y * cols + x] = '#';
            }
            else
            {
                board[y * cols + x] = ' ';
            }
        }
    }
}

// Di chuyển con trỏ chuột về gốc (0,0)
void clearScreen(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void Print_board()
{
    clearScreen(0, 0);
    for (y = 0; y < rows; y++)
    {
        for (x = 0; x < cols; x++)
        {
            putchar(board[y * cols + x]);
        }
        printf("\n");
    }
}
typedef struct SnakePart
{
    int x, y;
} SnakePart;

struct snake
{
    int length;
    SnakePart part[SNAKE_MAX_LEN];
};

struct snake snake;

void Move_snake(int deltaX, int deltaY)
{
    for (int i = snake.length - 1; i > 0; i--)
    {
        // Phần thân di chuyển theo
        snake.part[i] = snake.part[i - 1];
    }
    // Di chuyển đầu rắn
    snake.part[0].x += deltaX;
    snake.part[0].y += deltaY;
}

// Vẽ rắn từ đuôi vẽ về index 0 là đầu rắn 
void Draw_snake()
{
    for (int i = snake.length - 1; i > 0; i--)
    {
        board[snake.part[i].y * cols + snake.part[i].x] = 'o';
    }
    board[snake.part[0].y * cols + snake.part[0].x] = '0';
}


struct Food 
{
    int x,y;
    int consumed;
};
struct Food food[foods];

void Draw_food()
{
    for(int i=0;i<foods;i++)
    {
        // Nếu thức ăn chưa đc ăn
        if(!food[i].consumed)
        {
            board[food[i].y*cols + food[i].x] = '*';
        }
    }
}

void setUp_food()
{
    for(int i=0;i<foods;i++)
    {
        food[i].x = 1 + rand() %(cols-2);
        food[i].y = 1 + rand() %(rows-2);
        food[i].consumed = 0;
    }
}

void setUp_snake()
{
    snake.length = 1;
    snake.part[0].x = 1 + rand() %(cols-2);
    snake.part[0].y = 1 + rand() %(rows-2);
}

void Keyboard_input()
{
    int ch = getch();
    switch (ch)
    {
    case 'w':
        Move_snake(0, -1);
        break;
    case 's':
        Move_snake(0, 1);
        break;
    case 'a':
        Move_snake(-1, 0);
        break;
    case 'd':
        Move_snake(1, 0);
        break;
    }
}

int main()
{
    srand(time(0));
    setUp_snake();
    setUp_food();

    while (!gameOver)
    {
        Draw_board();
        Draw_food();
        Draw_snake();
        Print_board();
        Keyboard_input();
    }
}