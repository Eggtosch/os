#pragma once

#include <common.h>

void apic_register(u8 id);
u8 apic_id(void);
void apic_signal_eoi(void);
