#pragma once

#include <common.h>

void io_outb(u16 port, u8 value);
u8   io_inb(u16 port);
void io_wait(void);
