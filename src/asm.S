.global int_0x1, int_0x3, int_0x31, int_0xef, do_fake_stub
.align 8, 0x90

.macro pushaq
	push	%rax
	push	%rbx
	push	%rcx
	push	%rdx
	push	%rdi
	push	%rsi
	push	%r8
	push	%r9
	push	%r10
	push	%r11
	push	%r12
	push	%r13
	push	%r14
	push	%r15
    mov     %ds, %rax
    push    %rax
    mov     %es, %rax
    push    %rax
    mov     %fs, %rax
    push    %rax
    mov     %gs, %rax
    push    %rax
.endm

.macro popaq
    add     $0x10, %rsp
    pop     %rax
    mov     %rax, %es
    pop     %rax
    mov     %rax, %ds
	pop 	%r15
	pop 	%r14
	pop 	%r13
	pop 	%r12
	pop 	%r11
	pop 	%r10
	pop 	%r9
	pop 	%r8
	pop 	%rsi
	pop 	%rdi
	pop 	%rdx
	pop 	%rcx
	pop 	%rbx
	pop 	%rax
.endm

do_fake_stub:
	pushaq
	push	%rbp
	mov		%rsp, %rsi
	mov 	%rsp, %rdi
	testl	$0x3, 0xB0(%rsi)
	jz		1f
	swapgs
1:
	push	%rsi
	call	int_handler
	cli
	pop 	%rsi
	mov		%rsi, %rsp

	testl	$0x3, 0xB0(%rsp)
	jz		1f
	swapgs
1:
	pop		%rbp
	popaq
	addq	$0x10, %rsp
	iretq
	
int_0x1:
	pushq	$0x0
	pushq	$0x1
	jmp		do_fake_stub

int_0x3:
	pushq	$0x0
	pushq	$0x3
	jmp		do_fake_stub
	
int_0xef:
	pushq	$0x0
	pushq	$0xef
	jmp		do_fake_stub
	
int_0x31:
	pushq	$0x0
	pushq	$0x31
	jmp		do_fake_stub
