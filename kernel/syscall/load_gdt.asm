[bits 64]

global asm_load_gdt_and_tss

asm_load_gdt_and_tss:
	lgdt [rdi]

	mov ax, 0x48
	ltr ax

	mov ax, 0x30
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	mov ss, ax

	pop rdi
	mov rax, 0x28
	push rax
	push rdi
	retfq

