#pragma once
#include <immintrin.h>

#if 0
typedef __m512i u8x64;
typedef __m512i u16x32;
typedef __m512i u32x16;
typedef __m512i u64x8;
#else
typedef struct { __m256i v[2]; } u8x64;
typedef struct { __m256i v[2]; } u16x32;
typedef struct { __m256i v[2]; } u32x16;
typedef struct { __m256i v[2]; } u64x8;
#endif

typedef __m256i u8x32;
typedef __m256i u16x16;
typedef __m256i u32x8;
typedef __m256i u64x4;

typedef __m128i u8x16;
typedef __m128i u16x8;
typedef __m128i u32x4;
typedef __m128i u64x2;

typedef u64 u8x8;
typedef u64 u16x4;
typedef u64 u32x2;

typedef u32 u8x4;
typedef u32 u16x2;

typedef u16 u8x2;




INLINE u8x16 u8x16_add(u8x16 a, u8x16 b) {
    return _mm_add_epi8(a, b);
}
INLINE u16x8 u16x8_add(u16x8 a, u16x8 b) {
    return _mm_add_epi16(a, b);
}
INLINE u32x4 u32x4_add(u32x4 a, u32x4 b) {
    return _mm_add_epi32(a, b);
}
INLINE u64x2 u64x2_add(u64x2 a, u64x2 b) {
    return _mm_add_epi64(a, b);
}

INLINE u8x32 u8x32_add(u8x32 a, u8x32 b) {
    return _mm256_add_epi8(a, b);
}
INLINE u16x16 u16x16_add(u16x16 a, u16x16 b) {
    return _mm256_add_epi16(a, b);
}
INLINE u32x8 u32x8_add(u32x8 a, u32x8 b) {
    return _mm256_add_epi32(a, b);
}
INLINE u64x4 u64x4_add(u64x4 a, u64x4 b) {
    return _mm256_add_epi64(a, b);
}

#if 0
INLINE u8x64 u8x64_add(u8x64 a, u8x64 b) {
    return _mm512_add_epi8(a, b);
}
INLINE u16x32 u16x32_add(u16x32 a, u16x32) {
    return _mm512_add_epi16(a, b);
}
INLINE u32x16 u32x16_add(u32x16 a, u32x16 b) {
    return _mm512_add_epi32(a, b);
}
INLINE u64x8 u64x8_add(u64x8 a, u64x8 b) {
    return _mm512_add_epi64(a, b);
}
#else
INLINE u8x64 u8x64_add(u8x64 a, u8x64 b) {
    u8x64 r;
    r.v[0] = u8x32_add(a.v[0], b.v[0]);
    r.v[1] = u8x32_add(a.v[1], b.v[1]);
    return r;
}
INLINE u16x32 u16x32_add(u16x32 a, u16x32 b) {
    u16x32 r;
    r.v[0] = u16x16_add(a.v[0], b.v[0]);
    r.v[1] = u16x16_add(a.v[1], b.v[1]);
    return r;
}
INLINE u32x16 u32x16_add(u32x16 a, u32x16 b) {
    u32x16 r;
    r.v[0] = u32x8_add(a.v[0], b.v[0]);
    r.v[1] = u32x8_add(a.v[1], b.v[1]);
    return r;
}
INLINE u64x8 u64x8_add(u64x8 a, u64x8 b) {
    u64x8 r;
    r.v[0] = u64x4_add(a.v[0], b.v[0]);
    r.v[1] = u64x4_add(a.v[1], b.v[1]);
    return r;
}
#endif