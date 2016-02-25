int counter;
#include "exp2.hpp"
ticket_lock_t lock;
inline void acquire_lock(volatile void* local) {
	ticket_acquire(&lock);
}
inline void release_lock(volatile void* local) {
	ticket_release(&lock);
}
inline void inc() {
	counter++;
}
inline void init_lock() {
	lock.next_ticket = 0;
	lock.now_serving = 0;
}
inline void destroy_lock() {
}
inline volatile void* getlocal() {
	return NULL;
}
