#include <common.h>
#include <syscall/efi.h>

#define COLD_REBOOT 0
#define WARM_REBOOT 1
#define SHUTDOWN 2

#define EFI_RUNTIME_SERVICES_STRUCT_OFFSET 88
#define EFI_RESET_FUNCTION_POINTER_OFFSET 104

static u64(__attribute__((ms_abi)) * efi_reset)(int type, u64 _unused1, u64 _unused2,
                                                u16 *_unused3) = NULL;

void efi_functions_init(struct boot_info *boot_info) {
	if (boot_info->efi_ptr == NULL) {
		return;
	}

	void *efi_rt_services = *(void **) (boot_info->efi_ptr + EFI_RUNTIME_SERVICES_STRUCT_OFFSET);
	efi_reset             = *(void **) (efi_rt_services + EFI_RESET_FUNCTION_POINTER_OFFSET);
}

bool efi_supported(void) {
	return efi_reset != NULL;
}

void efi_cold_reboot(void) {
	if (efi_reset != NULL) {
		efi_reset(COLD_REBOOT, 0, 0, NULL);
	}
}

void efi_warm_reboot(void) {
	if (efi_reset != NULL) {
		efi_reset(WARM_REBOOT, 0, 0, NULL);
	}
}

void efi_shutdown(void) {
	if (efi_reset != NULL) {
		efi_reset(SHUTDOWN, 0, 0, NULL);
	}
}
