//Compile+Run: gcc assignment4.c LinkedList.c ASSN4_grader.o -o assignment4.out && ./assignment4.out

#include <stdlib.h>
#include <stdio.h>
#include "LinkedList.h"
#include "ASSN4_declarations.h"


#define MEM_SIZE 200
#define FALSE 0
#define TRUE 1


NODE *MemHoles;

//Setup memory holes LinkedList
void initMem()
{
    Segment newSeg;
    newSeg.base = 0;
    newSeg.sizee = MEM_SIZE;

    MemHoles = create_node(newSeg);
}

//Safely allocates memory, aborts program on failure
void *safe_malloc(size_t n)
{
    //Credits: Jonathon Reinhart, https://stackoverflow.com/a/35027099
    void *p = malloc(n);
    if (p == NULL) {
        fprintf(stderr, "Fatal: failed to allocate %zu bytes.\n", n);
        abort();
    }
    return p;
}

//Combines free memory holes after a process has been removed/deleted from memory
void combineHoles()
{
    int combined = FALSE;
    for (NODE *current = MemHoles; current->next != NULL; current = current->next) {
        int baseAddr = current->data.base;
        int procSize = current->data.sizee;

        if((baseAddr+procSize) == current->next->data.base) //If the current memory hole ends where the next memory hole begins
        {
            Segment data;
            data.base = current->next->data.base;
            data.sizee = current->next->data.sizee;
            MemHoles = delete_node(MemHoles, data); //Delete next memory hole

            data.base = baseAddr; //Current memory hole base address
            data.sizee = procSize+data.sizee; //Currrent memory hole size + next memory hole size
            set_node_val(current, data); //Update node to reflect changes
            combined = TRUE;
        }
    }
    if(combined) combineHoles(); //Keep combining until can't
}


//Prints segement table of a process
void PrintSegTable(int** tbl, int rows)
{
    for(int i = 0; i < rows; i++)
    {
        //(Base, Size)
        printf("(%d, %d) ", tbl[i][0], tbl[i][1]);
    }
    printf("\n");
}

//Allocates memory for a 2D array to be used for a process segment table
int **MakeSegTable(int rows, int cols)
{
    //Creating/Using 2D array: Bart van Ingen Schenau, https://stackoverflow.com/a/3912959
    int **tmp;
    tmp = safe_malloc(rows * sizeof(int *));
    for (int i = 0; i < rows; i++)
    {
        tmp[i] = safe_malloc(cols * sizeof(int));
    }
    return tmp;
}

//Counts how many memory segments a process has
int countSegments(Process proc)
{
    int segCount = 0;
    while((*proc.segments)[segCount][0] != 0 && (*proc.segments)[segCount][1])
    {
        segCount++;
    }
    return segCount;
}

//Adds a new/in process into memory and updates the free memory holes
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
            while(currentHole != NULL) //Loop through memory holes
            {
                int baseAddr = currentHole->data.base; //Get base address of memory hole
                int holeSize = currentHole->data.sizee;
                int procSize = (*proc.segments)[i][1]; //Size of process requesting memory
                if (holeSize >= procSize) //Find the first hole with enough space for process
                {
                    Segment newSeg;
                    int newBase = baseAddr + procSize; //Get base address for updated memory hole
                    int newSize = holeSize - procSize; //Shrink memory hole size

                    newSeg.base = newBase;
                    newSeg.sizee = newSize;

                    tbl[i][0] = baseAddr; //Update process seg table
                    tbl[i][1] =  procSize;
                    if(newSize != 0) 
                    {
                        set_node_val(currentHole, newSeg); //Update memory hole linked list
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

//Remove process from memory
void removeEntry(int ** tbl, int count)
{
    for(int i = 0; i < count; i++)
    {
        Segment data;
        data.base = tbl[i][0]; //Locate process segment base address
        data.sizee = tbl[i][1]; //Find size

        MemHoles = append_node(MemHoles, create_node(data)); //Add to free memory
        tbl[i][0] = -1; //Mark empty
    }
    combineHoles();
}

int main(int argc, char **argv)
{
    initMem();
    int *segTablePtr[20]; //Keep  track of each process' seg table
    Process proc = get_input(); //status(0: New, 1: In, 2: Out, 3: Terminated)
    while (proc.pid != -1)
    {
        int segCount = countSegments(proc);
        int **SegTable = MakeSegTable(segCount, 2); //Malloc a seg table to the size of counter
        if (proc.status == 0 || proc.status == 1) //New proc
        {
            printf("%3s, %7s %20s\n", "pid", "status", "---------------segments---------------" );
            printf("%3d, %7d, ", proc.pid, proc.status);

            addEntry(proc, SegTable, segCount);
            segTablePtr[proc.pid] = SegTable;
            PrintSegTable(SegTable, segCount);
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