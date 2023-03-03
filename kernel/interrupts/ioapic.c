#include <memory/pmm.h>
#include <panic.h>
#include <interrupts/interrupts.h>
#include <io/io.h>
#include <interrupts/lapic.h>
#include <interrupts/ioapic.h>

#define MAX_IOAPICS 4
#define IOREGSEL 0x00
#define IOREGWIN 0x10

#define IOAPIC_VERSION 1

#define IOAPIC_RENTRY_BASE 0x10

#define IOAPIC_ACTIVE_HIGH 0
#define IOAPIC_ACTIVE_LOW  1

#define IOAPIC_EDGE  0
#define IOAPIC_LEVEL 1

struct ioapic {
	void *addr;
	u32 gsi_base;
	u8 max_rentry;
	u8 id;
};

union ioapic_rentry {
	u64 raw;
	struct {
		u64 vector : 8;
		u64 deliv_mode : 3;
		u64 dest_mode : 1;
		u64 deliv_status : 1;
		u64 polarity : 1;
		u64 remote_irr : 1;
		u64 trigger_mode : 1;
		u64 mask : 1;
		u64 : 39;
		u64 dest : 8;
	};
} __attribute__((packed));

static struct ioapic ioapics[MAX_IOAPICS];
static int nioapics = 0;

static u8 redirections[256];

static u32 ioapic_read32(void *base_addr, u8 reg) {
	volatile u32 *regsel = (volatile u32*) (base_addr + IOREGSEL);
	volatile u32 *regwin = (volatile u32*) (base_addr + IOREGWIN);
	*regsel = reg;
	return *regwin;
}

static u64 ioapic_read64(void *base_addr, u8 index) {
	volatile u32 *regsel = (volatile u32*) (base_addr + IOREGSEL);
	volatile u32 *regwin = (volatile u32*) (base_addr + IOREGWIN);
	u64 value;

	*regsel = index;
	value = *regwin;
	*regsel = index + 1;
	value |= (u64) *regwin << 32;

	return value;
}

static void ioapic_write64(void *base_addr, u8 index, u64 value) {
	volatile u32 *regsel = (volatile u32*) (base_addr + IOREGSEL);
	volatile u32 *regwin = (volatile u32*) (base_addr + IOREGWIN);

	*regsel = index;
	*regwin = value & 0xffffffff;
	*regsel = index + 1;
	*regwin = (value >> 32) & 0xffffffff;
}

static u32 rentry_index(u8 rentry) {
	return IOAPIC_RENTRY_BASE + rentry * 2;
}

static struct ioapic *ioapic_get_from_irq(u8 irq) {
	for (int i = 0; i < nioapics; i++) {
		u8 base = ioapics[i].gsi_base;
		u8 entries = ioapics[i].max_rentry;
		if (irq >= base && irq < base + entries) {
			return &ioapics[i];
		}
	}
	return NULL;
}

void ioapic_register(u8 id, u64 addr, u32 gsi_base) {
	if (id >= MAX_IOAPICS || nioapics >= MAX_IOAPICS) {
		kprintf("ioapic: ignoring %d, max supported is %d\n", id, MAX_IOAPICS);
		return;
	}

	struct ioapic *ioapic = &ioapics[nioapics++];

	if (ioapic->addr != NULL) {
		panic("ioapic id %d already registered!", id);
	}

	ioapic->id = id;
	ioapic->addr = pmm_to_virt(addr);

	u32 version = ioapic_read32(ioapic->addr, IOAPIC_VERSION);
	ioapic->max_rentry = (version >> 16) & 0xff;

	ioapic->gsi_base = gsi_base;

	kprintf("ioapic id %d, from %d to %d\n", id, gsi_base, gsi_base + ioapic->max_rentry);

	for (u8 i = ioapic->gsi_base; i < ioapic->gsi_base + ioapic->max_rentry; i++) {
		ioapic_set_irq_override(i, i, 1);
		redirections[i] = i;
	}
}

void ioapic_set_irq_override(u8 src_irq, u8 dst_irq, u16 flags) {
	struct ioapic *ioapic = ioapic_get_from_irq(src_irq);

	if (ioapic == NULL) {
		panic("no ioapic for irq %d", src_irq);
	}

	union ioapic_rentry rentry = { .raw = ioapic_read64(ioapic->addr, rentry_index(dst_irq)) };
	rentry.mask = 1;
	rentry.vector = src_irq + INT_BASE;
	rentry.dest = apic_id();

	u8 polarity = flags & 0x3;
	if (polarity == 0b00 || polarity == 0b11) {
		rentry.polarity = IOAPIC_ACTIVE_LOW;
	} else if (polarity == 0b01) {
		rentry.polarity = IOAPIC_ACTIVE_HIGH;
	} else {
		panic("invalid irq (%d) polarity %d", src_irq, polarity);
	}

	u8 trigger_mode = (flags >> 2) & 0x3;
	if (trigger_mode == 0b00 || trigger_mode == 0b01) {
		rentry.trigger_mode = IOAPIC_EDGE;
	} else if (trigger_mode == 0b11) {
		rentry.trigger_mode = IOAPIC_LEVEL;
	} else {
		panic("invalid irq (%d) trigger mode %d", src_irq, trigger_mode);
	}

	ioapic_write64(ioapic->addr, rentry_index(dst_irq), rentry.raw);
	redirections[src_irq] = dst_irq;
}

void ioapic_set_mask(u8 irq, u8 mask) {
	if (irq < INT_BASE) {
		panic("trying to set ioapic mask for exception %d", irq);
	}

	irq -= INT_BASE;

	struct ioapic *ioapic = ioapic_get_from_irq(irq);
	if (ioapic == NULL) {
		panic("no ioapic for irq %d", irq);
	}

	if (irq != redirections[irq]) {
		irq = redirections[irq];
		ioapic = ioapic_get_from_irq(irq);
	}

	union ioapic_rentry rentry = { .raw = ioapic_read64(ioapic->addr, rentry_index(irq)) };
	rentry.mask = mask & 1;
	ioapic_write64(ioapic->addr, rentry_index(irq), rentry.raw);
}
