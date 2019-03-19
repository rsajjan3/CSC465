#include <pthread.h>
#include <stdlib.h>
#include <stdio.h> 
#include <time.h>
#include <math.h>

#define LETTER_COUNT 26
#define MATRIX_SIZE 7
#define THREAD_MAX (MATRIX_SIZE * 2)

static struct { int index; int count; char letter; } winners[2];

char matrix[MATRIX_SIZE][MATRIX_SIZE];
int letter_count[THREAD_MAX][LETTER_COUNT];
int thread_num = 0, row_num = 0, col_num = 0;

void* counter(void* arg)
{
    char letter;
    if(thread_num % 2 == 0) //Row
    {
        for(int i = 0; i < MATRIX_SIZE; i++)
        {
            letter = matrix[row_num][i];
            int letter_idx = (int)letter - 65;
            letter_count[thread_num][letter_idx]++;
        }
        row_num++;
    }
    else //Col
    {
        for(int i = 0; i < MATRIX_SIZE; i++)
        {
            letter = matrix[i][col_num];
            int letter_idx = (int)letter - 65;
            letter_count[thread_num][letter_idx]++;
        }
        col_num++;
    }
    thread_num++;
}

void fillarrays()
{
    char letter;
    srand(time(NULL));

    //Fill matrix with random letters
    for(int i = 0; i < MATRIX_SIZE; i++)
    {
        for(int j = 0; j < MATRIX_SIZE; j++)
        {
            letter = (rand() % LETTER_COUNT) + 65; //Capital letters in ASCII start at 65
            matrix[i][j] = letter;
        }
    }

    //Fill counter with 0s before starting
    for(int i = 0; i < MATRIX_SIZE * 2 ; i++)
    {
        for(int j = 0; j < LETTER_COUNT; j++)
        {
            letter_count[i][j] = 0;
        }
    }
}
int main(int argc, char **argv)
{
    fillarrays();

    pthread_t threads[THREAD_MAX];
    for(int i = 0; i < THREAD_MAX; i ++)
    {
        pthread_create(&threads[i], NULL, counter, (void *) NULL);
    }
    for(int i = 0; i < THREAD_MAX; i++)
    {
        pthread_join(threads[i], NULL);
    }

    int winners_idx = 0;
    for(int i = 0; i < THREAD_MAX; i++)
    {
        winners_idx = i % 2; //0: Row, 1: Col
        for(int j = 0; j < LETTER_COUNT; j++)
        {
            if(letter_count[i][j] > winners[winners_idx].count) //TODO: Figure out how to save all the ties
            {
                winners[winners_idx].index =  floor(i/2) + 1;
                winners[winners_idx].count = letter_count[i][j];
                winners[winners_idx].letter = (char) j + 65;
            }
        }
    }

    for(int i = 0; i < MATRIX_SIZE; i++)
    {
        for(int j = 0; j < MATRIX_SIZE; j++)
        {
            printf("%c ", matrix[i][j]);
        }
        printf("\n");
    }
    
    printf("Indices start at 1\n");
    printf("Row %d: %d occurrences of letter %c\n", winners[0].index, winners[0].count, winners[0].letter);
    printf("Column %d: %d occurrences of letter %c\n", winners[1].index, winners[1].count, winners[1].letter);
    return 0;
}