[bits 64]

global asm_jump_usermode

asm_jump_usermode:
	mov ax, 0x43
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov rax, rsp
	push 0x43
	push rax
	pushf
	push 0x3b
	push rdi

	iretq
