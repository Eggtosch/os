#include <acpi/acpi.h>
#include <acpi/acpi_defs.h>
#include <time/hpet.h>
#include <interrupts/ioapic.h>
#include <interrupts/lapic.h>
#include <io/stdio.h>
#include <string.h>
#include <panic.h>
#include <memory/pmm.h>

static struct acpi_fadt *acpi_fadt;

u16 acpi_get_century_register(void) {
	return acpi_fadt->century;
}

static bool rsdp_is_v2(struct acpi_rsdp *rsdp) {
	return rsdp->revision != 0;
}

static bool checksum_valid(u8 *bytes, u32 count) {
	u32 checksum = 0;
	for (u32 i = 0; i < count; i++) {
		checksum += bytes[i];
	}
	return (checksum & 0xff) == 0;
}

static bool rsdp_checksum_valid(struct acpi_rsdp *rsdp) {
	u32 length = rsdp_is_v2(rsdp) ? sizeof(struct acpi_rsdp) : offsetof(struct acpi_rsdp, len);
	return checksum_valid((u8*) rsdp, length);
}

static struct acpi_sdt_header *rsdt_find(struct acpi_rsdt *rsdt, const char *signature) {
	if (strlen(signature) != 4) {
		return NULL;
	}

	struct acpi_sdt_header *header = NULL;

	int entries = (rsdt->header.len - sizeof(rsdt->header)) / 4;
	for (int i = 0; i < entries; i++) {
		struct acpi_sdt_header *h = (struct acpi_sdt_header*) pmm_to_virt(rsdt->sdts[i]);
		if (memcmp((void*) h->signature, (void*) signature, 4) == 0) {
			header = h;
			break;
		}
	}

	if (header == NULL) {
		return NULL;
	}

	if (!checksum_valid((u8*) header, header->len)) {
		panic("Checksum of ACPI entry %.4s invalid!\n", signature);
	}

	return header;
}

__unused static void print_sdt_entries(struct acpi_rsdt *rsdt) {
	int entries = (rsdt->header.len - sizeof(rsdt->header)) / 4;
	for (int i = 0; i < entries; i++) {
		struct acpi_sdt_header *h = (struct acpi_sdt_header*) (u64) rsdt->sdts[i];
		kprintf("%.4s\n", h->signature);
	}
}

static void parse_fadt(struct acpi_rsdt *rsdt);
static void parse_hpet(struct acpi_rsdt *rsdt);
static void parse_madt(struct acpi_rsdt *rsdt);

void acpi_init(struct boot_info *boot_info) {
	struct acpi_rsdp *rsdp = (struct acpi_rsdp*) boot_info->rsdp;

	if (memcmp(rsdp->signature, "RSD PTR ", 8) != 0) {
		panic("ACPI signature invalid\n");
	}
	if (!rsdp_checksum_valid(rsdp)) {
		panic("ACPI checksum invalid\n");
	}

	struct acpi_rsdt *rsdt = (struct acpi_rsdt*) pmm_to_virt(rsdp->rsdt_addr);

	parse_fadt(rsdt);
	parse_hpet(rsdt);
	parse_madt(rsdt);
}

static void parse_fadt(struct acpi_rsdt *rsdt) {
	acpi_fadt = (struct acpi_fadt*) (u64) rsdt_find(rsdt, "FACP");
	if (acpi_fadt == NULL) {
		panic("ACPI fadt table not found!\n");
	}

	kprintf("found acpi table: FACP\n");
}

static void parse_hpet(struct acpi_rsdt *rsdt) {
	struct acpi_hpet *hpet = (struct acpi_hpet*) rsdt_find(rsdt, "HPET");
	if (hpet == NULL) {
		panic("ACPI hpet table not found!\n");
	}

	kprintf("found acpi table: HPET\n");
	hpet_register(hpet->base_address.addr);
}

static void parse_madt(struct acpi_rsdt *rsdt) {
	struct acpi_madt *madt = (struct acpi_madt*) rsdt_find(rsdt, "APIC");
	if (madt == NULL) {
		panic("ACPI madt table not found!\n");
	}

	kprintf("found acpi table: APIC\n");

	struct acpi_madt_entry *entry = (void*) madt + sizeof(struct acpi_madt);
	while ((u64) entry < (u64) madt + madt->header.len) {
		if (entry->type == ACPI_MADT_TYPE_LAPIC) {
			struct acpi_madt_lapic *lapic = (void*) entry;
			apic_register(lapic->apic_id);
			kprintf("local apic: %d/%d/%#x\n", lapic->processor_uid, lapic->apic_id, lapic->flags);
		} else if (entry->type == ACPI_MADT_TYPE_IOAPIC) {
			struct acpi_madt_ioapic *ioapic = (void*) entry;
			kprintf("found io apic: id=%d\n", ioapic->ioapic_id);
			ioapic_register(ioapic->ioapic_id, ioapic->addr, ioapic->g_int_base);
		}

		entry = (void*) entry + entry->len;
	}

	entry = (void*) madt + sizeof(struct acpi_madt);
	while ((u64) entry < (u64) madt + madt->header.len) {
		if (entry->type == ACPI_MADT_TYPE_INT_SRC) {
			struct acpi_madt_int_src *override = (void*) entry;
			if (override->source != override->g_sys_int) {
				kprintf("remapping irq %d to irq %d\n", override->source, override->g_sys_int);
			}
			ioapic_set_irq_override(override->source, override->g_sys_int, override->flags);
		}

		entry = (void*) entry + entry->len;
	}
}
