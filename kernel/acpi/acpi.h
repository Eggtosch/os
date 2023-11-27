#pragma once

#include <boot/boot_info.h>

void acpi_init(struct boot_info *boot_info);
u16 acpi_get_century_register(void);
