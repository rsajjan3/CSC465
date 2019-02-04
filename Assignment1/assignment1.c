#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
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
//https://stackoverflow.com/questions/5656530/how-to-use-shared-memory-with-linux-in-c
void problem2()
{

}
int main(int argc, char **argv)
{
    return 0;
}