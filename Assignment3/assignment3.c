//Author: Ravi Sajjan
/*
    Notes: Program does not produce the correct output because the limit for machineD taking from machineB is not coded in
           Everything else works as expected
    Compile/Run: gcc -pthread  assignment3.c Queue.c ASSN3_grader.o -o assignment3.out -lm  && ./assignment3.out
*/

#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <semaphore.h>
#include "Queue.h"
#include "Sausage.h"
#include "ASSN3_declarations.h"

#define TRUE 1
#define FALSE 0
#define C_NUM_SAUSAGES 100
#define D_NUM_SAUSAGES 400

Queue Q1, Q2, Q3, Q4, Q5;
int howManyC, isCFinished;
int mixtureA, mixtureB, totalMixture;
sem_t ABProduced, CDProduced, CProduced4, EFMutex;
sem_t Q3Mutex, Q3Num, Q4Mutex, Q4Num, Q5Mutex, Q5Num;
int input[2], output[2][2];

void initialize()
{
    printf("Entering initialize");

    srand(time(NULL));
    get_input(&input);

    mixtureA = input[0];
    mixtureB = input[1];
    totalMixture = mixtureA + mixtureB;

    sem_init(&Q3Mutex, 0, 1);
    sem_init(&Q4Mutex, 0, 1);
    sem_init(&Q5Mutex, 0, 1);
    sem_init(&Q3Num, 0, 0);
    sem_init(&Q4Num, 0, 0);
    sem_init(&Q5Num, 0, 0);
    sem_init(&ABProduced, 0, 0);
    sem_init(&CDProduced, 0, 0);
    sem_init(&CProduced4, 0 , 0);
    sem_init(&EFMutex, 0, 1);

    Queue_Init(&Q1, sizeof(int));
    Queue_Init(&Q2, sizeof(int));
    Queue_Init(&Q3, sizeof(struct Sausage));
    Queue_Init(&Q4, sizeof(struct Sausage));
    Queue_Init(&Q5, sizeof(struct Sausage));

    //Fill queues up
    printf("Filling Q1\n");
    for (int i = 0; i < mixtureA; i++)
    {
        enqueue(&Q1, &i);
    }

    printf("Filling Q2\n");
    for(int i = 0; i < mixtureB; i++)
    {
        enqueue(&Q2, &i);
    }

    printf("Exiting initialize\n");
}

void destroy()
{
    sem_destroy(&Q3Mutex);
    sem_destroy(&Q4Mutex);
    sem_destroy(&Q5Mutex);
    sem_destroy(&Q3Num);
    sem_destroy(&Q4Num);
    sem_destroy(&Q5Num);
    sem_destroy(&ABProduced);
    sem_destroy(&CDProduced);
    sem_destroy(&CProduced4);
    sem_destroy(&EFMutex);
    
    Queue_Clear(&Q1);
    Queue_Clear(&Q2);
    Queue_Clear(&Q4);
    Queue_Clear(&Q5);
}

void *machineA(void *arg)
{
    int mixtureCount = 0;
    printf("Entering machine A\n");
    while (mixtureCount < mixtureA)
    {
        int val;
        dequeue(&Q1, &val);

        struct Sausage mixture;
        mixture.type = LEAN;

        sem_wait(&Q3Mutex); //Lock Q3

        enqueue(&Q3, &mixture); //Critical section

        sem_post(&Q3Mutex); //Free Q3
        sem_post(&Q3Num); //Q3 Consumable
        sem_post(&ABProduced); //A/B has produced something, C/D can start

        mixtureCount++;
    }
    printf("Exiting machine A\n");
}

void *machineB(void *arg)
{
    int mixtureCount = 0;
    printf("Entering machine B\n");
    while (mixtureCount < mixtureB)
    {
        int val;
        dequeue(&Q2, &val);

        struct Sausage mixture;
        mixture.type = EXTRA_LEAN;

        sem_wait(&Q3Mutex); //Lock Q3

        enqueue(&Q3, &mixture); //Critical section

        sem_post(&Q3Mutex); //Free Q3
        sem_post(&Q3Num); //Q3 Consumable
        sem_post(&ABProduced); //A/B has produced something, C/D can start

        mixtureCount++;
    }
    printf("Exiting machine B\n");
    sem_destroy(&ABProduced);
}

void *machineC(void *arg)
{
    sem_wait(&ABProduced); //Wait for A/B to produce something before starting machine
    int largeSausages = 0;
    printf("Entering machine C\n");
    while(largeSausages < (3*totalMixture/4)) //C is supposed to produce 3/4 of the total
    {
        struct Sausage value;

        if (howManyC % 4 == 0) sem_post(&CProduced4); //Unlock when C has produced 4 items
        sem_wait(&Q3Num); //Wait for Q3 to be consumable
        sem_wait(&Q4Mutex); //Lock Q4
        sem_wait(&Q3Mutex); //Lock Q3

        dequeue(&Q3, &value); //Critical section
        
        sem_post(&Q3Mutex);   //Free Q3

        for(int i = 0; i < C_NUM_SAUSAGES; i++)
        {
            enqueue(&Q4, &value); //Critical section
        }

        sem_post(&Q4Mutex); //Free Q4
        sem_post(&Q4Num); //Q4 Consumable
        sem_post(&CDProduced); //C/D has produced something, E/F can start
        howManyC++;
        largeSausages++;
    }
    isCFinished = TRUE;
    sem_post(&CProduced4);
    printf("Exiting machine C\n");
}

void *machineD(void *arg)
{
    sem_wait(&ABProduced); //Wait for A/B to produce something before starting machine
    int smallSausages = 0;
    printf("Entering machine D\n");
    while(smallSausages < totalMixture/4)
    {
        struct Sausage value;

        if (isCFinished == FALSE) sem_wait(&CProduced4);
        sem_wait(&Q3Num); //Wait for Q3 to be consumable
        sem_wait(&Q5Mutex); //Lock Q5
        sem_wait(&Q3Mutex); //Lock Q3

        dequeue(&Q3, &value); //Critical section

        sem_post(&Q3Mutex);   //Free Q3

        for (int i = 0; i < D_NUM_SAUSAGES; i++)
        {
            enqueue(&Q5, &value); //Critical section
        }

        
        sem_post(&Q5Mutex); //Free Q4
        sem_post(&Q5Num); //Q5 Consumable
        sem_post(&CDProduced); //C/D has produced something, E/F can start

        smallSausages++;
    }
    printf("Exiting machine D\n");
}

void *machineE(void *arg)
{
    sem_wait(&CDProduced); //Wait for C/D to produce something before starting machine
    int largePackages = 0;
    printf("Entering machine E\n");
    while (largePackages < (3 * totalMixture / 4))
    {
        struct Sausage value;
        
        sem_wait(&Q4Num); //Wait for Q4 to be consumable
        sem_wait(&Q4Mutex); //Lock Q4
        sem_wait(&EFMutex); //Lock output

        dequeue(&Q4, &value); //Critical section

        if (value.type == LEAN) output[0][0]++; //Critical section
        else output[1][0]++;

        sem_post(&EFMutex); //Free output

        for(int i = 1; i < C_NUM_SAUSAGES; i++) //Start at 1 because dequeue above
        {
            dequeue(&Q4, &value);
        }

        sem_post(&Q4Mutex); //Free Q4

        largePackages++;
    }
    sem_post(&EFMutex);
    printf("Exiting machine E\n");
}

void *machineF(void *arg)
{
    sem_wait(&CDProduced);
    int smallPackages = 0;
    printf("Entering machine F\n"); //Wait for C/D to produce something before starting machine
    while (smallPackages < totalMixture/4)
    {
        struct Sausage value;
        
        sem_wait(&Q5Num); //Wait for Q5 to be consumable
        sem_wait(&Q5Mutex); //Lock Q5
        sem_wait(&EFMutex); //Lock output

        dequeue(&Q5, &value); //Critical section

        if (value.type == LEAN) output[0][1]++; //Critical section
        else output[1][1]++;

        sem_post(&EFMutex); //Free output

        for (int i = 1; i < D_NUM_SAUSAGES; i++) //Start at 1 because dequeue above
        {
            dequeue(&Q5, &value);
        }
        
        sem_post(&Q5Mutex); //Free Q5
        smallPackages++;
    }
    sem_post(&EFMutex);
    printf("Exiting machine F\n");
}

int main(int argc, char **argv)
{
    initialize();

    pthread_t threads[6];

    pthread_create(&threads[0], NULL, machineA, (void *)NULL);
    pthread_create(&threads[1], NULL, machineB, (void *)NULL);
    pthread_create(&threads[2], NULL, machineC, (void *)NULL);
    pthread_create(&threads[3], NULL, machineD, (void *)NULL);
    pthread_create(&threads[4], NULL, machineE, (void *)NULL);
    pthread_create(&threads[5], NULL, machineF, (void *)NULL);
    for (int i = 0; i < 6; i++)
    {
        pthread_join(threads[i], NULL);
    }
    destroy();
    return done(&output);
}