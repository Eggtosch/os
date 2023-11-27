#include <mutex.h>

void mutex_init(mutex_t *m) {
	__atomic_clear(m, __ATOMIC_RELEASE);
}

void mutex_lock(mutex_t *m) {
	while (1) {
		if (mutex_trylock(m)) {
			break;
		}
		while (__atomic_load_n((bool *) m, __ATOMIC_RELAXED)) {
			asm volatile("pause" ::: "memory");
		}
	}
}

bool mutex_trylock(mutex_t *m) {
	return __atomic_test_and_set(m, __ATOMIC_ACQUIRE) == 0;
}

void mutex_unlock(mutex_t *m) {
	__atomic_clear(m, __ATOMIC_RELEASE);
}
