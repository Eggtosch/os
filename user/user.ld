OUTPUT_FORMAT(elf64-x86-64)
OUTPUT_ARCH(i386:x86-64)

ENTRY(_start)

PHDRS
{
	null	PT_NULL		FLAGS(0);
	/* read and execute */
	text	PT_LOAD		FLAGS((1 << 0) | (1 << 2));
	/* read */
	rodata	PT_LOAD		FLAGS((1 << 2));
	/* read and write */
	data	PT_LOAD		FLAGS((1 << 1) | (1 << 2));
}

SECTIONS
{
	/* start address */
	. = 0x400000;

	.text : {
		*(.text .text.*)
	} :text

	.rodata : {
		*(.rodata .rodata.*)
	} :rodata

	.data : {
		*(.data .data.*)
	} :data

	.bss : {
		*(COMMON)
		*(.bss .bss.*)
	} :data
}
