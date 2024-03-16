#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

#include "ggfiber.h"

static_assert( sizeof( void * ) == 8, "64bit only" );

/*
 * x64 Windows
 */

#if GGFIBER_ARCHITECTURE_X86 && GGFIBER_PLATFORM_WINDOWS

#if defined( _MSC_VER )
#  pragma section( ".text" )
#  define INLINE_X64 __declspec( allocate( ".text" ) )
#else
#  define INLINE_X64 [[gnu::section( ".text" )]]
#endif

alignas( 16 ) INLINE_X64 static const uint8_t FiberWrapperX64[] = {
	0x4c, 0x89, 0xe9, // mov rcx, r13
	0x41, 0xff, 0xe4, // jmp r12
	0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, // int3
};

alignas( 16 ) INLINE_X64 static const uint8_t SwitchContextX64[] = {
	0x48, 0x8d, 0x05, 0x3e, 0x01, 0x00, 0x00,             // lea rax, [.yield_target]
	0x48, 0x89, 0x01,                                     // mov [rcx + 0 * 8], rax
	0x48, 0x89, 0x61, 0x08,                               // mov [rcx + 1 * 8], rsp
	0x48, 0x89, 0x69, 0x10,                               // mov [rcx + 2 * 8], rbp
	0x48, 0x89, 0x59, 0x18,                               // mov [rcx + 3 * 8], rbx
	0x4c, 0x89, 0x61, 0x20,                               // mov [rcx + 4 * 8], r12
	0x4c, 0x89, 0x69, 0x28,                               // mov [rcx + 5 * 8], r13
	0x4c, 0x89, 0x71, 0x30,                               // mov [rcx + 6 * 8], r14
	0x4c, 0x89, 0x79, 0x38,                               // mov [rcx + 7 * 8], r15
	0x48, 0x89, 0x79, 0x40,                               // mov [rcx + 8 * 8], rdi
	0x48, 0x89, 0x71, 0x48,                               // mov [rcx + 9 * 8], rsi
	0x0f, 0x29, 0x71, 0x50,                               // movaps [rcx + 10 * 8], xmm6
	0x0f, 0x29, 0x79, 0x60,                               // movaps [rcx + 12 * 8], xmm7
	0x44, 0x0f, 0x29, 0x41, 0x70,                         // movaps [rcx + 14 * 8], xmm8
	0x44, 0x0f, 0x29, 0x89, 0x80, 0x00, 0x00, 0x00,       // movaps [rcx + 16 * 8], xmm9
	0x44, 0x0f, 0x29, 0x91, 0x90, 0x00, 0x00, 0x00,       // movaps [rcx + 18 * 8], xmm10
	0x44, 0x0f, 0x29, 0x99, 0xa0, 0x00, 0x00, 0x00,       // movaps [rcx + 20 * 8], xmm11
	0x44, 0x0f, 0x29, 0xa1, 0xb0, 0x00, 0x00, 0x00,       // movaps [rcx + 22 * 8], xmm12
	0x44, 0x0f, 0x29, 0xa9, 0xc0, 0x00, 0x00, 0x00,       // movaps [rcx + 24 * 8], xmm13
	0x44, 0x0f, 0x29, 0xb1, 0xd0, 0x00, 0x00, 0x00,       // movaps [rcx + 26 * 8], xmm14
	0x44, 0x0f, 0x29, 0xb9, 0xe0, 0x00, 0x00, 0x00,       // movaps [rcx + 28 * 8], xmm15
	0x65, 0x4c, 0x8b, 0x14, 0x25, 0x30, 0x00, 0x00, 0x00, // mov r10, [gs:dword 0x30]
	0x4d, 0x8b, 0x5a, 0x08,                               // mov r11, [r10 + 0x8]
	0x4c, 0x89, 0x99, 0xf0, 0x00, 0x00, 0x00,             // mov [rcx + 30 * 8], r11
	0x4d, 0x8b, 0x5a, 0x10,                               // mov r11, [r10 + 0x10]
	0x4c, 0x89, 0x99, 0xf8, 0x00, 0x00, 0x00,             // mov [rcx + 31 * 8], r11
	0x4d, 0x8b, 0x9a, 0x78, 0x14, 0x00, 0x00,             // mov r11, [r10 + 0x1478]
	0x4c, 0x89, 0x99, 0x00, 0x01, 0x00, 0x00,             // mov [rcx + 32 * 8], r11
	0x4d, 0x8b, 0x5a, 0x20,                               // mov r11, [r10 + 0x20]
	0x4c, 0x89, 0x99, 0x08, 0x01, 0x00, 0x00,             // mov [rcx + 33 * 8], r11
	0x4c, 0x8b, 0x9a, 0x08, 0x01, 0x00, 0x00,             // mov r11, [rdx + 33 * 8]
	0x4d, 0x89, 0x5a, 0x20,                               // mov [r10 + 0x20], r11
	0x4c, 0x8b, 0x9a, 0x00, 0x01, 0x00, 0x00,             // mov r11, [rdx + 32 * 8]
	0x4d, 0x89, 0x9a, 0x78, 0x14, 0x00, 0x00,             // mov [r10 + 0x1478], r11
	0x4c, 0x8b, 0x9a, 0xf8, 0x00, 0x00, 0x00,             // mov r11, [rdx + 31 * 8]
	0x4d, 0x89, 0x5a, 0x10,                               // mov [r10 + 0x10], r11
	0x4c, 0x8b, 0x9a, 0xf0, 0x00, 0x00, 0x00,             // mov r11, [rdx + 30 * 8]
	0x4d, 0x89, 0x5a, 0x08,                               // mov [r10 + 0x8], r11
	0x44, 0x0f, 0x28, 0xba, 0xe0, 0x00, 0x00, 0x00,       // movaps xmm15, [rdx + 28 * 8]
	0x44, 0x0f, 0x28, 0xb2, 0xd0, 0x00, 0x00, 0x00,       // movaps xmm14, [rdx + 26 * 8]
	0x44, 0x0f, 0x28, 0xaa, 0xc0, 0x00, 0x00, 0x00,       // movaps xmm13, [rdx + 24 * 8]
	0x44, 0x0f, 0x28, 0xa2, 0xb0, 0x00, 0x00, 0x00,       // movaps xmm12, [rdx + 22 * 8]
	0x44, 0x0f, 0x28, 0x9a, 0xa0, 0x00, 0x00, 0x00,       // movaps xmm11, [rdx + 20 * 8]
	0x44, 0x0f, 0x28, 0x92, 0x90, 0x00, 0x00, 0x00,       // movaps xmm10, [rdx + 18 * 8]
	0x44, 0x0f, 0x28, 0x8a, 0x80, 0x00, 0x00, 0x00,       // movaps xmm9,  [rdx + 16 * 8]
	0x44, 0x0f, 0x28, 0x42, 0x70,                         // movaps xmm8,  [rdx + 14 * 8]
	0x0f, 0x28, 0x7a, 0x60,                               // movaps xmm7,  [rdx + 12 * 8]
	0x0f, 0x28, 0x72, 0x50,                               // movaps xmm6,  [rdx + 10 * 8]
	0x48, 0x8b, 0x72, 0x48,                               // mov rsi, [rdx + 9 * 8]
	0x48, 0x8b, 0x7a, 0x40,                               // mov rdi, [rdx + 8 * 8]
	0x4c, 0x8b, 0x7a, 0x38,                               // mov r15, [rdx + 7 * 8]
	0x4c, 0x8b, 0x72, 0x30,                               // mov r14, [rdx + 6 * 8]
	0x4c, 0x8b, 0x6a, 0x28,                               // mov r13, [rdx + 5 * 8]
	0x4c, 0x8b, 0x62, 0x20,                               // mov r12, [rdx + 4 * 8]
	0x48, 0x8b, 0x5a, 0x18,                               // mov rbx, [rdx + 3 * 8]
	0x48, 0x8b, 0x6a, 0x10,                               // mov rbp, [rdx + 2 * 8]
	0x48, 0x8b, 0x62, 0x08,                               // mov rsp, [rdx + 1 * 8]
	0xff, 0x22,                                           // jmp qword [rdx + 0 * 8]
	0xc3,                                                 // .yield_target: ret
	0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, 0xcc, // int3
};

void MakeFiberContext( VolatileRegisters * fiber, FiberCallback callback, void * callback_arg, void * stack, size_t stack_size ) {
	constexpr size_t kShadowStackSize = 32;

	assert( ( uintptr_t( stack ) + stack_size ) % 16 == 0 );
	assert( stack_size >= kShadowStackSize + sizeof( void * ) );
	char * char_stack = ( char * ) stack;

	*fiber = {
		.rip = ( void * ) FiberWrapperX64,
		// NOTE: Windows expects the stack to be aligned before `call`, which pushes a return
		// address, so when switching into a fiber we need to misalign the stack by 8
		.rsp = char_stack + stack_size - kShadowStackSize - sizeof( void * ),
		.r12 = ( void * ) callback,
		.r13 = ( void * ) callback_arg,
		.tib = {
			.stack_base = char_stack + stack_size,
			.stack_top = stack,
			.stack_dealloc = stack,
		},
	};

	uintptr_t iced = 0x1ced;
	memcpy( fiber->rsp, &iced, sizeof( iced ) );
}

void SwitchContext( VolatileRegisters * from, const VolatileRegisters * to ) {
	using SwitchContextSignature = void ( * )( VolatileRegisters * from, const VolatileRegisters * to );
	SwitchContextSignature f = ( SwitchContextSignature ) ( void * ) SwitchContextX64;
	f( from, to );
}

#endif // GGFIBER_ARCHITECTURE_X86 && GGFIBER_PLATFORM_WINDOWS

/*
 * x64 non-Windows
 */

#if GGFIBER_ARCHITECTURE_X86 && !GGFIBER_PLATFORM_WINDOWS

[[gnu::noreturn]] static void FiberWrapper() {
	asm volatile( "movq %%r13, %%rdi" : : : "rdi" );
	asm volatile( "jmpq *%%r12" : );
	__builtin_unreachable();
}

void MakeFiberContext( VolatileRegisters * fiber, FiberCallback callback, void * callback_arg, void * stack, size_t stack_size ) {
	assert( ( uintptr_t( stack ) + stack_size ) % 16 == 0 );
	assert( stack_size >= sizeof( void * ) );
	char * char_stack = ( char * ) stack;

	*fiber = {
		.rip = ( void * ) FiberWrapper,
		.rsp = char_stack + stack_size - sizeof( void * ),
		.rbp = char_stack + stack_size - sizeof( void * ),
		.r12 = ( void * ) callback,
		.r13 = ( void * ) callback_arg,
	};

	uintptr_t iced = 0x1ced;
	memcpy( fiber->rsp, &iced, sizeof( iced ) );
}

void SwitchContext( VolatileRegisters * from, const VolatileRegisters * to ) {
	// save current context to `from`
	asm volatile(
		"leaq 1f(%%rip), %%rax\n"
		"movq %%rax, (0 * 8)(%0)\n"
		"movq %%rsp, (1 * 8)(%0)\n"
		"movq %%rbp, (2 * 8)(%0)\n"
		"movq %%rbx, (3 * 8)(%0)\n"
		"movq %%r12, (4 * 8)(%0)\n"
		"movq %%r13, (5 * 8)(%0)\n"
		"movq %%r14, (6 * 8)(%0)\n"
		"movq %%r15, (7 * 8)(%0)\n"
		: "=D"( from ) : : "rax"
	);

	// load context from `to`
	asm volatile(
		"movq (7 * 8)(%0), %%r15\n"
		"movq (6 * 8)(%0), %%r14\n"
		"movq (5 * 8)(%0), %%r13\n"
		"movq (4 * 8)(%0), %%r12\n"
		"movq (3 * 8)(%0), %%rbx\n"
		"movq (2 * 8)(%0), %%rbp\n"
		"movq (1 * 8)(%0), %%rsp\n"
		"jmpq *(%0)\n"
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

#endif // GGFIBER_ARCHITECTURE_X86 && !GGFIBER_PLATFORM_WINDOWS

/*
 * arm64 non-Windows
 */

#if GGFIBER_ARCHITECTURE_ARM64 && !GGFIBER_PLATFORM_WINDOWS

[[gnu::noreturn]] static void FiberWrapper() {
	asm volatile( "mov x0, x20" );
	asm volatile( "mov x30, x21" );
	asm volatile( "br x19" );
	__builtin_unreachable();
}

void MakeFiberContext( VolatileRegisters * fiber, FiberCallback callback, void * callback_arg, void * stack, size_t stack_size ) {
	assert( ( uintptr_t( stack ) + stack_size ) % 16 == 0 );
	*fiber = {
		.x19 = ( void * ) callback,
		.x20 = callback_arg,
		.x21 = ( void * ) 0x1ced,
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

#endif // GGFIBER_ARCHITECTURE_ARM64 && !GGFIBER_PLATFORM_WINDOWS
