#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "atomic_ops.h"
using namespace std;

mcs_qnode_t **lock;

int counter = 0;
void *inc(void *_i) {
	int i = *((int*) _i);
	mcs_qnode_t local;
	for (int j = 0; j < i; j++) {
		mcs_acquire(lock, &local);
		counter++;
		//cout << counter << endl;
		mcs_release(lock, &local);
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	lock = new mcs_qnode_t*[1];
	lock[0] = 0;
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
