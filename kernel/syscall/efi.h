#pragma once

#include <boot/boot_info.h>

void efi_functions_init(struct boot_info *boot_info);
bool efi_supported(void);
void efi_cold_reboot(void);
void efi_warm_reboot(void);
void efi_shutdown(void);
