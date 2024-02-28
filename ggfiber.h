#pragma once

#if defined( _WIN32 )
#  define GGFIBER_PLATFORM_WINDOWS 1
#elif defined( __APPLE__ )
#  define GGFIBER_PLATFORM_MACOS 1
#elif defined( __linux__ )
#  define GGFIBER_PLATFORM_LINUX 1
#else
#  error unsupported platform
#endif

#if defined( _M_X64 ) || defined( __x86_64__ )
#  define GGFIBER_ARCHITECTURE_X86 1
#elif defined( __aarch64__ )
#  define GGFIBER_ARCHITECTURE_ARM64 1
#else
#  error unsupported architecture
#endif

#include <stddef.h>

#if GGFIBER_ARCHITECTURE_X86
struct VolatileRegisters {
	using Register = void *;
	Register rip, rsp, rbp, rbx, r12, r13, r14, r15;

#if GGFIBER_PLATFORM_WINDOWS
	struct XMMRegister {
		alignas( 16 ) char value[ 16 ];
	};

	XMMRegister xmm6, xmm7, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15;
#endif
};
#endif

#if GGFIBER_ARCHITECTURE_ARM64
struct VolatileRegisters {
	using Register = void *;
	Register x19, x20, x21, x22, x23, x24, x25, x26, x27, x28, x29, x30;
	Register d8, d9, d10, d11, d12, d13, d14, d15;
	Register sp, pc;
};
#endif

using FiberCallback = void ( * )( void * );

void MakeFiberContext( VolatileRegisters * fiber, FiberCallback callback, void * callback_arg, void * stack, size_t stack_size );
void SwitchContext( VolatileRegisters * from, const VolatileRegisters * to );

