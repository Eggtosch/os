# build $(M) target if M is specified,
# else build all applications found directly in this directory
ifdef M
PROGRAMS := $(M)
else
PROGRAMS := $(shell find * -mindepth 0 -maxdepth 0 -type d)
endif

.PHONY: all $(PROGRAMS)
all: $(PROGRAMS)

$(PROGRAMS):
	make --no-print-directory -f Makefile.user M=$@

$(PROGRAMS)-clean:
	make --no-print-directory -f Makefile.user M=$(patsubst %-clean,%,$@) clean

ifdef M
clean:
	make --no-print-directory -f Makefile.user M=$(M) clean
endif
