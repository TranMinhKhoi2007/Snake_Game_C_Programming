/*
    # # # # # # # # # # # # #
    #                       #
    #                       #
    #  0  @***              #
    #        *              #
    #                       #
    #                       #
    #                       #
    #                       #
    #                       #
    #                       #
    # # # # # # # # # # # # #
*/
#include <stdio.h>

#define cols 15
#define rows 15
int x, y, baitX, baitY, gameOver = 0;
char board[cols*rows];

void Draw_board()
{
    for (y = 0; y < rows; y++)
    {
        for (x = 0; x < cols; x++)
        {
            if(x==0||y==0||x==cols-1||y==rows-1)
            {
                board[y*cols + x] = '#';
            }
            else 
            {
                board[y*cols + x] = ' ';
            }
        }
    }
}

void Print_board()
{
    for (y = 0; y < rows; y++)
    {
        for (x = 0; x < cols; x++)
        {
            putchar(board[y*cols + x]);
        }
        printf("\n");
    }
}

int main()
{
    Draw_board();
    Print_board();
    
}