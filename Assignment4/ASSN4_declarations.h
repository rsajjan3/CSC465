#define size 5

typedef struct { 
    int pid;                // process name
    int status;             // process status
    int (*segments)[size][2];  // segments
} Process;
// e.g. the segments for p1 are given by: int p1[4][2] = {{1,5},{2,10},{3,3},{4,2}};

// print the segments
void printer(int (*p)[][2]);
// get the next process
Process get_input();
// send a pointer to the holes list
void done();