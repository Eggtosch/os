PROGRAMS := $(wildcard *.user)
PROGRAMS_CLEAN := $(PROGRAMS:%=%-clean)

BINDIR := ../../bin

.PHONY: all clean-all $(PROGRAMS) $(PROGRAMS_CLEAN)

all: $(PROGRAMS)
clean-all: $(PROGRAMS_CLEAN)

$(PROGRAMS):
	$(eval TARGET := $(BINDIR)/$(patsubst %.user,%.elf,$@))
	$(MAKE) -C $@ TARGET=$(TARGET)

$(PROGRAMS_CLEAN):
	$(eval DIR    := $(patsubst %-clean,%,$@))
	$(eval TARGET := $(BINDIR)/$(patsubst %.user,%.elf,$@))
	$(MAKE) --no-print-directory -C $(DIR) clean TARGET=$(TARGET)

