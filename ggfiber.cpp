#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "ggfiber.h"

static void FiberWrapper() {
	asm volatile( "movq %%r13, %%rdi" : : : "rdi" );
	asm volatile( "jmpq *%%r12" : );
}

void MakeFiberContext( VolatileRegisters * fiber, FiberCallback callback, void * callback_arg, void * stack, size_t stack_size ) {
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

	uintptr_t based = 0xba5ed0dead0ba5ed;
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
