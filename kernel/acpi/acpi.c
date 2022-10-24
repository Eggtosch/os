#include <acpi/acpi.h>
#include <acpi/acpi_defs.h>
#include <io/stdio.h>
#include <io/io.h>
#include <string.h>
#include <panic.h>
#include <driver/util.h>

static struct fadt *acpi_fadt;

u16 acpi_get_century_register(void) {
	return acpi_fadt->century;
}

static bool rsdp_is_v2(struct rsdp *rsdp) {
	return rsdp->revision != 0;
}

static bool checksum_valid(u8 *bytes, u32 count) {
	u32 checksum = 0;
	for (u32 i = 0; i < count; i++) {
		checksum += bytes[i];
	}
	return (checksum & 0xff) == 0;
}

static bool rsdp_checksum_valid(struct rsdp *rsdp) {
	u32 length = rsdp_is_v2(rsdp) ? sizeof(struct rsdp) : offsetof(struct rsdp, len);
	return checksum_valid((u8*) rsdp, length);
}

static struct sdt_header *rsdt_find(struct rsdt *rsdt, const char *signature) {
	if (strlen(signature) != 4) {
		return NULL;
	}

	struct sdt_header *header = NULL;

	int entries = (rsdt->header.len - sizeof(rsdt->header)) / 4;
	for (int i = 0; i < entries; i++) {
		struct sdt_header *h = (struct sdt_header*) (u64) rsdt->sdts[i];
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

static void parse_fadt(struct rsdt *rsdt);

void acpi_init(struct boot_info *boot_info) {
	struct rsdp *rsdp = (struct rsdp*) boot_info->rsdp;

	if (memcmp(rsdp->signature, "RSD PTR ", 8) != 0) {
		panic("ACPI signature invalid\n");
	}
	if (!rsdp_checksum_valid(rsdp)) {
		panic("ACPI checksum invalid\n");
	}

	struct rsdt *rsdt = (struct rsdt*) (u64) rsdp->rsdt_addr;

	parse_fadt(rsdt);
}

static void parse_fadt(struct rsdt *rsdt) {
	acpi_fadt = (struct fadt*) (u64) rsdt_find(rsdt, "FACP");
	if (acpi_fadt == NULL) {
		panic("ACPI fadt not found!\n");
	}
}

