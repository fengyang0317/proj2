int counter;
#include "exp1.hpp"
pthread_mutex_t mutex;
inline void acquire_lock(volatile void* local) {
	pthread_mutex_lock(&mutex);
}
inline void release_lock(volatile void* local) {
	pthread_mutex_unlock(&mutex);
}
inline void inc() {
	counter++;
}
inline void init_lock() {
	pthread_mutex_init(&mutex, NULL);
}
inline void destroy_lock() {
	pthread_mutex_destroy(&mutex);
}
inline volatile void* getlocal() {
	return NULL;
}
