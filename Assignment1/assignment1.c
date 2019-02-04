#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>

const char *Situation[6] = {"The light is turning green", "There is a pedestrian in front of me", "The car in front of me just stopped", "The car in front of me is moving", "The road is turning to the right", "\0"};
const char *Recommendation[6] = {"Press the accelerator", "Press the break", "Press the break", "Press the accelerator", "Steer right", "\0"};
const char *Action[5] = {"Done", "Oops, I pressed the break instead", "Oops, I pressed the accelerator instead", "Oops, I steered left", "\0" };

void problem1()
{
    pid_t childPID = 0;
    printf("[Level0]myPID: %d, parentPID: %d\n", getpid(), getppid());
    for(int level1 = 0;level1 < 4; level1++) // Create 4 processes at level 1
    {
        childPID = fork();
        if(childPID < 0) printf("[Level1] RIP Fork\n");
        else if(childPID == 0) //Level1 Child
        { 
            printf("\t[Level1]myPID: %d, parentPID: %d\n",getpid(),getppid());

            for(int level2 = 0; level2 < level1; level2++) { //Create 5 processes at level 2
                if(level1 > 1)
                {
                    childPID = fork();
                    if(childPID < 0) printf("[Level2] RIP Fork");
                    else if(childPID == 0) //Level2 Child
                    {
                        printf("\t\t[Level2]myPID: %d, parentPID: %d\n",getpid(),getppid());

                        for(int level3 = 0; level3 < level2; level3++) { //Create 2 processes at level 3
                            if(level1 > 2)
                            {
                                childPID = fork();
                                if(childPID < 0) printf("[Level3] RIP Fork");
                                else if(childPID == 0) //Level 3 Child
                                {
                                    printf("\t\t\t[Level3]myPID: %d, parentPID: %d\n",getpid(),getppid());
                                }
                                else //Level3 Parent
                                {
                                    wait(0);
                                }
                                exit(0);
                            }
                        }
                        exit(0);
                    }
                    else //Level2 Parent
                    { 
                        wait(0);
                    }
                }
            }
            exit(0);
        }
        else //Level1 Parent
        {
            wait(0);
        }
    }
}

void problem2()
{
    //{Message_Index, 0: Parent_Ready 1: Child_Ready}
    int messenger[2] = {0, 0};
    //mmap is the newer way to do shared memory vs shmget
    int *shmem = mmap(NULL, sizeof(messenger), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, 0, 0);
    memcpy(shmem, messenger, sizeof(messenger)); //Copy messenger variable into shared memory

    if(fork() == 0) //Child
    {
        srand(time(NULL));
        int choice = rand() % 5;

        messenger[0] = choice; //Picking the Situation
        messenger[1] = 1; //Child_Ready
        printf("Student: %s\n", Situation[choice]);
        memcpy(shmem, messenger, sizeof(messenger));

        while(shmem[1] != 0) { sleep(.75); } //Waiting for parent to be ready

        //Read in info
        if ((rand() % 2) == 0) //0: Student performs correct action
        {
            messenger[0] = 0; //Responding "Done"
            messenger[1] = 1; //Child Ready
            printf("Student: %s\n", Action[messenger[0]]);
            memcpy(shmem, messenger, sizeof(messenger));
        }
        else //1: Student performs wrong action
        {
            choice = shmem[0]; //Recommendation
            if (choice == 0) messenger[0] = 1; //Picking the wrong action based on recommendation
            else if (choice == 1) messenger[0] = 2;
            else if (choice == 2) messenger[0] = 2;
            else if (choice == 3) messenger[0] = 1;
            else if (choice == 4) messenger[0] = 3;
            messenger[1] = 1; //Child Ready
            printf("Student: %s\n", Action[messenger[0]]);
            memcpy(shmem, messenger, sizeof(messenger));
        }
        exit(0);
    }
    else //Parent
    {
        while(shmem[1] != 1) { sleep(.75); } //Waiting for child to be ready.

        //Read in info
        int choice = shmem[0]; //Situation
        messenger[0] = choice; //Recommendation. Situation and Recommendation indicies match up
        messenger[1] = 0; //Parent_Ready
        printf("Teacher: %s\n", Recommendation[choice]);
        memcpy(shmem, messenger, sizeof(shmem));
        wait(0); //Wait for student to finish

        //Student finished, print how the teacher did
        if(shmem[0] == 0) printf("Teacher: What a good teacher I am!\n");
        else printf("Teacher: What a lousy teacher I am!\n");
    }
}

int main(int argc, char **argv)
{
    //problem1();
    //printf("\n\n\n");
    problem2();
    return 0;
}