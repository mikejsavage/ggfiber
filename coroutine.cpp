#include <stdio.h>
#include <assert.h>

#include "ggfiber.h"

struct Coroutine;
using CoroutineCallback = void ( * )( Coroutine * c );

struct Coroutine {
	static constexpr size_t STACK_SIZE = 1024 * 16;
	alignas( 16 ) char stack[ STACK_SIZE ];

	CoroutineCallback callback;
	GGFiberContext coroutine_context;
	GGFiberContext yield_context;
};

static void CoroutineWrapper( void * arg ) {
	Coroutine * c = ( Coroutine * ) arg;
	c->callback( c );
	GGFiberSwapContext( &c->coroutine_context, &c->yield_context );
	assert( false );
}

static void InitCoroutine( Coroutine * c, CoroutineCallback callback ) {
	*c = { };
	c->callback = callback;
	GGFiberMakeContext( &c->coroutine_context, CoroutineWrapper, c, c->stack, sizeof( c->stack ) );
}

static void ResumeCoroutine( Coroutine * c ) {
	GGFiberSwapContext( &c->yield_context, &c->coroutine_context );
}

static void Yield( Coroutine * c ) {
	GGFiberSwapContext( &c->coroutine_context, &c->yield_context );
}

#include <emmintrin.h>

static void f( Coroutine * c ) {
	float a = 1.0f;
	__m128 x;
	_mm_store_ps( ( float * ) &x, _mm_set1_ps( a ) );
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
