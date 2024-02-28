#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "ggfiber.h"

#if GGFIBER_ARCHITECTURE_X86

// TODO: windows specific stuff

[[gnu::noreturn]] static void FiberWrapper() {
	asm volatile( "movq %%r13, %%rdi" : : : "rdi" );
	asm volatile( "jmpq *%%r12" : );
	__builtin_unreachable();
}

void MakeFiberContext( VolatileRegisters * fiber, FiberCallback callback, void * callback_arg, void * stack, size_t stack_size ) {
	assert( uintptr_t( stack ) % 16 == 0 );
	assert( stack_size >= sizeof( void * ) );
	char * char_stack = ( char * ) stack;

	*fiber = {
		.rip = ( void * ) FiberWrapper,
		// TODO: not sure this is completely right but it does give us ba5ed in bt
		.rsp = char_stack + stack_size - 2 * sizeof( void * ),
		.rbp = char_stack + stack_size - 2 * sizeof( void * ),
		.r12 = ( void * ) callback,
		.r13 = ( void * ) callback_arg,
	};

	uintptr_t based = 0xba5ed2ed61113d;
	memcpy( char_stack + stack_size - 2 * sizeof( void * ), &based, sizeof( based ) );
}

void SwitchContext( VolatileRegisters * from, const VolatileRegisters * to ) {
	// save current context to `from`
	asm volatile(
		"leaq 1f(%%rip), %%rax\n"
		"movq %%rax,  0(%0)\n"
		"movq %%rsp,  8(%0)\n"
		"movq %%rbp, 16(%0)\n"
		"movq %%rbx, 24(%0)\n"
		"movq %%r12, 32(%0)\n"
		"movq %%r13, 40(%0)\n"
		"movq %%r14, 48(%0)\n"
		"movq %%r15, 56(%0)\n"
		: "=D"( from ) : : "rax"
	);

	// load context from `to`
	asm volatile(
		"movq 56(%0), %%r15\n"
		"movq 48(%0), %%r14\n"
		"movq 40(%0), %%r13\n"
		"movq 32(%0), %%r12\n"
		"movq 24(%0), %%rbx\n"
		"movq 16(%0), %%rbp\n"
		"movq  8(%0), %%rsp\n"
		"jmpq  *(%0)\n"
		: : "S"( to )
	);

	// we land here when switching back to `from`
	asm volatile(
		"1:\n"
		// after we jump to 1: everything will be clobbered. seems like
		// we should put everything here but GCC won't let us, so just
		// copy what LuaCoco does
		: : : "rax", "rcx", "rdx", "rdi", "r8", "r9", "r10", "r11", "memory", "cc"
	);
}

#endif // GGFIBER_ARCHITECTURE_X86

#if GGFIBER_ARCHITECTURE_ARM64

[[gnu::noreturn]] static void FiberWrapper() {
	asm volatile( "mov x0, x20" );
	asm volatile( "mov x30, x21" );
	asm volatile( "br x19" );
	__builtin_unreachable();
}

void MakeFiberContext( VolatileRegisters * fiber, FiberCallback callback, void * callback_arg, void * stack, size_t stack_size ) {
	assert( uintptr_t( stack ) % 16 == 0 );
	*fiber = {
		.x19 = ( void * ) callback,
		.x20 = callback_arg,
		.x21 = ( void * ) 0xba5ed,
		.sp = ( char * ) stack + stack_size,
		.pc = ( void * ) FiberWrapper,
	};
}

void SwitchContext( VolatileRegisters * from, const VolatileRegisters * to ) {
	// save current context to `from`
	asm volatile(
		"adr x11, 1f\n"
		// NOTE: hardcode x0 instead of using from because the latter uses x8
		// pointing to the wrong thing for some reason
		"stp x19, x20, [x0, #(0 * 16)]\n"
		"stp x21, x22, [x0, #(1 * 16)]\n"
		"stp x23, x24, [x0, #(2 * 16)]\n"
		"stp x25, x26, [x0, #(3 * 16)]\n"
		"stp x27, x28, [x0, #(4 * 16)]\n"
		"stp x29, x30, [x0, #(5 * 16)]\n"
		"stp  d8,  d9, [x0, #(6 * 16)]\n"
		"stp d10, d11, [x0, #(7 * 16)]\n"
		"stp d12, d13, [x0, #(8 * 16)]\n"
		"stp d14, d15, [x0, #(9 * 16)]\n"

		"mov x10, sp\n" // can't str sp directly
		"stp x10, x11, [x0, #(10 * 16)]\n"
		:
	);

	// load context from `to`
	asm volatile(
		"ldp x19, x20, [x1, #(0 * 16)]\n"
		"ldp x21, x22, [x1, #(1 * 16)]\n"
		"ldp x23, x24, [x1, #(2 * 16)]\n"
		"ldp x25, x26, [x1, #(3 * 16)]\n"
		"ldp x27, x28, [x1, #(4 * 16)]\n"
		"ldp x29, x30, [x1, #(5 * 16)]\n"
		"ldp  d8,  d9, [x1, #(6 * 16)]\n"
		"ldp d10, d11, [x1, #(7 * 16)]\n"
		"ldp d12, d13, [x1, #(8 * 16)]\n"
		"ldp d14, d15, [x1, #(9 * 16)]\n"

		"ldp x10, x11, [x1, #(10 * 16)]\n"
		"mov sp, x10\n"

		"br x11\n"
		:
	);

	asm volatile( "1:" );
}

#endif // GGFIBER_ARCHITECTURE_ARM64
