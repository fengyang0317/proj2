int counter;
#include "exp1.hpp"
tatas_lock_t lock = 0;
inline void acquire_lock(volatile void* local) {
	tatas_acquire(&lock);
}
inline void release_lock(volatile void* local) {
	tatas_release(&lock);
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
