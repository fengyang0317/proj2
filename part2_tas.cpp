#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "atomic_ops.h"
#include <string.h>
using namespace std;
#define N 1000
typedef volatile unsigned long tas_lock_t;

tas_lock_t lock[N];
inline void acquire_lock(tas_lock_t* L) {
	while(tas(L))
		;
}
inline void release_lock(tas_lock_t* L) {
	*L = 0;
}

int counter[N];
void *inc(void *_i) {
	int i = *((int*) _i);
	int loc;
	for (int j = 0; j < i; j++) {
		loc = rand() % N;
		acquire_lock(lock + loc);
		counter[loc]++;
		//cout << loc << ' ' << counter[loc] << endl;
		release_lock(lock + loc);
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	memset(counter, 0, sizeof(counter));
	int i = 10000, t = 4;
	void *status;
	char rc;
	while ((rc = getopt (argc, argv, "hi:t:")) != -1)
		switch (rc) 
		{
			case 'i':
				i = atoi (optarg);
				break;
			case 't':
				t = atoi (optarg);
				break;
			case 'h':
				printf("Usage: ./exp1 [-t <num threads>] [-i <num>]\n");
				return 0;
		}

	printf("%d threads, i = %d\n", t, i);

	pthread_t threads[t];
	for (int j = 0; j < t; j++) {
		pthread_create(threads + j, NULL, inc, &i);
	}
	for (int j = 0; j < t; j++) {
		pthread_join(threads[j], &status);
	}
	return 0;
}
