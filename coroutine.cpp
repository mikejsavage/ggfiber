#include <stdio.h>
#include <assert.h>

#include "ggfiber.h"

struct Coroutine;
using CoroutineCallback = void ( * )( Coroutine * c );

struct Coroutine {
	static constexpr size_t STACK_SIZE = 1024 * 16;
	alignas( 16 ) char stack[ STACK_SIZE ];

	CoroutineCallback callback;
	VolatileRegisters coroutine_context;
	VolatileRegisters yield_context;
};

static void CoroutineWrapper( void * arg ) {
	Coroutine * c = ( Coroutine * ) arg;
	c->callback( c );
	SwitchContext( &c->coroutine_context, &c->yield_context );
	assert( false );
}

static void InitCoroutine( Coroutine * c, CoroutineCallback callback ) {
	*c = { };
	c->callback = callback;
	MakeFiberContext( &c->coroutine_context, CoroutineWrapper, c, c->stack, sizeof( c->stack ) );
}

static void ResumeCoroutine( Coroutine * c ) {
	SwitchContext( &c->yield_context, &c->coroutine_context );
}

static void Yield( Coroutine * c ) {
	SwitchContext( &c->coroutine_context, &c->yield_context );
}

static void f( Coroutine * c ) {
	float a = 1.0f;
	printf( "yielding once, a = %f\n", a );
	Yield( c );
	a += 1.0f;
	printf( "yielding twice, a = %f\n", a );
}

int main() {
	printf( "start main\n" );
	Coroutine c;
	InitCoroutine( &c, f );
	ResumeCoroutine( &c );
	printf( "yielded\n" );
	ResumeCoroutine( &c );
	printf( "end main\n" );
	return 0;
}
