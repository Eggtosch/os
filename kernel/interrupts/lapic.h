#pragma once

#include <common.h>

/*
 * check if the relevant features for the lapic and tsc timer exist.
 * panic if any features are not available.
 */
void apic_check_features(void);

/*
 * to be called when a cpu other than cpu 0 starts.
 * id: lapic id.
 */
void apic_init(u8 id);

/*
 * register a new local apic.
 * id: The local apic id as found in the acpi tables.
 */
void apic_register(u8 id);

/*
 * get the cpu that should handle general ioapic interrupts.
 * return: the local apic id.
 */
u8 apic_ioirq_id(void);

/*
 * must be called at the end of the interrupt handler.
 */
void apic_signal_eoi(void);

/*
 * get the current tsc value from the calling cpu.
 * return: tsc value.
 */
u64 apic_current_tsc(void);

/*
 * set the next timer interrupt deadline.
 * tsc: value of next tsc deadline.
 */
void apic_set_next_int(u64 tsc);

/*
 * get the cpu this function is executed on.
 * return: the cpu id
 */
u8 apic_current_cpu(void);
