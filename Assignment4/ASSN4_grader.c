#include <stdio.h>
#include "ASSN4_declarations.h"
// gcc -c ASSN4_grader.c

static int i=-1;

int p1[size][2] = {{1,5},{2,10},{3,3}, {4,2}};
int p2[size][2] = {{1,15},{2,10},{3,5}};
int p3[size][2] = {{1,5},{2,5},{3,5}};
int p4[size][2] = {{1,4},{2,4}};
int p5[size][2] = {{1,15},{2,10}};
int p6[size][2] = {{1,3},{2,6},{3,0}};
int p7[size][2] = {{1,5}};
int p8[size][2] = {{1,5},{2,9}, {3,5}};


Process i1 = {.pid = 1, .status = 0, .segments = &p1};
Process i2 = {.pid = 2, .status = 0, .segments = &p2};
Process i3 = {.pid = 1, .status = 2, .segments = &p1};
Process i4 = {.pid = 3, .status = 0, .segments = &p3};
Process i5 = {.pid = 4, .status = 0, .segments = &p4};
Process i6 = {.pid = 2, .status = 2, .segments = &p2};
Process i7 = {.pid = 3, .status = 3, .segments = &p3};
Process i8 = {.pid = 2, .status = 1, .segments = &p2};
Process i9 = {.pid = 5, .status = 0, .segments = &p5};
Process i10 = {.pid = 6, .status = 0, .segments = &p6};
Process i11 = {.pid = 7, .status = 0, .segments = &p7};
Process i12 = {.pid = 5, .status = 2, .segments = &p5};
Process i13 = {.pid = 4, .status = 3, .segments = &p4};
Process i14 = {.pid = 8, .status = 0, .segments = &p8};
Process null = {.pid = -1};


/*******************************************************/

//0=new, 1=in, 2=out, 3=terminated
Process get_input()
{
    i++;
    Process instructions[16] = {i1,i2,i3,i4,i5,i6,i7,i8,i9,i10,i11,i12,i13,i14,null};
    return instructions[i];
}
void printer(int (*p)[][2])
{
    int i=0,j=0;
    for(i;i<1;i++) {
        for(j; j<size*2;) {
            printf("(%2d, %2d) ", (*p)[i][j], (*p)[i][j+1]);
            j=j+2;
        }
    }
    printf("\n");

}

/* Acept a pointer to a linked list. Check the linked list vs the correct list */
void done (void *x)
{
    ;
}
