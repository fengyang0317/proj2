#include <iostream>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <sched.h>
using namespace std;
const int totalcores = 72;
int t = 4, i = 10000;
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
struct timespec start, end;
void *inc(void *_t) {
    int tid = *((int*) _t);
    barrier(t);
    if (tid == 0) {
        clock_gettime(CLOCK_REALTIME, &start);
    }
    barrier(t);
	for (int j = 0; j < i; j++) {
		barrier(t);
	}
    barrier(t);
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
    int param[t];
    cpu_set_t mask;
	for (int j = 0; j < t; j++) {
        param[j] = j;
        pthread_create(threads + j, NULL, inc, param + j);
        CPU_ZERO(&mask);
        CPU_SET(j % totalcores, &mask);
        if (pthread_setaffinity_np(threads[j], sizeof(mask), &mask) < 0) {
            printf("pin error\n");
            exit(1);
        }
	}
	for (int j = 0; j < t; j++) {
		pthread_join(threads[j], &status);
	}
	return 0;
}
