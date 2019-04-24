//Compile+Run: gcc assignment4.c LinkedList.c ASSN4_grader.o -o assignment4.out && ./assignment4.out

#include <stdlib.h>
#include <stdio.h>
#include "LinkedList.h"
#include "ASSN4_declarations.h"


#define MEM_SIZE 200
#define FALSE 0
#define TRUE 1

int **SegTable;
NODE *MemHoles;

void initMem()
{
    Segment newSeg;
    newSeg.base = 0;
    newSeg.sizee = 200;

    MemHoles = create_node(newSeg);
}

void combineHoles()
{
    int combined = FALSE;
    for (NODE *current = MemHoles; current->next != NULL; current = current->next) {
        int baseAddr = current->data.base;
        int procSize = current->data.sizee;

        if((baseAddr+procSize) == current->next->data.base)
        {
            Segment data;
            data.base = current->next->data.base;
            data.sizee = current->next->data.sizee;
            MemHoles = delete_node(MemHoles, data);

            data.base = baseAddr;
            data.sizee = procSize+data.sizee;
            set_node_val(current, data);
            combined = TRUE;
        }
    }
    if(combined) combineHoles(); //Keep combining until can't
}

//For debugging
void PrintSegTable(int rows)
{
    printf("SEG TABLE: ");
    for(int i = 0; i < rows; i++)
    {
        printf("(Address: %d|Size: %d) ", SegTable[i][0], SegTable[i][1]);
    }
    printf("\n");
}

int **MakeSegTable(int rows, int cols)
{
    //Creating/Using 2D array: https://stackoverflow.com/questions/3911400/how-to-pass-2d-array-matrix-in-a-function-in-c
    int **tmp;
    tmp = malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++)
    {
        tmp[i] = malloc(cols * sizeof(int));
    }
    return tmp;
}

int countSegments(Process proc)
{
    int segCount = 0;
    while((*proc.segments)[segCount][0] != 0 && (*proc.segments)[segCount][1])
    {
        segCount++;
    }
    return segCount;
}

void addEntry(Process proc, int** tbl, int count)
{
    NODE *currentHole = MemHoles;
    if(currentHole == NULL)
    {  
        printf("Memory Table is broken\n");
        exit(EXIT_FAILURE);
    }
    else
    {
        for(int i = 0; i < count; i++)
        {
            currentHole = MemHoles;
            while(currentHole != NULL)
            {
                int baseAddr = currentHole->data.base;
                int holeSize = currentHole->data.sizee;
                int procSize = (*proc.segments)[i][1];
                if (holeSize >= procSize)
                {
                    Segment newSeg;
                    int newBase = baseAddr + procSize;
                    int newSize = holeSize - procSize;

                    newSeg.base = newBase;
                    newSeg.sizee = newSize;

                    tbl[i][0] = baseAddr;
                    tbl[i][1] =  procSize;
                    if(newSize != 0) 
                    {
                        set_node_val(currentHole, newSeg);
                    }
                    else
                    {
                        MemHoles = delete_node(MemHoles, currentHole->data);
                    }
                    break;
                }
                currentHole = currentHole->next; 
            }
        }
    }
}

void removeEntry(int ** tbl, int count)
{
    for(int i = 0; i < count; i++)
    {
        Segment data;
        data.base = tbl[i][0];
        data.sizee = tbl[i][1];

        MemHoles = append_node(MemHoles, create_node(data));
        tbl[i][0] = -1; //Mark empty
    }
    combineHoles();
}

int main(int argc, char **argv)
{
    initMem();
    int *segTablePtr[20];
    Process proc = get_input(); //status(0: New, 1: In, 2: Out, 3: Terminated)
    while (proc.pid != -1)
    {
        printf("%3s, %7s %20s\n", "pid", "status", "---------------segments---------------" );
        printf("%3d, %7d, ", proc.pid, proc.status);
        printer(proc.segments);

        int segCount = countSegments(proc);
        if (proc.status == 0) //New proc
        {
            SegTable = MakeSegTable(segCount, 2); //Malloc a seg table to the size of counter
            addEntry(proc, SegTable, segCount);
            segTablePtr[proc.pid] = SegTable;
        }
        else if(proc.status == 1) //In
        {
            SegTable = segTablePtr[proc.pid];
            addEntry(proc, SegTable, segCount);
            segTablePtr[proc.pid] = SegTable;
        }
        else if(proc.status == 2) //Out
        {
            SegTable = segTablePtr[proc.pid];
            removeEntry(SegTable, segCount);
            printf("Process %d’s segment table entries have been set to -1\n", proc.pid);
        }
        else if(proc.status == 3) //Terminated
        {
            SegTable = segTablePtr[proc.pid];
            removeEntry(SegTable, segCount);
            segTablePtr[proc.pid] = NULL;
            printf("Process %d’s segment table has been deleted\n", proc.pid);
        }
        proc = get_input(); // Read in seg table
    }
    print_all(MemHoles);
    return 0;
}