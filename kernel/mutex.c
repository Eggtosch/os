#include <mutex.h>

void mutex_init(mutex_t *m) {
	__atomic_clear(m, __ATOMIC_RELEASE);
}

// Busy loop until the mutex can be acquired.
void mutex_lock(mutex_t *m) {
	while (__atomic_test_and_set(m, __ATOMIC_ACQUIRE)) {
		asm volatile("pause" ::: "memory");
	}
}

// Try to lock the mutex, returns true if locking was successfull.
bool mutex_trylock(mutex_t *m) {
	return __atomic_test_and_set(m, __ATOMIC_ACQUIRE) == 0;
}

void mutex_unlock(mutex_t *m) {
	__atomic_clear(m, __ATOMIC_RELEASE);
}
