KERNEL := ../bin/kernel.elf

CC := ../cross-compiler/bin/x86_64-elf-gcc
LD := ../cross-compiler/bin/x86_64-elf-ld

CFLAGS =												\
	-I. -nostdinc										\
	-Wall -Wextra										\
	-O2 -pipe -g -ggdb									\
	-ffreestanding -fno-stack-protector -fno-pic 		\
	-mno-80387 -mno-mmx -mno-3dnow -mno-sse -mno-sse2 	\
	-mno-red-zone -mcmodel=kernel 						\
	-MMD -MP

LDFLAGS =												\
	-Tkernel.ld											\
	-nostdlib											\
	-zmax-page-size=0x1000								\
	-static

OBJDIR       := .bin
ALL_SRC_DIRS := $(shell find . -type d ! -path "./$(OBJDIR)" ! -path "./$(OBJDIR)/*" -printf '%P\n')
MKDIRS       := $(patsubst %, $(OBJDIR)/%, $(ALL_SRC_DIRS))
MKDIRS       += $(OBJDIR)

CFILES       := $(shell find . -type f -name '*.c' -printf '%P\n')
ASMFILES     := $(shell find . -type f -name '*.asm' -printf '%P\n')

ASMOBJS      := $(ASMFILES:%.asm=$(OBJDIR)/%.o)
KERNEL_OBJS  := $(CFILES:%.c=$(OBJDIR)/%.o)
KERNEL_OBJS  += $(ASMOBJS)

HEADER_DEPS  := $(CFILES:%.c=$(OBJDIR)/%.d)

.PHONY: all
all: $(MKDIRS) $(KERNEL)

$(MKDIRS):
	mkdir -p $(MKDIRS)

$(KERNEL): $(KERNEL_OBJS)
	@$(LD) $(KERNEL_OBJS) $(LDFLAGS) -o $@
	@echo LD $(@F)

$(OBJDIR)/%.o: %.asm
	@nasm -f elf64 $< -o $@
	@echo ASM $(@F)

-include $(HEADER_DEPS)
$(OBJDIR)/%.o: %.c
	@$(CC) $(CFLAGS) -c $< -o $@
	@echo CC $(@F)

.PHONY: clean
clean:
	rm -rf $(KERNEL)
	rm -rf $(KERNEL_OBJS)
	rm -rf $(HEADER_DEPS)
