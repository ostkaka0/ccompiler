#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#ifdef _WIN32
#define BREAKPOINT __debugbreak()
#else
#include <signal.h>
#define BREAKPOINT raise(SIGTRAP)
#endif
#define RUNTIME_ERROR(...) (fprintf(stderr, "Error at %s #%i: ", __FILE__, __LINE__), fprintf(stderr, __VA_ARGS__), fprintf(stderr, "\n"), fflush(stderr), BREAKPOINT, assert(0), exit(-1))
#define WARNING(...) (fprintf(stderr, "Warning at %s #%i: ", __FILE__, __LINE__), fprintf(stderr, __VA_ARGS__), fprintf(stderr, "\n"), ddluah(stderr), BREAKPOINT)
#define MALLOC_STRUCT(T) ((T*)_malloc_struct(sizeof(T)))
//#define ALIGN_OF(T) ((size_t)&((struct { char c; T t; } *)0)->t)

void* _malloc_struct(size_t size) {
    void* buf = malloc(size);
    if (!buf) RUNTIME_ERROR("Out of memory");
    return buf;
}


#define JOIN2_(a, b) a##b
#define JOIN3_(a, b, c) a##b##c
#define JOIN2(a, b) JOIN2_(a, b)
#define JOIN3(a, b, c) JOIN3_(a, b, c)

#ifdef _MSC_VER
#define INLINE __forceinline
#else
#define INLINE static inline __attribute__((always_inline))
#define NOINLINE __attribute__((noinline))
#endif

#define ZERO(a) memset(a, 0, sizeof(*(a)))

#define MIN(a, b) ((a < b)? a : b)
#define MAX(a, b) ((a > b)? a : b)
#define CLAMP(x, a, b) MIN(MAX(x, a), b)

#define DIV_FLOOR(n, d) (((n) < 0)? (n) / (d) - 1 : (n) / (d))
#define REM_FLOOR(n, d) (((n) < 0)? (n) % (d) + (d) : (n) % (d))
