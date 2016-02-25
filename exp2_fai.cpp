volatile unsigned long counter;
#include "exp2.hpp"
inline void acquire_lock(volatile void *local) {
}
inline void release_lock(volatile void *local) {
}
inline void inc() {
	fai(&counter);
}
inline void init_lock() {
}
inline void destroy_lock() {
}
inline volatile void* getlocal() {
	return NULL;
}
