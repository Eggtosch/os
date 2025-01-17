FS_DIR         := fs
FS_BOOT_DIR    := fs/boot
FS_MODULES_DIR := fs/modules
BOOT_FILES     := bin/kernel.elf					\
					kernel/boot/limine.cfg			\
					limine/limine.sys				\
					limine/limine-cd.bin			\
					limine/limine-cd-efi.bin
INIT_PROGRAM   := bin/init

BINDIR    := bin

ISO       := bin/os.iso
QEMU_UEFI := /usr/share/ovmf/x64/OVMF.fd

LOG_DIR     := log
LOG_XORRISO := $(LOG_DIR)/xorriso.log
LOG_LIMINE  := $(LOG_DIR)/limine-install.log
LOG_DD      := $(LOG_DIR)/dd.log

LIMINE_DIR := limine
CROSS_COMPILER := cross-compiler

.PHONY: build
build: | $(LIMINE_DIR) $(CROSS_COMPILER) $(BINDIR) $(LOG_DIR) libc user
	@# create needed directories
	mkdir -p $(FS_BOOT_DIR) $(FS_MODULES_DIR) $(LOG_DIR)
	@# make kernel
	make --no-print-directory -C kernel all -j$(shell nproc)
	@# copy kernel runtime files
	cp -u $(BOOT_FILES) $(FS_BOOT_DIR)
	cp -u $(INIT_PROGRAM) $(FS_MODULES_DIR)
	@# create iso file from directory
	@$(eval ISO_CMD := xorriso -as mkisofs -b boot/limine-cd.bin -no-emul-boot -boot-load-size 4 -boot-info-table \
			--efi-boot boot/limine-cd-efi.bin -efi-boot-part --efi-boot-image --protective-msdos-label \
			fs/ -o $(ISO))
	@$(ISO_CMD) 2> $(LOG_XORRISO) || (cat $(LOG_XORRISO) && false)
	@echo $(ISO_CMD)
	@# install limine bootloader to iso
	@./limine/limine-deploy $(ISO) 2> $(LOG_LIMINE)  || (cat $(LOG_LIMINE) && false)
	@echo ./limine/limine-deploy $(ISO)

$(BINDIR):
	mkdir -p $(BINDIR)

$(LOG_DIR):
	mkdir -p $(LOG_DIR)

$(CROSS_COMPILER):
	./scripts/build-cross-compiler.sh

$(LIMINE_DIR):
	git clone --depth 1 --branch v4.20230317.0-binary https://github.com/limine-bootloader/limine.git
	make -C limine
	rm limine/limine-version* limine/limine-deploy.exe
	rm limine/limine-enroll-config limine/limine-enroll-config.exe
	rm limine/*.c limine/*.h
	rm limine/*.EFI
	rm limine/Makefile limine/LICENSE limine/install-sh
	rm limine/.gitignore
	rm -rf limine/.git

.PHONY: libc
libc: | $(BINDIR)
	$(MAKE) --no-print-directory -C libc -j$(nproc)

.PHONY: user
user: | $(BINDIR)
	$(MAKE) --no-print-directory -C user all -j$(nproc)

QEMU_OPTS := -enable-kvm -smp 4 -serial stdio -s -drive format=raw,file=$(ISO) -vga virtio -cpu host

.PHONY: run
run: build
	qemu-system-x86_64 $(QEMU_OPTS)

.PHONY: run-dbg
run-dbg: build
	qemu-system-x86_64 -S $(QEMU_OPTS)

.PHONY: run-uefi
run-uefi: build
	qemu-system-x86_64 -bios $(QEMU_UEFI) $(QEMU_OPTS)

.PHONY: install
install: build
	@test $(dev) || (echo Need argument 'dev=<device_path>' for target \'install\' && false)
	./scripts/install.sh $(shell pwd) $(ISO) $(dev) $(LOG_DD)

.PHONY: clean
clean:
	$(MAKE) --no-print-directory -C kernel clean
	$(MAKE) --no-print-directory -C user init-clean
	$(MAKE) --no-print-directory -C libc clean
	rm -rf bin
	rm -f $(ISO)
	rm -rf $(FS_DIR)
	rm -rf $(LOG_DIR)
