#include <acpi/acpi.h>
#include <acpi/acpi_defs.h>
#include <acpi/hpet.h>
#include <io/stdio.h>
#include <io/io.h>
#include <string.h>
#include <panic.h>
#include <driver/util.h>

static struct acpi_fadt *acpi_fadt;

u16 acpi_get_century_register(void) {
	return acpi_fadt->century;
}

bool acpi_reset(void) {
	if (acpi_fadt->header.revision >= 2 && acpi_fadt->flags & (1 << 10)) {
		struct acpi_generic_addr addr = acpi_fadt->reset_reg;
		if (addr.addr_space == ADDR_SPACE_SYSTEM_IO) {
			io_outb(addr.addr, acpi_fadt->reset_value);
		} else if (addr.addr_space == ADDR_SPACE_SYSTEM_MEMORY) {
			*(u8*) addr.addr = acpi_fadt->reset_value;
		} else if (addr.addr_space == ADDR_SPACE_PCI_CONFIG) {
		}
	}

	u8 good = 0x02;
	while (good & 0x02) {
		good = io_inb(0x64);
	}
	io_outb(0x64, 0xFE);

	return false;
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
		struct acpi_sdt_header *h = (struct acpi_sdt_header*) (u64) rsdt->sdts[i];
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
		printf("%.4s\n", h->signature);
	}
}

static void parse_fadt(struct acpi_rsdt *rsdt);
static void parse_hpet(struct acpi_rsdt *rsdt);

void acpi_init(struct boot_info *boot_info) {
	struct acpi_rsdp *rsdp = (struct acpi_rsdp*) boot_info->rsdp;

	if (memcmp(rsdp->signature, "RSD PTR ", 8) != 0) {
		panic("ACPI signature invalid\n");
	}
	if (!rsdp_checksum_valid(rsdp)) {
		panic("ACPI checksum invalid\n");
	}

	struct acpi_rsdt *rsdt = (struct acpi_rsdt*) (u64) rsdp->rsdt_addr;

	parse_fadt(rsdt);
	parse_hpet(rsdt);
}

static void parse_fadt(struct acpi_rsdt *rsdt) {
	acpi_fadt = (struct acpi_fadt*) (u64) rsdt_find(rsdt, "FACP");
	if (acpi_fadt == NULL) {
		panic("ACPI fadt table not found!\n");
	}
}

static void parse_hpet(struct acpi_rsdt *rsdt) {
	struct acpi_hpet *hpet = (struct acpi_hpet*) rsdt_find(rsdt, "HPET");
	if (hpet == NULL) {
		panic("ACPI hpet table not found!\n");
	}
	hpet_register(hpet->base_address.addr);
}

