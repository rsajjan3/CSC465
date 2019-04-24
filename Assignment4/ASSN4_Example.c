#include <stdio.h>
#include "ASSN4_declarations.h"

// run with gcc ASSN4_Example.c ASSN4_grader.o -o ASSN4_Example; ./ASSN4_Example
void main()
{
    printf("%3s, %7s %20s\n", "pid", "status", "---------------segments---------------" );
    Process x = get_input();
    /* Call get_input function in a loop to get all the input
    Use the test x.pid!=-1 to see when the input is over */ 
    while (x.pid!=-1) {
        printf("%3d, %7d, ", x.pid, x.status);
        printer(x.segments);
        x=get_input();
    }

}