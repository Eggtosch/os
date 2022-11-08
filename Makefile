FS_DIR         := fs
FS_BOOT_DIR    := fs/boot
FS_MODULES_DIR := fs/modules
BOOT_FILES     := bin/kernel.elf					\
					kernel/boot/limine.cfg			\
					limine/limine.sys				\
					limine/limine-cd.bin			\
					limine/limine-cd-efi.bin
KERNEL_MODULES := bin/osl.bin programs/shell.osl

ISO       := bin/os.iso
QEMU_UEFI := /usr/share/ovmf/OVMF.fd

LOG_DIR     := log
LOG_XORRISO := $(LOG_DIR)/xorriso.log
LOG_LIMINE  := $(LOG_DIR)/limine-install.log
LOG_DD      := $(LOG_DIR)/dd.log

LIMINE_DIR := limine
CROSS_COMPILER := cross-compiler


.PHONY: build
build: | $(LIMINE_DIR) $(CROSS_COMPILER)
	@# create needed directories
	mkdir -p bin $(FS_BOOT_DIR) $(FS_MODULES_DIR) $(LOG_DIR)
	@# make kernel and osl submodules
	make --no-print-directory -C kernel all -j$(shell nproc)
	make --no-print-directory -C osl all -j$(shell nproc)
	@# copy kernel runtime files
	cp -u $(BOOT_FILES) $(FS_BOOT_DIR)
	cp -u $(KERNEL_MODULES) $(FS_MODULES_DIR)
	@# create iso file from directory
	@$(eval ISO_CMD := xorriso -as mkisofs -b boot/limine-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table \
			--efi-boot boot/limine-cd-efi.bin -efi-boot-part --efi-boot-image --protective-msdos-label \
			fs/ -o $(ISO))
	@$(ISO_CMD) 2> $(LOG_XORRISO) || (cat $(LOG_XORRISO) && false)
	@echo $(ISO_CMD)
	@# install limine bootloader to iso
	@./limine/limine-deploy $(ISO) 2> $(LOG_LIMINE)  || (cat $(LOG_LIMINE) && false)
	@echo ./limine/limine-deploy $(ISO)

$(CROSS_COMPILER):
	./scripts/build-cross-compiler.sh

$(LIMINE_DIR):
	git clone --depth 1 --branch v4.20221030.0-binary https://github.com/limine-bootloader/limine.git
	make -C limine
	rm limine/limine-version* limine/limine-deploy.exe
	rm limine/*.c limine/*.h
	rm limine/*.EFI
	rm limine/Makefile limine/LICENSE.md limine/install-sh
	rm limine/.gitignore
	rm -rf limine/.git

.PHONY: fonts
fonts:
	./scripts/gen_font_bitmap.rb fonts/ kernel/console/font.gen.h
	cp -u kernel/console/font.gen.h osl/src/font.gen.h

.PHONY: run
run: build
	qemu-system-x86_64 -enable-kvm -serial stdio -s -drive format=raw,file=$(ISO)

.PHONY: run-uefi
run-uefi: build
	qemu-system-x86_64 -enable-kvm -bios $(QEMU_UEFI) -serial stdio -s -drive format=raw,file=$(ISO)

.PHONY: install
install:
	@test $(dev) || (echo Need argument 'dev=<device_path>' for target \'install\' && false)
	./scripts/install.sh $(shell pwd) $(ISO) $(dev) $(LOG_DD)

.PHONY: clean
clean:
	make --no-print-directory -C kernel clean
	make --no-print-directory -C osl clean
	rm -rf bin
	rm -f $(ISO)
	rm -rf $(FS_DIR)

