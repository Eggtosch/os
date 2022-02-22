FS_BOOT_DIR    := fs/boot
FS_MODULES_DIR := fs/modules
BOOT_FILES     := bin/kernel.elf					\
					kernel/boot/limine.cfg			\
					limine/limine.sys				\
					limine/limine-hdd.bin			\
					limine/limine-eltorito-efi.bin
KERNEL_MODULES := bin/osl.bin

BOOT_RM_FILES    := $(patsubst %, $(FS_BOOT_DIR)/%, $(notdir $(BOOT_FILES)))
MODULES_RM_FILES := $(wildcard $(FS_MODULES_DIR)/*)

ISO := bin/os.iso

LOG_XORRISO := log/xorriso.log
LOG_LIMINE  := log/limine-install.log
LOG_DD      := log/dd.log


.PHONY: build b
build b:
	@mkdir -p bin
	@./scripts/gen_font_bitmap.rb fonts/ kernel/console/font.gen.h 1> /dev/null
	@make -s -C lib all
	@make -s -C kernel all
	@make -s -C osl all
	@cp -u $(BOOT_FILES) $(FS_BOOT_DIR)
	@cp -u $(KERNEL_MODULES) $(FS_MODULES_DIR)
	@xorriso -as mkisofs				\
			-b boot/limine-hdd.bin	\
			-no-emul-boot -boot-load-size 4 -boot-info-table \
			--efi-boot boot/limine-eltorito-efi.bin \
			-efi-boot-part --efi-boot-image --protective-msdos-label \
			fs/ -o $(ISO) 2> $(LOG_XORRISO) || (cat $(LOG_XORRISO) && false)
	@limine-install $(ISO) 2> $(LOG_LIMINE)  || (cat $(LOG_LIMINE)  && false)

.PHONY: run r
run r:
	@qemu-system-x86_64 -d int -D qemu_log.txt -serial stdio -s -drive format=raw,file=$(ISO)

.PHONY: install i
install i:
	@test $(dev) || (echo Need argument 'dev=<device_path>' to target 'install' && false)
	@./scripts/install.sh $(shell pwd) $(ISO) $(dev) $(LOG_DD)

.PHONY: clean c
clean c:
	@make -s -C kernel clean
	@make -s -C lib clean
	@make -s -C osl clean
	@rm -f $(ISO)
	@rm -f $(BOOT_RM_FILES)
	@rm -f $(MODULES_RM_FILES)


ALL_C_H_RB_FILES := $(shell find . -type f -name "*.c" -o -name "*.h" -o -name "*.rb")
IGNORE           := $(shell find . -type f -path "./cross-compiler/*")
FILTERED_FILES   := $(filter-out $(IGNORE), $(ALL_C_H_RB_FILES))

.PHONY: lines
lines:
	@cat $(FILTERED_FILES) | wc -l
