// Copyright (c) John Emanuelsson - All Rights Reserved
// File created 2018-12-11

#pragma once
#include <stdint.h>

// TODO: Replace KB with KiB
#define KB (1024ULL     )
#define MB (1024ULL * KB)
#define GB (1024ULL * MB)
#define TB (1024ULL * GB)
#define PB (1024ULL * TB)

#define KIBI (1024ULL       )
#define MEBI (1024ULL * KIBI)
#define GIBI (1024ULL * MEBI)
#define TEBI (1024ULL * GIBI)
#define PEBI (1024ULL * TEBI)

#define KIB KIBI
#define MIB MEBI
#define GIB GIBI
#define TIB TEBI
#define PIB PEBI

//#define KIM KIBI
//#define MIM MIBI
//#define GIM GIBI
//#define TIM TEBI
//#define PIM PEBI

typedef int64_t   i64;
typedef int32_t   i32;
typedef int16_t   i16;
typedef int8_t    i8 ;
typedef uint64_t  u64;
typedef uint32_t  u32;
typedef uint16_t  u16;
typedef uint8_t   u8 ;
typedef intptr_t  iptr;
typedef uintptr_t uptr;
typedef double    f64;
typedef float     f32;

/*
u8  bitcast_to_u8 (void* a) { u8  r; memcpy(&r, a, sizeof(u8 ); return r; }
u16 bitcast_to_u16(void* a) { u16 r; memcpy(&r, a, sizeof(u16); return r; }
u32 bitcast_to_u32(void* a) { u32 r; memcpy(&r, a, sizeof(u32); return r; }
u64 bitcast_to_u64(void* a) { u64 r; memcpy(&r, a, sizeof(u64); return r; }

*/
