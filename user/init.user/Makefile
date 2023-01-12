CC      := ../../cross-compiler/bin/x86_64-elf-gcc
LD      := ../../cross-compiler/bin/x86_64-elf-ld

CFLAGS =									\
	-Isrc/ -I../../libc/include/ -nostdinc	\
	-Wall -Wextra							\
	-O2 -pipe -mcmodel=large				\
	-ffreestanding -fno-stack-protector		\
	-MMD -MP

LDFLAGS =									\
	-T../user.ld							\
	-nostdlib								\
	-zmax-page-size=0x1000					\
	-static -L../../libc/bin -lc

OBJDIR       := .bin
ALL_SRC_DIRS := $(shell find . -type d ! -path "./$(OBJDIR)" ! -path "./$(OBJDIR)/*" -printf '%P\n')
MKDIRS       := $(patsubst %, $(OBJDIR)/%, $(ALL_SRC_DIRS))
MKDIRS       += $(OBJDIR)

CFILES       := $(shell find . -type f -name '*.c' -printf '%P\n')
ASMFILES     := $(shell find . -type f -name '*.asm' -printf '%P\n')

ASMOBJS      := $(ASMFILES:%.asm=$(OBJDIR)/%.o)
TARGET_OBJS  := $(CFILES:%.c=$(OBJDIR)/%.o)
TARGET_OBJS  += $(ASMOBJS)

HEADER_DEPS  := $(CFILES:%.c=$(OBJDIR)/%.d)

.PHONY: all
all: $(MKDIRS) $(TARGET)

$(MKDIRS):
	mkdir -p $(MKDIRS)

$(TARGET): $(TARGET_OBJS)
	@$(LD) $(TARGET_OBJS) $(LDFLAGS) -o $@
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
	rm -rf $(TARGET)
	rm -rf $(TARGET_OBJS)
	rm -rf $(HEADER_DEPS)

