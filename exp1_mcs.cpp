int counter;
#include "exp1.hpp"
mcs_qnode_t **lock;
inline void acquire_lock(volatile void *local) {
	mcs_acquire(lock, (mcs_qnode_t*)local);
}
inline void release_lock(volatile void *local) {
	mcs_release(lock, (mcs_qnode_t*)local);
}
inline void inc() {
	counter++;
}
inline void init_lock() {
	lock = new mcs_qnode_t*[1];
	lock[0] = 0;
}
inline void destroy_lock() {
}
inline volatile void* getlocal() {
	return new mcs_qnode_t();
}
