STATIC_LIB   := ../bin/libc.a

CC           := x86_64-elf-gcc
AR           := x86_64-elf-ar

CFLAGS =												\
	-I. -I../include -nostdinc							\
	-Wall -Wextra										\
	-O2 -pipe											\
	-ffreestanding -fno-stack-protector -fno-pic 		\
	-mno-80387 -mno-mmx -mno-3dnow -mno-sse -mno-sse2 	\
	-mno-red-zone -mcmodel=kernel 						\
	-MMD -MP
ARFLAGS      := -crs

OBJDIR       := .bin
ALL_SRC_DIRS := $(shell find . -type d ! -path "./$(OBJDIR)" ! -path "./$(OBJDIR)/*" -printf '%P\n')
MKDIRS       := $(patsubst %, $(OBJDIR)/%, $(ALL_SRC_DIRS))
MKDIRS       += $(OBJDIR)

CFILES       := $(shell find . -type f -name '*.c' -printf '%P\n')
LIB_OBJS     := $(CFILES:%.c=$(OBJDIR)/%.o)

HEADER_DEPS  := $(CFILES:%.c=$(OBJDIR)/%.d)

.PHONY: all
all: $(MKDIRS) $(STATIC_LIB)

$(MKDIRS):
	mkdir -p $(OBJDIR)
	mkdir -p $@

$(STATIC_LIB): $(LIB_OBJS)
	$(AR) $(ARFLAGS) $@ $(LIB_OBJS)

-include $(HEADER_DEPS)
$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(STATIC_LIB) $(LIB_OBJS) $(HEADER_DEPS)
