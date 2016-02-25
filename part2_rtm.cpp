#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include "rtm.h"
#include <string.h>
using namespace std;
#define N 1000

int counter[N];
void *inc(void *_i) {
	int i = *((int*) _i);
	int loc, status;
	for (int j = 0; j < i; j++) {
		loc = rand() % N;
		while((status = _xbegin()) != _XBEGIN_STARTED)
			;
		counter[loc]++;
		//cout << loc << ' ' << counter[loc] << endl;
		_xend();
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
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
