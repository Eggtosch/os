[bits 64]

global asm_load_idt

asm_load_idt:
	lidt [rdi]
	sti
	ret
