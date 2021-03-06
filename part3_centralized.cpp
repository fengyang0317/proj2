#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "atomic_ops.h"
#define MAX_THREADS 1000
using namespace std;
const int totalcores = 72;
int i= 10000, nthreads = 4;
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
struct timespec start, end;
void *inc(void *_t) {
	int t = *((int*) _t);
    barrier(t);
    if (t == 0) {
        clock_gettime(CLOCK_REALTIME, &start);
    }
    barrier(t);
	for (int j = 0; j < i; j++) {
		barrier(t);
	}
    if (t == 0) {
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
				i = atoi (optarg);
				break;
			case 't':
				nthreads = atoi (optarg);
				break;
			case 'h':
				printf("Usage: ./exp1 [-t <num threads>] [-i <num>]\n");
				return 0;
		}

	printf("%d threads, i = %d\n", nthreads, i);

	pthread_t threads[nthreads];
	int _t[nthreads];
    cpu_set_t mask;
	for (int j = 0; j < nthreads; j++) {
		_t[j] = j;
		pthread_create(threads + j, NULL, inc, _t + j);
        CPU_ZERO(&mask);
        CPU_SET(j % totalcores, &mask);
        if (pthread_setaffinity_np(threads[j], sizeof(mask), &mask) < 0) {
            printf("pin error\n");
            exit(1);
        }
	}
	for (int j = 0; j < nthreads; j++) {
		pthread_join(threads[j], &status);
	}
	return 0;
}
