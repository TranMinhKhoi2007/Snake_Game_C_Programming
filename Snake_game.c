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
#include <time.h>
#define cols 20
#define rows 20
#define SNAKE_MAX_LEN 500
#define MAX_FOOD 30
// Tốc độ gane: mỗi vòng lặp nghỉ TICK_MS mili giây
#define TICK_MS 100
// Thức ăn sẽ xuất hiện sau mỗi khoảng [FOOD_MIN_TICK, FOOD_MAX_TICK] tick
#define FOOD_MIN_TICK 30
#define FOOD_MAX_TICK 40
// Hướng di chuyển
#define DIR_UP 0
#define DIR_DOWN 1
#define DIR_LEFT 2
#define DIR_RIGHT 3

// Biến toàn cục
int x, y, gameOver = 0;
char board[cols * rows];
int currentDir = DIR_RIGHT;
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
struct Food
{
    int x, y;
    int active;
};

struct Food food[MAX_FOOD];
int foodCount = 0;
int foodSpawnTimer = 0;

// CONSOLE HELPERS
// Di chuyển con trỏ chuột về gốc (0,0)
void clearScreen(int x, int y)
{
    COORD coord;
    coord.X = x;
    coord.Y = y;
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), coord);
}

void HideCursor()
{
    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 100;
    info.bVisible = FALSE; // FALSE là ẩn con trỏ đi
    SetConsoleCursorInfo(consoleHandle, &info);
}

// Vẽ bản đồ, rắn, thức ăn
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

// Vẽ rắn từ đuôi vẽ về index 0 là đầu rắn
void Draw_snake()
{
    for (int i = snake.length - 1; i > 0; i--)
    {
        board[snake.part[i].y * cols + snake.part[i].x] = 'o';
    }
    board[snake.part[0].y * cols + snake.part[0].x] = '0';
}

void Draw_food()
{
    for (int i = 0; i < MAX_FOOD; i++)
    {
        if (food[i].active)
        {
            board[food[i].y * cols + food[i].x] = '*';
        }
    }
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

// Khởi tạo
void setUp_snake()
{
    snake.length = 1;
    snake.part[0].x = 1 + rand() % (cols - 2);
    snake.part[0].y = 1 + rand() % (rows - 2);
    currentDir = DIR_RIGHT;
}

// Sinh thời gian ngẫu nhiên cho lần spawn kế tiếp
int randomSpawnDelay()
{
    return FOOD_MIN_TICK + rand() % (FOOD_MAX_TICK - FOOD_MIN_TICK + 1);
}

void setUp_food()
{
    for (int i = 0; i < MAX_FOOD; i++)
    {
        food[i].active = 0;
    }
    foodCount = 0;
    // Spawn sẵn 1 trái đầu tiên
    food[0].x = 1 + rand() % (cols - 2);
    food[0].y = 1 + rand() % (rows - 2);
    food[0].active = 1;
    foodCount = 1;
    foodSpawnTimer = randomSpawnDelay();
}

// Tìm 1 vị trí không trùng với thân rắn và không trùng food khác
void spawnRandomFood()
{
    if (foodCount >= MAX_FOOD)
        return; /* het cho */

    int slot = -1;
    for (int i = 0; i < MAX_FOOD; i++)
        if (!food[i].active)
        {
            slot = i;
            break;
        }
    if (slot == -1)
        return;

    int fx, fy, valid;
    do
    {
        valid = 1;
        fx = 1 + rand() % (cols - 2);
        fy = 1 + rand() % (rows - 2);

        // Không trùng với thân rắn
        for (int i = 0; i < snake.length; i++)
            if (snake.part[i].x == fx && snake.part[i].y == fy)
            {
                valid = 0;
                break;
            }

        // Không trùng với các food khác đang còn trên bảng
        if (valid)
            for (int i = 0; i < MAX_FOOD; i++)
                if (food[i].active && food[i].x == fx && food[i].y == fy)
                {
                    valid = 0;
                    break;
                }

    } while (!valid);

    food[slot].x = fx;
    food[slot].y = fy;
    food[slot].active = 1;
    foodCount++;
}

// Gọi mới tick: đếm ngược, hết giờ thì spawn 1 trái mới
void Update_food_timer()
{
    foodSpawnTimer--;
    if (foodSpawnTimer <= 0)
    {
        spawnRandomFood();
        foodSpawnTimer = randomSpawnDelay();
    }
}

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

// Rắn tự động di chuyển theo currentdir
void MoveInCurrentDir()
{
    int dx[] = {0, 0, -1, 1}; // UP, DOWN, LEFT, RIGHT
    int dy[] = {-1, 1, 0, 0};
    Move_snake(dx[currentDir], dy[currentDir]);
}

// Tránh rắn di chuyển 180 độ
int isOpposite(int a, int b)
{
    return (a == DIR_UP && b == DIR_DOWN) ||
           (a == DIR_DOWN && b == DIR_UP) ||
           (a == DIR_LEFT && b == DIR_RIGHT) ||
           (a == DIR_RIGHT && b == DIR_LEFT);
}

void Keyboard_input()
{
    if (!kbhit())
        return;
    int ch = getch();

    int newDir = currentDir;
    switch (ch)
    {
    case 'w':
    case 'W':
        newDir = DIR_UP;
        break;
    case 's':
    case 'S':
        newDir = DIR_DOWN;
        break;
    case 'a':
    case 'A':
        newDir = DIR_LEFT;
        break;
    case 'd':
    case 'D':
        newDir = DIR_RIGHT;
        break;
    default:
        return;
    }

    // Không cho đổi hướng ngược lại 180 độ
    if (!isOpposite(currentDir, newDir))
        currentDir = newDir;
}

void Game_rules()
{
    // Ăn thức ăn
    for (int i = 0; i < MAX_FOOD; i++)
    {
        if (food[i].active &&
            food[i].x == snake.part[0].x && food[i].y == snake.part[0].y)
        {
            food[i].active = 0;
            foodCount--;
            snake.length++;
        }
    }

    // Đụng tường
    if (snake.part[0].x == 0 || snake.part[0].x == cols - 1 ||
        snake.part[0].y == 0 || snake.part[0].y == rows - 1)
    {
        gameOver = 1;
        return;
    }

    // Chạm thân
    for (int i = 1; i < snake.length; i++)
    {
        if (snake.part[0].x == snake.part[i].x &&
            snake.part[0].y == snake.part[i].y)
        {
            gameOver = 1;
            return;
        }
    }
}

int main()
{
    srand((unsigned)time(0));
    setUp_snake();
    setUp_food();
    HideCursor();
    while (!gameOver)
    {
        Keyboard_input();
        MoveInCurrentDir();
        Update_food_timer();
        Game_rules();
        Draw_board();
        Draw_food();
        Draw_snake();
        Print_board();
        printf("\nScore: %d", snake.length * 100);
        Sleep(TICK_MS);
    }
    Draw_board();
    Draw_food();
    Draw_snake();
    Print_board();
    printf("\nGame over, final score: %d", snake.length * 100);
}