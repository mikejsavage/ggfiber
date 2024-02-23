#pragma once

#include <stddef.h>

struct VolatileRegisters {
	void * rip;
	void * rsp;
	void * rbp;
	void * rbx;
	void * r12;
	void * r13;
	void * r14;
	void * r15;

#if _WIN32
	struct XMMRegister {
		alignas( 16 ) char value[ 16 ];
	};

	XMMRegister xmm6, xmm7, xmm8, xmm9, xmm10, xmm11, xmm12, xmm13, xmm14, xmm15;
#endif
};

using FiberCallback = void ( * )( void * );

void MakeFiberContext( VolatileRegisters * fiber, FiberCallback callback, void * callback_arg, void * stack, size_t stack_size );
void SwitchContext( VolatileRegisters * from, const VolatileRegisters * to );

