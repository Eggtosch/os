[bits 64]

global asm_jump_usermode

; (rip, rsp, arg1, arg2)
asm_jump_usermode:
	mov ax, 0x43
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	push 0x43
	push rsi
	pushf
	push 0x3b
	push rdi
	mov rdi, rdx
	mov rsi, rcx

	iretq
