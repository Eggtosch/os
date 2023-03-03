#pragma once

#include <common.h>

#define IOAPIC_MAX_IRQS 16

void ioapic_register(u8 id, u64 addr, u32 gsi_base);
void ioapic_set_irq_override(u8 src_irq, u8 dst_irq, u16 flags);
void ioapic_set_mask(u8 irq, u8 mask);
