#pragma once

#include <common.h>

typedef _Atomic bool mutex_t;

/*
 * create a new mutex.
 * mutex: uninizialized memory location to use for mutex.
 */
void mutex_init(mutex_t *mutex);

/*
 * busyloop until mutex lock can be acquired.
 * mutex: mutex initialized with mutex_init.
 */
void mutex_lock(mutex_t *mutex);

/*
 * try to acquire a mutex lock and return if it failed.
 * mutex: mutex initialized with mutex_init.
 * return: if the mutex lock could be acquired.
 */
bool mutex_trylock(mutex_t *mutex);

/*
 * unlock a held mutex, no op if mutex is not held by caller.
 * mutex: mutex initialized with mutex_init.
 */
void mutex_unlock(mutex_t *mutex);
