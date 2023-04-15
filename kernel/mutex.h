#pragma once

#include <common.h>

typedef _Atomic bool mutex_t;

void mutex_init(mutex_t *m);
void mutex_lock(mutex_t *m);
bool mutex_trylock(mutex_t *m);
void mutex_unlock(mutex_t *m);
