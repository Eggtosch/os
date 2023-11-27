#include <cpu/cpu.h>
#include <interrupts/interrupts.h>
#include <interrupts/lapic.h>
#include <io/io.h>
#include <io/stdio.h>
#include <memory/pmm.h>
#include <panic.h>
#include <time/hpet.h>

#define APIC_BASE 0xFEE00000
#define APIC_ID 0x20
#define APIC_EOI 0xB0
#define APIC_SVR 0xF0
#define APIC_TIMER 0x320

struct lapic {
	u64 tsc_rate;
	u8 lapic_id;
};

static void *apic_base = NULL;
static struct lapic lapics[MAX_CPUS];
static u8 n_lapics = 0;

static void apic_write32(u32 reg, u32 value) {
	volatile u32 *addr = apic_base + reg;
	*addr              = value;
}

static u32 apic_read32(u32 reg) {
	volatile u32 *addr = apic_base + reg;
	return *addr;
}

static void setup_tsc_timer(void) {
	u32 setup = apic_read32(APIC_TIMER);
	setup |= INT_SCHED;
	// unmask
	setup &= ~(1 << 16);
	// set to tsc deadline mode
	setup |= (0x2 << 17);
	apic_write32(APIC_TIMER, setup);
}

void apic_check_features(void) {
	if (!cpu_has_flag(CPUID_TSC) || !cpu_has_flag(CPUID_TSCDEADLINE)) {
		panic("tsc not available");
	}
}

static bool valid_id(u8 id) {
	for (int i = 0; i < n_lapics; i++) {
		if (lapics[i].lapic_id == id) {
			return true;
		}
	}

	return false;
}

void apic_init(u8 id) {
	if (!valid_id(id)) {
		return;
	}

	u64 min_cycles = 0xffffffffffffffffUL;
	u32 ms         = 5;
	for (int i = 0; i < 5; i++) {
		u64 t0 = apic_current_tsc();
		hpet_delay_ms(ms);
		u64 t1     = apic_current_tsc();
		u64 cycles = t1 - t0;
		if (cycles < min_cycles) {
			min_cycles = cycles;
		}
	}

	u64 hz              = min_cycles * 1000 / ms;
	lapics[id].tsc_rate = hz;

	u64 mhz          = hz / 1000000;
	u64 mhz_fraction = hz % 1000000;
	kprintf("cpu %d tsc rate: %d.%0.3d MHz\n", apic_current_cpu(), mhz, mhz_fraction / 1000);

	// locally enable apic
	apic_write32(APIC_SVR, 0x1ff);
	setup_tsc_timer();
	apic_signal_eoi();
}

void apic_register(u8 id) {
	struct lapic *lapic = &lapics[n_lapics++];
	lapic->lapic_id     = id;
	lapic->tsc_rate     = 0;

	if (apic_base == NULL) {
		apic_base    = pmm_to_virt(APIC_BASE);
		u64 apic_msr = rdmsr(MSR_APIC_BASE);
		// globally enable apic
		wrmsr(MSR_APIC_BASE, apic_msr | (1 << 11));
	}

	if (id != 0) {
		return;
	}

	apic_init(0);
}

u8 apic_ioirq_id(void) {
	for (int i = 0; i < n_lapics; i++) {
		if (lapics[i].lapic_id == 0) {
			return 0;
		}
	}
	panic("no lapic with id 0");
}

void apic_signal_eoi(void) {
	apic_write32(APIC_EOI, 0);
}

u64 apic_current_tsc(void) {
	u32 hi;
	u32 lo;
	asm("rdtsc" : "=a"(lo), "=d"(hi));
	return ((u64) hi << 32) | lo;
}

void apic_set_next_int(u64 tsc) {
	wrmsr(MSR_TSC_DEADLINE, tsc);
}

u8 apic_current_cpu(void) {
	if (apic_base == NULL) {
		return 0;
	}
	return (apic_read32(APIC_ID) >> 24) & 0xff;
}
