#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <ctype.h>


/* the program execution starts here */
int main(int argc, char **argv)
{
    char    *program;

    if (argc != 2) {
        printf("Usage: mon fileName\n where fileName is an executable file.\n");
        exit(-1);
    } else
        program = argv[1];

    //1. launch the program specified by the variable 'fileName' and get its pid
    pid_t pid = fork();
    if(pid < 0) {
        printf("\r\nRIP Fork");
        exit(-1);
        }
    else if(pid == 0){  //Child
        execlp(program, program, (char *) NULL);
        exit(0);
    }
    else { //Parent
        //wait(NULL);
        printf("\nThis is the parent");

        //2. launch 'procmon pid' where pid is the pid of the program launched in step 1
        char pid_str[6];
        sprintf(pid_str, "%d", pid);
        execlp("./procmon", "./procmon", pid_str, (char *)NULL);

        //3. wait for 20 seconds
        sleep(20);
        printf("\nThis is the parent again");

        //4. kill the first program
        kill(pid, SIGKILL);

        //5. wait for 2 seconds
        sleep(2);
    
        //6. kill the procmon
        //kill();
        exit(0);
    }
}
