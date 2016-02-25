#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "atomic_ops.h"
#define MAX_THREADS 1000
using namespace std;
int i= 10000, nthreads = 4;
int depth = 31;

struct node {
	node() {
		locksense = 0;
		parent = NULL;
		k = 1;
		count = 1;
	}
	int k;
	volatile unsigned long count;
	bool locksense;
	node* parent;
};

node **array;

void combining_barrier_aux(node * no, bool& sense) {
	if (fad(&no->count) == 1) {
		if (no->parent) {
			combining_barrier_aux(no->parent, sense);
		}
		no->count = no->k;
		no->locksense = !no->locksense;
	}
	else {
		while (no->locksense != sense)
			;
	}
}

void combining_barrier(int t, bool& sense) {
	combining_barrier_aux(array[t], sense);
	sense = !sense;
}

struct timespec start, end;
void *inc(void *_t) {
	int t = *((int*) _t);
	bool sense = 1;
    combining_barrier(t, sense);
    if (t == 1) {
        clock_gettime(CLOCK_REALTIME, &start);
    }
    combining_barrier(t, sense);
	for (int j = 0; j < i; j++) {
		combining_barrier(t, sense);
        //cout << t << ' ' << j << endl;
	}
    combining_barrier(t, sense);
    if (t == 1) {
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

	array = new node*[nthreads + 1];
	for (int j = 1; j <= nthreads; j++) {
		array[j] = new node();
		if (j != 1) {
			array[j]->parent = array[j / 2];
			array[j]->parent->k++;
			array[j]->parent->count++;
		}
	}

	pthread_t threads[nthreads];
	int _t[nthreads];
	for (int j = 0; j < nthreads; j++) {
		_t[j] = j + 1;
		pthread_create(threads + j, NULL, inc, _t + j);
	}
	for (int j = 0; j < nthreads; j++) {
		pthread_join(threads[j], &status);
	}
	return 0;
}
