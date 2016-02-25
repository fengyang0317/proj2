#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "atomic_ops.h"
#include <time.h>
#define MAX_THREADS 1000
using namespace std;

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

int *times;
struct timespec start, end;
extern void acquire_lock(volatile void* local);
extern void release_lock(volatile void* local);
extern void init_lock();
extern volatile void* getlocal();
extern void destroy_lock();
extern void inc();
void *run(void *_t) {
	int tid = *((int*) _t);
	int lc = 0;
	volatile void *local = getlocal();
	barrier(tid);
	if (tid == 0) {
		counter = 0;
		clock_gettime(CLOCK_REALTIME, &start);
	}
	barrier(tid);
	for (int j = 0; j < iters; j++) {
		acquire_lock(local);
		if (counter >= iters) {
			release_lock(local);
			break;
		}
		inc();
		lc++;
		release_lock(local);
	}
	times[tid] = lc;
	barrier(tid);
	if (tid == 0) {
		clock_gettime(CLOCK_REALTIME, &end);
		double ti = (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;
		cout << counter << ' ' << ti << endl;
		int sum = 0;
		for (int j = 0; j < nthreads; j++) {
			cout << times[j] << endl;
			sum += times[j];
		}
		cout << sum << endl;
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	init_lock();
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
	times = new int[nthreads];
	int param[nthreads];
	for (int j = 0; j < nthreads; j++) {
		param[j] = j;
		pthread_create(threads + j, NULL, run, param + j);
	}
	for (int j = 0; j < nthreads; j++) {
		pthread_join(threads[j], &status);
	}
	destroy_lock();
	return 0;
}
