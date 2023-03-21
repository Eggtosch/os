#include <io/io.h>
#include <io/stdio.h>
#include <memory/pmm.h>
#include <interrupts/lapic.h>

#define APIC_BASE 0xFEE00000
#define APIC_TPR 0x80
#define APIC_EOI 0xB0
#define APIC_DFR 0xE0
#define APIC_SVR 0xF0

static void *apic_base = NULL;
static u8 lapic_id;

static void apic_write32(u32 reg, u32 value) {
	volatile u32 *addr = apic_base + reg;
	*addr = value;
}

void apic_register(u8 id) {
	if (id != 0)
		return;

	lapic_id = id;

	apic_base = pmm_to_virt(APIC_BASE);

	u64 apic_msr = rdmsr(0x1B);
	wrmsr(0x1B, apic_msr | (1 << 11));

	apic_write32(APIC_TPR, 0);

	apic_write32(APIC_SVR, 0x1ff);

	apic_write32(APIC_DFR, 0xffffffff);
	apic_signal_eoi();
}

u8 apic_id(void) {
	return lapic_id;
}

void apic_signal_eoi(void) {
	apic_write32(APIC_EOI, 0);
}
