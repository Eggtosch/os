#pragma once

#include <common.h>

struct acpi_rsdp {
	char signature[8];
	u8 checksum;
	char oem_id[6];
	u8 revision;
	u32 rsdt_addr;

	u32 len;
	u64 xsdt_addr;
	u8 ext_checksum;
	u8 reserved[3];
} __attribute__((packed));

struct acpi_sdt_header {
	char signature[4];
	u32 len;
	u8 revision;
	u8 checksum;
	char oem_id[6];
	char oem_table_id[8];
	u32 oem_revision;
	u32 creator_id;
	u32 creator_revision;
} __attribute__((packed));

struct acpi_rsdt {
	struct acpi_sdt_header header;
	u32 sdts[];
} __attribute__((packed));

struct acpi_generic_addr {
	u8 addr_space;
	u8 bitwidth;
	u8 bitoffset;
	u8 access_size;
	u64 addr;
} __attribute__((packed));

enum acpi_addr_space {
	ADDR_SPACE_SYSTEM_MEMORY   = 0,
	ADDR_SPACE_SYSTEM_IO       = 1,
	ADDR_SPACE_PCI_CONFIG      = 2,
	ADDR_SPACE_EMBEDDED_CONT   = 3,
	ADDR_SPACE_SYSTEM_MGMT_BUS = 4,
	ADDR_SPACE_SYSTEM_CMOS     = 5,
	ADDR_SPACE_PCI_BAR_TARGET  = 6,
	ADDR_SPACE_IPMI            = 7, /* Intelligent Platform Management Infrastructure */
	ADDR_SPACE_GPIO            = 8,
	ADDR_SPACE_SERIAL_BUS      = 9,
	ADDR_SPACE_COMM_CHANNEL    = 10,
	ADDR_SPACE_RESERVED_BEGIN  = 11,
	ADDR_SPACE_RESERVED_END    = 127,
	ADDR_SPACE_OEM_BEGIN       = 128,
	ADDR_SPACE_OEM_END         = 255,
};

enum acpi_access_size {
	ACCESS_SIZE_UNDEFINED = 0,
	ACCESS_SIZE_8         = 1,
	ACCESS_SIZE_16        = 2,
	ACCESS_SIZE_32        = 3,
	ACCESS_SIZE_64        = 4,
};

struct acpi_fadt {
	struct acpi_sdt_header header;
	u32 firmware_ctrl;
	u32 dsdt;

	u8 reserved;

	u8 preferred_power_management_profile;
	u16 sci_interrupt;
	u32 smi_command_port;
	u8 acpi_enable;
	u8 acpi_disable;
	u8 s4bios_req;
	u8 pstate_control;
	u32 pm1a_event_block;
	u32 pm1b_event_block;
	u32 pm1a_control_block;
	u32 pm1b_control_block;
	u32 pm2_control_block;
	u32 pm_timer_block;
	u32 gpe0_block;
	u32 gpe1_block;
	u8 pm1_event_len;
	u8 pm1_control_len;
	u8 pm2_control_len;
	u8 pm_timer_len;
	u8 gpe0_len;
	u8 gpe1_len;
	u8 gpe1_base;
	u8 cstate_control;
	u16 worst_c2_latency;
	u16 worst_c3_latency;
	u16 flush_size;
	u16 flush_stride;
	u8 duty_offset;
	u8 duty_width;
	u8 day_alarm;
	u8 month_alarm;
	u8 century;

	u16 boot_arch_flags;
	u8 reserved2;
	u32 flags;

	struct acpi_generic_addr reset_reg;

	u8 reset_value;
	u8 reserved3[3];

	u64 x_firmware_control;
	u64 x_dsdt;

	struct acpi_generic_addr x_pm1a_event_block;
	struct acpi_generic_addr x_pm1b_event_block;
	struct acpi_generic_addr x_pm1a_control_block;
	struct acpi_generic_addr x_pm1b_control_block;
	struct acpi_generic_addr x_pm2_control_block;
	struct acpi_generic_addr x_pm_timer_block;
	struct acpi_generic_addr x_gpe0_block;
	struct acpi_generic_addr x_gpe1_block;
} __attribute__((packed));

struct acpi_hpet {
	struct acpi_sdt_header header;
	u32 capabilities;
	struct acpi_generic_addr base_address;
	u8 hpet_id;
	u16 minimum_tick;
	u8 page_protection;
} __attribute__((packed));

struct acpi_madt {
	struct acpi_sdt_header header;
	u32 lapic_addr;
	u32 flags;
} __attribute__((packed));

enum acpi_madt_entry_type {
	ACPI_MADT_TYPE_LAPIC            = 0,
	ACPI_MADT_TYPE_IOAPIC           = 1,
	ACPI_MADT_TYPE_INT_SRC          = 2,
	ACPI_MADT_TYPE_NMI_INT_SRC      = 3,
	ACPI_MADT_LAPIC_NMI             = 4,
	ACPI_MADT_TYPE_LAPIC_OVERRIDE   = 5,
	ACPI_MADT_TYPE_IOSAPIC          = 6,
	ACPI_MADT_TYPE_LSAPIC           = 7,
	ACPI_MADT_TYPE_PLATFORM_INT_SRC = 8,
	ACPI_MADT_TYPE_LX2APIC          = 9,
};

struct acpi_madt_entry {
	u8 type;
	u8 len;
} __attribute__((packed));

struct acpi_madt_lapic {
	struct acpi_madt_entry header;
	u8 processor_uid;
	u8 apic_id;
	u32 flags;
} __attribute__((packed));

struct acpi_madt_ioapic {
	struct acpi_madt_entry header;
	u8 ioapic_id;
	u8 reserved;
	u32 addr;
	u32 g_int_base;
} __attribute__((packed));

struct acpi_madt_int_src {
	struct acpi_madt_entry header;
	u8 bus;
	u8 source;
	u32 g_sys_int;
	u16 flags;
} __attribute__((packed));

struct acpi_madt_nmi_int_src {
	struct acpi_madt_entry header;
	u16 flags;
	u32 g_sys_int;
} __attribute__((packed));

struct acpi_madt_apic_nmi {
	struct acpi_madt_entry header;
	u8 processor_uid;
	u16 flags;
	u8 lapic_int;
} __attribute__((packed));
