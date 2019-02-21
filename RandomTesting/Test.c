#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/mman.h>

int main(int argc, char **argv)
{
    printf("PARENT PID: %d\n", getpid());
    for(int i = 0; i < 3; i++)
    {
        fork();
    }
    printf("Parent: %d MyPID: %d\n", getppid(), getpid());
    exit(0);
}