#pragma once

#include <common.h>

void scheduler_add_task(void);

/*
 * initialize scheduler inzerrupts.
 */
void scheduler_init(void);

/*
 * start the first scheduler interrupt.
 * has to be called per core.
 */
void scheduler_start(void);
