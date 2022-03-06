FS_DIR         := fs
FS_BOOT_DIR    := fs/boot
FS_MODULES_DIR := fs/modules
BOOT_FILES     := bin/kernel.elf					\
					kernel/boot/limine.cfg			\
					limine/limine.sys				\
					limine/limine-hdd.bin			\
					limine/limine-eltorito-efi.bin
KERNEL_MODULES := bin/osl.bin programs/shell.osl

ISO := bin/os.iso

LOG_DIR     := log
LOG_XORRISO := $(LOG_DIR)/xorriso.log
LOG_LIMINE  := $(LOG_DIR)/limine-install.log
LOG_DD      := $(LOG_DIR)/dd.log


.PHONY: build b
build b:
	@# generate binaries
	@mkdir -p bin
	@make -s -C kernel all
	@make -s -C osl all
	@# generate fs
	@mkdir -p $(FS_BOOT_DIR) $(FS_MODULES_DIR)
	@cp -u $(BOOT_FILES) $(FS_BOOT_DIR)
	@cp -u $(KERNEL_MODULES) $(FS_MODULES_DIR)
	@# create iso
	@mkdir -p $(LOG_DIR)
	@xorriso -as mkisofs				\
			-b boot/limine-hdd.bin	\
			-no-emul-boot -boot-load-size 4 -boot-info-table \
			--efi-boot boot/limine-eltorito-efi.bin \
			-efi-boot-part --efi-boot-image --protective-msdos-label \
			fs/ -o $(ISO) 2> $(LOG_XORRISO) || (cat $(LOG_XORRISO) && false)
	@limine-install $(ISO) 2> $(LOG_LIMINE)  || (cat $(LOG_LIMINE)  && false)

.PHONY: fonts
fonts:
	@./scripts/gen_font_bitmap.rb fonts/ kernel/console/font.gen.h 1> /dev/null

.PHONY: run r
run r:
	@qemu-system-x86_64 -serial stdio -s -drive format=raw,file=$(ISO)

.PHONY: install i
install i:
	@test $(dev) || (echo Need argument 'dev=<device_path>' to target 'install' && false)
	@./scripts/install.sh $(shell pwd) $(ISO) $(dev) $(LOG_DD)

.PHONY: clean c
clean c:
	@make -s -C kernel clean
	@make -s -C osl clean
	@rm -f $(ISO)
	@rm -rf $(FS_DIR)



ALL_C_H_RB_FILES := $(shell find . -type f -name "*.c" -o -name "*.h" -o -name "*.rb")
IGNORE           := $(shell find . -type f -path "./cross-compiler/*")
FILTERED_FILES   := $(filter-out $(IGNORE), $(ALL_C_H_RB_FILES))

.PHONY: lines
lines:
	@cat $(FILTERED_FILES) | wc -l
