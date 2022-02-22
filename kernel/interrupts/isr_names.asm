[bits 64]

%macro pusha 0
	push rax
	push rbx
	push rcx
	push rdx
	push rbp
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15
%endmacro

%macro popa 0
	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rbp
	pop rdx
	pop rcx
	pop rbx
	pop rax
%endmacro

extern interrupt_handler

global asm_isr_names

asm_isr_handler:
	cld
	pusha

	mov rdi, rsp
	call interrupt_handler
	mov rsp, rax

	popa
	add rsp, 16

	iretq

%macro ISR_NAME 1
dq _ISR%1
%endmacro

%macro ISR_STUB_ERR 1
_ISR%1:
	push %1
	jmp asm_isr_handler
%endmacro

%macro ISR_STUB_NO_ERR 1
_ISR%1:
	push 0
	push %1
	jmp asm_isr_handler
%endmacro

ISR_STUB_NO_ERR	0
ISR_STUB_NO_ERR	1
ISR_STUB_NO_ERR	2
ISR_STUB_NO_ERR	3
ISR_STUB_NO_ERR	4
ISR_STUB_NO_ERR	5
ISR_STUB_NO_ERR	6
ISR_STUB_NO_ERR	7
ISR_STUB_ERR   	8
ISR_STUB_NO_ERR	9
ISR_STUB_ERR   	10
ISR_STUB_ERR   	11
ISR_STUB_ERR   	12
ISR_STUB_ERR   	13
ISR_STUB_ERR   	14
ISR_STUB_NO_ERR	15
ISR_STUB_NO_ERR	16
ISR_STUB_ERR   	17
ISR_STUB_NO_ERR	18
ISR_STUB_NO_ERR	19
ISR_STUB_NO_ERR	20
ISR_STUB_ERR   	21
ISR_STUB_NO_ERR	22
ISR_STUB_NO_ERR	23
ISR_STUB_NO_ERR	24
ISR_STUB_NO_ERR	25
ISR_STUB_NO_ERR	26
ISR_STUB_NO_ERR	27
ISR_STUB_NO_ERR	28
ISR_STUB_NO_ERR	29
ISR_STUB_NO_ERR	30
ISR_STUB_NO_ERR	31

%assign i 32
%rep 224
ISR_STUB_NO_ERR i
%assign i i+1
%endrep

asm_isr_names:
	%assign i 0
	%rep 256
	ISR_NAME i
	%assign i i+1
	%endrep
