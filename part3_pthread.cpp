#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
using namespace std;
int t = 4;
pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
void barrier (int expect)
{
	static int arrived = 0;

	pthread_mutex_lock (&mut);	//lock

	arrived++;
	if (arrived < expect)
		pthread_cond_wait (&cond, &mut);
	else {
		arrived = 0;		// reset the barrier before broadcast is important
		pthread_cond_broadcast (&cond);
	}

	pthread_mutex_unlock (&mut);	//unlock
}
void *inc(void *_i) {
	int i = *((int*) _i);
	for (int j = 0; j < i; j++) {
		barrier(t);
	}
	pthread_exit(NULL);
}

int main(int argc, char *argv[]) {
	int i = 10000;
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
