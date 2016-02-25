#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "rtm.h"
#include "atomic_ops.h"
#include <string.h>
using namespace std;
#define N 1000
#define MAX_THREADS 1000

int nthreads = 4;
int iters = 10000;

void barrier(int tid)
{
    static volatile unsigned long count = 0;
    static volatile unsigned int sense = 0;
    static volatile unsigned int thread_sense[MAX_THREADS] = {0};

    thread_sense[tid] = !thread_sense[tid];
    if (fai(&count) == nthreads-1) {
        count = 0;
        sense = !sense;
    } else {
        while (sense != thread_sense[tid]);     /* spin */
    }
}

int counter[N];
struct timespec start, end;
void *inc(void *_t) {
    int tid = *((int*) _t);
    int loc, status;
    barrier(tid);
    if (tid == 0) {
        memset(counter, 0, sizeof(counter));
        clock_gettime(CLOCK_REALTIME, &start);
    }
    barrier(tid);
    for (int j = 0; j < iters; j++) {
        loc = rand() % N;
        while((status = _xbegin()) != _XBEGIN_STARTED)
            ;
        counter[loc]++;
        //cout << loc << ' ' << counter[loc] << endl;
        _xend();
    }
    barrier(tid);
    if (tid == 0) {
        clock_gettime(CLOCK_REALTIME, &end);
        double ti = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
        cout << ti << endl;
    }
    pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
    void *status;
    char rc;
    while ((rc = getopt (argc, argv, "hi:t:")) != -1)
        switch (rc)
        {
            case 'i':
                iters = atoi (optarg);
                break;
            case 't':
                nthreads = atoi (optarg);
                break;
            case 'h':
                printf("Usage: ./exp1 [-t <num threads>] [-i <num>]\n");
                return 0;
        }

    printf("%d threads, i = %d\n", nthreads, iters);

    pthread_t threads[nthreads];
    int param[nthreads];
    for (int j = 0; j < nthreads; j++) {
        param[j] = j;
        pthread_create(threads + j, NULL, inc, param + j);
    }
    for (int j = 0; j < nthreads; j++) {
        pthread_join(threads[j], &status);
    }
    return 0;
}
