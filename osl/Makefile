OSL     := ../bin/osl.bin
OSL_ELF := ../bin/osl.elf

CC      := x86_64-elf-gcc
LD      := x86_64-elf-ld
OBJCOPY := x86_64-elf-objcopy

CFLAGS =												\
	-Isrc -nostdinc										\
	-Wall -Wextra										\
	-O2 -pipe -mcmodel=large							\
	-ffreestanding -fno-stack-protector					\
	-MMD -MP

LDFLAGS =												\
	-Tosl.ld											\
	-nostdlib											\
	-zmax-page-size=0x1000								\
	-static


OBJDIR       := .bin
ALL_SRC_DIRS := $(shell find . -type d ! -path "./$(OBJDIR)" ! -path "./$(OBJDIR)/*" -printf '%P\n')
MKDIRS       := $(patsubst %, $(OBJDIR)/%, $(ALL_SRC_DIRS))
MKDIRS       += $(OBJDIR)

CFILES       := $(shell find . -type f -name '*.c' -printf '%P\n')

OSL_OBJS     := $(CFILES:%.c=$(OBJDIR)/%.o)

HEADER_DEPS  := $(CFILES:%.c=$(OBJDIR)/%.d)

.PHONY: all
all: $(MKDIRS) $(OSL)

$(MKDIRS):
	mkdir -p $(MKDIRS)

$(OSL): $(OSL_OBJS)
	$(LD) $(OSL_OBJS) $(LDFLAGS) -o $(OSL_ELF)
	$(OBJCOPY) -O binary -j .text -j .data -j .rodata -j .bss $(OSL_ELF) $(OSL)

-include $(HEADER_DEPS)
$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OSL) $(OSL_ELF) $(OSL_OBJS) $(HEADER_DEPS)
