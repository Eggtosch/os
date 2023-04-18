OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)

ENTRY(_start)

PHDRS
{
	null	PT_NULL		FLAGS(0);
	text	PT_LOAD		FLAGS((1 << 0) | (1 << 2));
	rodata	PT_LOAD		FLAGS((1 << 2));
	data	PT_LOAD		FLAGS((1 << 1) | (1 << 2));
}

SECTIONS
{
	. = 0xffffffff80000000;

	.text : {
		*(.text .text.*);
	} :text

	PROVIDE(__etext = .);

	. += CONSTANT(MAXPAGESIZE);

	.limine_reqs : {
		KEEP(*(.limine_reqs));
	} :rodata

	.driver_init : {
		__start_driver_init = .;
		KEEP(*(.driver_init));
		__stop_driver_init = .;
	} :rodata

	.rodata : {
		*(.rodata .rodata.*);
	} :rodata

	. += CONSTANT(MAXPAGESIZE);

	.data : {
		*(.data .data.*);
	} :data

	.bss : {
		*(COMMON);
		*(.bss .bss.*);
	} :data
}
