#ifndef MEMORY_H
#define MEMORY_H


#define SHM_SIZE          4096 
#define MAX_N             20 
#define KEY_PATH          "."
#define KEY_ID            1707

#define PRODUCER_SLEEP_US 100000
#define CHECK_SLEEP_S     1
#define CONSUMER_SLEEP_US 50000 

typedef struct {
    int first;
    int last;
    int free_off;
    int done;
} ShmHeader;

typedef struct {
    int count;
    int next;

} BlockHeader;

typedef struct {
    void *shm;
    int   shmid;
    int   semid;
} Mem;

int mem_create(Mem *m);
int mem_open(Mem *m);
void mem_detach(Mem *m);
void mem_destroy(Mem *m);

int blockmem(Mem *m);
int unblockmem(Mem *m);

int writearray(Mem *m, const int *arr, int n);

int readarray(Mem *m, int *buf, int *n, int *done);

#endif