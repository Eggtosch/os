PROGRAMS := $(shell find . -mindepth 1 -maxdepth 1 -type d)

.PHONY: all $(PROGRAMS)
all: $(PROGRAMS)

$(PROGRAMS):
	make --no-print-directory -f Makefile.user M=$@

$(PROGRAMS)-clean:
	make --no-print-directory -f Makefile.user M=$(patsubst %-clean,%,$@) clean
