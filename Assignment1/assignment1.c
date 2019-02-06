#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>

#define PROC1_READ 0
#define PROC2_WRITE 1
#define PROC2_READ 2
#define PROC1_WRITE 3
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

void problem3()
{
    int fd[4];
    int length = 0;
    //{Message_Index, 0: Parent_Ready 1: Child_Ready}
    int messenger[2] = {0, 0};
    for(int i = 0; i < 2; i++)
    {
        if(pipe(fd + (i*2)) < 0)
        {
            printf("RIP pipes");
            exit(1);
        }
    }
    if(fork() == 0) //Child
    {
        //Close uneeded stuff
        close(fd[PROC1_READ]);
        close(fd[PROC1_WRITE]);

        srand(time(NULL));
        int choice = rand() % 5;

        messenger[0] = choice; //Picking the situation
        messenger[1] = 1; //Child_Ready
        printf("Student: %s\n", Situation[choice]);
        if (write(fd[PROC2_WRITE], &messenger, sizeof(messenger)) != sizeof(messenger))
        {
            printf("Student: FAILURE TO SEND\n");
            exit(1);
        }

        length = read(fd[PROC2_READ], &messenger, sizeof(messenger));
        if(length > 0)
        {
            //Read in info
            if ((rand() % 2) == 0) //0: Student performs correct action
            {
                messenger[0] = 0; //Responding "Done"
                messenger[1] = 1; //Child_Ready
                printf("Student: %s\n", Action[messenger[0]]);
                if(write(fd[PROC2_WRITE], &messenger, sizeof(messenger)) != sizeof(messenger))
                {
                    printf("Student: FAILURE TO SEND2\n");
                    exit(1);
                }
            }
            else //1: Student performs wrong action
            {
                choice = messenger[0]; //Recommendation
                if (choice == 0) messenger[0] = 1; //Picking the wrong action based on recommendation
                else if (choice == 1) messenger[0] = 2;
                else if (choice == 2) messenger[0] = 2;
                else if (choice == 3) messenger[0] = 1;
                else if (choice == 4) messenger[0] = 3;
                messenger[1] = 1; //Child Ready
                printf("Student: %s\n", Action[messenger[0]]);
                if(write(fd[PROC2_WRITE], &messenger, sizeof(messenger)) != sizeof(messenger))
                {
                    printf("Student: FAILURE TO SEND2\n");
                    exit(1);
                }   
            }
        }
        close(fd[PROC2_READ]);
        close(fd[PROC2_WRITE]);
        exit(0);
    }
    else //Parent
    {
        //Close uneeded stuff
        close(fd[PROC2_READ]);
        close(fd[PROC2_WRITE]);

        //First message from student
        length = read(fd[PROC1_READ], &messenger, sizeof(messenger));
        if(length > 0)
        {
            //Read in info
            int choice = messenger[0]; //Situation
            messenger[0] = choice; //Recommendation. Situation and Recommendation indicies match up. Line is un-needed, but is nice for seeing what's going on
            messenger[1] = 0; //Parent_Ready
            printf("Teacher: %s\n", Recommendation[choice]);
            if (write(fd[PROC1_WRITE], &messenger, sizeof(messenger)) != sizeof(messenger))
            {
                printf("Teacher: FAILURE TO SEND\n");
                exit(1);
            }
            close(fd[PROC1_WRITE]); //No longer need to write
        }

        //Second message from student
        length = read(fd[PROC1_READ], &messenger, sizeof(messenger));
        if(length > 0)
        {
            wait(0); //Wait for student to finish
            if(messenger[0] == 0) printf("Teacher: What a good teacher I am!\n");
            else printf("Teacher: What a lousy teacher I am!\n");
            close(fd[PROC1_READ]); //Done reading
        }
    }
}
int main(int argc, char **argv)
{
    problem1();
    printf("\n\n\n");
    problem2();
    printf("\n\n\n");
    sleep(1); //Without sleep, the rand produces the same value for both
    problem3();
    return 0;
}