.code

ALIGN 16
FiberWrapper proc
	mov rcx, r13
	jmp r12
FiberWrapper endp

ALIGN 16
SwitchContextWindows proc
	lea rax, QWORD PTR [yield_target]
	mov QWORD PTR [rcx + 0 * 8], rax
	mov QWORD PTR [rcx + 1 * 8], rsp
	mov QWORD PTR [rcx + 2 * 8], rbp
	mov QWORD PTR [rcx + 3 * 8], rbx
	mov QWORD PTR [rcx + 4 * 8], r12
	mov QWORD PTR [rcx + 5 * 8], r13
	mov QWORD PTR [rcx + 6 * 8], r14
	mov QWORD PTR [rcx + 7 * 8], r15
	mov QWORD PTR [rcx + 8 * 8], rdi
	mov QWORD PTR [rcx + 9 * 8], rsi
	movaps XMMWORD PTR [rcx + 10 * 8], xmm6
	movaps XMMWORD PTR [rcx + 12 * 8], xmm7
	movaps XMMWORD PTR [rcx + 14 * 8], xmm8
	movaps XMMWORD PTR [rcx + 16 * 8], xmm9
	movaps XMMWORD PTR [rcx + 18 * 8], xmm10
	movaps XMMWORD PTR [rcx + 20 * 8], xmm11
	movaps XMMWORD PTR [rcx + 22 * 8], xmm12
	movaps XMMWORD PTR [rcx + 24 * 8], xmm13
	movaps XMMWORD PTR [rcx + 26 * 8], xmm14
	movaps XMMWORD PTR [rcx + 28 * 8], xmm15
	mov r10, QWORD PTR gs:[30h]
	mov r11, QWORD PTR [r10 + 8h]
	mov QWORD PTR [rcx + 30 * 8], r11
	mov r11, QWORD PTR [r10 + 10h]
	mov QWORD PTR [rcx + 31 * 8], r11
	mov r11, QWORD PTR [r10 + 1478h]
	mov QWORD PTR [rcx + 32 * 8], r11
	mov r11, QWORD PTR [r10 + 20h]
	mov QWORD PTR [rcx + 33 * 8], r11

	mov r11, QWORD PTR [rdx + 33 * 8]
	mov QWORD PTR [r10 + 20h], r11
	mov r11, QWORD PTR [rdx + 32 * 8]
	mov QWORD PTR [r10 + 1478h], r11
	mov r11, QWORD PTR [rdx + 31 * 8]
	mov QWORD PTR [r10 + 10h], r11
	mov r11, QWORD PTR [rdx + 30 * 8]
	mov QWORD PTR [r10 + 8h], r11
	movaps xmm15, XMMWORD PTR [rdx + 28 * 8]
	movaps xmm14, XMMWORD PTR [rdx + 26 * 8]
	movaps xmm13, XMMWORD PTR [rdx + 24 * 8]
	movaps xmm12, XMMWORD PTR [rdx + 22 * 8]
	movaps xmm11, XMMWORD PTR [rdx + 20 * 8]
	movaps xmm10, XMMWORD PTR [rdx + 18 * 8]
	movaps xmm9,  XMMWORD PTR [rdx + 16 * 8]
	movaps xmm8,  XMMWORD PTR [rdx + 14 * 8]
	movaps xmm7,  XMMWORD PTR [rdx + 12 * 8]
	movaps xmm6,  XMMWORD PTR [rdx + 10 * 8]
	mov rsi, QWORD PTR [rdx + 9 * 8]
	mov rdi, QWORD PTR [rdx + 8 * 8]
	mov r15, QWORD PTR [rdx + 7 * 8]
	mov r14, QWORD PTR [rdx + 6 * 8]
	mov r13, QWORD PTR [rdx + 5 * 8]
	mov r12, QWORD PTR [rdx + 4 * 8]
	mov rbx, QWORD PTR [rdx + 3 * 8]
	mov rbp, QWORD PTR [rdx + 2 * 8]
	mov rsp, QWORD PTR [rdx + 1 * 8]
	jmp QWORD PTR [rdx + 0 * 8]

yield_target:
	ret
SwitchContextWindows endp

end
