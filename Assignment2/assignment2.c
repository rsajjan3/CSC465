#include <pthread.h>
#include <stdlib.h>
#include <stdio.h> 
#include <time.h>

#define MATRIX_SIZE 7
#define THREAD_MAX (MATRIX_SIZE * 2)

static struct { int intVal; char charVal; } winners[2];

char matrix[MATRIX_SIZE][MATRIX_SIZE];
int letter_count[THREAD_MAX][26];
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

int fill_arrays()
{
    char letter;
    srand(time(NULL));

    //Fill matrix with random letters
    for(int i = 0; i < MATRIX_SIZE; i++)
    {
        for(int j = 0; j < MATRIX_SIZE; j++)
        {
            letter = (rand() % 26) + 65; //Capital letters in ASCII start at 65
            matrix[i][j] = letter;
        }
    }

    //Fill counter with 0s before starting
    for(int i = 0; i < MATRIX_SIZE * 2 ; i++)
    {
        for(int j = 0; j < 26; j++)
        {
            letter_count[i][j] = 0;
        }
    }
}
int main(int argc, char **argv)
{
    fill_arrays();

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
        for(int j = 0; j < 26; j++)
        {
            if(letter_count[i][j] > winners[winners_idx].intVal) //TODO: Figure out how to save all the ties
            {
                winners[winners_idx].intVal = letter_count[i][j];
                winners[winners_idx].charVal = (char) j + 65;
            }
        }
    }
    printf("Row %d: %d occurrences of letter %c\n", 0, winners[0].intVal, winners[0].charVal);
    printf("Column %d: %d occurrences of letter %c\n", 0, winners[1].intVal, winners[1].charVal);
    return 0;
}