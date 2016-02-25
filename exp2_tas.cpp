int counter;
#include "exp2.hpp"
volatile unsigned long lock = 0;
inline void acquire_lock(volatile void* local) {
	while(tas(&lock))
		;
}
inline void release_lock(volatile void* local) {
	lock = 0;
}
inline void inc() {
	counter++;
}
inline void init_lock() {
}
inline void destroy_lock() {
}
inline volatile void* getlocal() {
	return NULL;
}
