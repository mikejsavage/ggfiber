# ggfiber

ggfiber is a tiny library that provides the building blocks for implementing cooperative
multitasking on x86_64 Windows/Linux and x86_64/arm64 macOS. ggfiber has no concept of fiber or
coroutine objects itself, instead it exposes two functions for creating and switching between
contexts, which are simply POD structs containing a snapshot of the processor's execution state, and
stack memory buffers that you manage yourself.

The API in its entirety is:

```cpp
using FiberCallback = void ( * )( void * );
struct VolatileRegisters;

void MakeFiberContext( VolatileRegisters * fiber,
	FiberCallback callback, void * callback_arg,
	void * stack, size_t stack_size );
void SwitchContext( VolatileRegisters * from, const VolatileRegisters * to );
```

As such ggfiber doesn't implement coroutines, but you can use ggfiber to implement them yourself in
30 lines of code, see [coroutine.cpp](https://github.com/mikejsavage/ggfiber/blob/master/coroutine.cpp).

ggfiber's implementation was heavily guided by [LuaCoco](https://coco.luajit.org) and
[minicoro](https://github.com/edubart/minicoro), which itself is heavily guided by LuaCoco.

## Basic example code

```cpp
#include <stdio.h>
#include "ggfiber.h"

static void f( void * bye ) {
	printf( "hello\n" );
	VolatileRegisters dummy;
	SwitchContext( &dummy, ( VolatileRegisters * ) bye );
}

int main() {
	VolatileRegisters bye, hello;
	// printf needs a surprising amount of stack space
	alignas( 16 ) char stack[ 16 * 1024 ];
	MakeFiberContext( &hello, f, &bye, stack, sizeof( stack ) );
	SwitchContext( &bye, &hello );
	printf( "bye\n" );
	return 0;
}
```
