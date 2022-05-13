// This is unfinished!

#pragma once
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdalign.h>

#include "core/types.h"
#include "core/alloc.h"


#define Array(T) struct { T* at; i64 len; i64 cap; Allocator allocator; }

typedef Array(int) intArray;
typedef Array(u8)  u8Array;
typedef Array(u16) u16Array;
typedef Array(i64) i64Array;
typedef Array(u64) u64Array;
typedef Array(i8)  i8Array;
typedef Array(i16) i16Array;
typedef Array(i32) i32Array;
typedef Array(u32) u32Array;
typedef Array(f32) f32Array;
typedef Array(f64) f64Array;
typedef Array(uptr) uptrArray;
typedef Array(iptr) iptrArray;
typedef Array(void*) PtrArray;
typedef Array(void**) PtrPtrArray;
typedef Array(u8*) CstrArray;
typedef Array(void) voidArray; // Generic array


// Void arrays are untested
#define VOID_ARRAY_FROM(a) (VoidArray){(a).at, (a).len * sizeof(*(a).at), (a).cap * sizeof(*(a).at), (a).allocator}
// Assertions to detect incorrect casting.
#define VOID_ARRAY_TO(a) (assert((a).len % sizeof(*(a).at) == 0), assert((a).cap % sizeof(*(a).at) == 0, assert((iptr)(a).at % align_alignof(*(a).at) == 0), (VoidArray){(a).at, (a).len / sizeof(*(a).at), (a).cap / sizeof(*(a).at), (a).allocator})



#define ARRAY_UNPACK(a) (void**)&(a).at, &(a).len, &(a).cap, &(a).allocator, sizeof(*(a).at), alignof(*(a).at)
#define ARRAY_ARGS(NAME) i64* NAME ## _len, i64* NAME ## _cap, i64 NAME ## _item_size, i64 NAME ## _alignment
#define ARRAY_PASS_ARGS(NAME) NAME ## _len, NAME ## _cap, NAME ## _item_size, NAME ## _alignment

#define array_push(a, ...) (_array_reserve(ARRAY_UNPACK(a), (a).len + 1), (a).at[(a).len++] = (__VA_ARGS__))
#define array_push_uninitialized(a) _array_reserve(ARRAY_UNPACK(a), ++(a).len)
#define array_pop(a) ((a).len--, (a).at[(a).len])
#define array_insert(a, idx, val) (array_push_empty(a), memmove(&(a).at[idx + 1], &(a).at[idx], sizeof(*(a).at) * ((a).len - idx)), (a).at[idx] = val)
#define array_splice(a, idx, len) (memmove(&(a).at[idx], &(a).at[(idx) + (len)], sizeof(*(a).at) * ((a).len - (idx) - (len))), (a).len -= len)

// TODO: Check the arguments? assertions?

#define array_reserve(a, size) _array_reserve(ARRAY_UNPACK(a), size)
#define array_expand(a, size) (_array_reserve(ARRAY_UNPACK(a), (a).len + size), (a).len += size)
#define array_resize(a, size) (_array_reserve(ARRAY_UNPACK(a), size), (a).len = size)
#define array_set(a, i, v)    (array_reserve(a, i + 1), (a).len = ((a).len >= i + 1)? (a).len : i + 1, (a).at[i] = v)
#define array_set_empty(a, i) (array_reserve(a, i + 1), (a).len = ((a).len >= i + 1)? (a).len : i + 1)
#define array_clear(a) (a.len = 0)
#define array_first(a) ((a).at[0])
#define array_last(a) ((a).at[(a).len - 1])

#define array_init(a, allocator_val) (memset(&(a), 0, sizeof(a)), (a).allocator = allocator_val)
#define array_create(allocator) {NULL, 0, 0, (allocator)}
#define array_from(n, a) {a, n, 0, (Allocator)-1}

#define array_free(a) ((a).at ? (a).allocator(FREE, (a).at, 0) : 0)


 void _array_reserve2(void** a, i64* len, i64* cap, Allocator* allocator, i64 item_size, i64 alignment, i64 new_len) {
    assert(alignment > 0);
    i64 old_cap = *cap;
    if (*cap >= new_len) return;
    if (*cap == 0) *cap = 16;
    while(*cap < new_len) *cap *= 2;
    if (!*allocator) *allocator = heap_allocator; // TODO: Remove this hack
    //if (!*allocator)
    //    *a = realloc(*a, (size_t)(*cap * item_size));
    //else
    *a = (*allocator)(REALLOC, *a, *cap * item_size);//realloc(*a, (size_t)(*cap * item_size));
    if (!*a) {
        printf("Out of memory!\n");
        printf("New capacity: %llu\n", *cap    * item_size + 2llu * sizeof(i64));
        printf("Old capacity: %llu\n", old_cap * item_size + 2llu * sizeof(i64));
        fflush(stdout);
        assert(0);
        exit(-1);
    }
}
void _array_reserve(void** a, i64* len, i64* cap, Allocator* allocator, i64 item_size, i64 alignment, i64 new_len) {
    _array_reserve2(a, len, cap, allocator, item_size, alignment, new_len);
    _array_reserve2(a, len, cap, allocator, item_size, alignment, new_len);
}

#define array_heap_insert(a, val, compare) \
do {\
    i64 __idx = a.len;\
    array_expand(a, 1);\
    assert(a.cap >= a.len);\
    assert(a.len == __idx + 1);\
    while(__idx) {\
        i64 __parent_idx = (__idx + 1) / 2 - 1;\
        if (compare(val, a.at[__parent_idx]) >= 0) break;\
        a.at[__idx] = a.at[__parent_idx];\
        __idx       = __parent_idx;\
    }\
    a.at[__idx] = val;\
} while(0)

#define array_heap_fetch(a) (assert((a).len), (a).at[0])
#define array_heap_pop(out, a, compare) \
do {\
    i64 __idx     = 0;\
    i64 __child   = (__idx + 1) * 2 - 1;\
    out         = a.at[__idx];\
    a.at[__idx]   = array_pop(a);\
    a.at[a.len] = out;\
    while (__child < a.len) {\
        if (__child + 1 < a.len && compare(a.at[__child], a.at[__child + 1]) > 0) __child++;\
        if ((int)compare(a.at[__idx], a.at[__child]) <= 0) break;\
        out         = a.at[__child];\
        a.at[__child] = a.at[__idx  ];\
        a.at[__idx  ] = out;\
        __idx         = __child;\
        __child       = (__idx + 1) * 2 - 1;\
    }\
    out = a.at[a.len];\
} while(0)

#define DECL_BINARY_SEARCH_T(T, BINARY_SEARCH_T, TARRAY)\
i64 BINARY_SEARCH_T(TARRAY a, T val) {\
    assert(0 && "This function is untested!");\
    i64 begin = 0;\
    i64 end = a.len - 1;\
    while (begin <= end) {\
        i64 middle = begin + (end - begin / 2);\
        if      (val > a.at[middle]) begin = middle + 1;\
        else if (val < a.at[middle]) end   = middle - 1;\
        else {\
            begin = middle;\
            break;\
        }\
    }\
    return begin;\
}

DECL_BINARY_SEARCH_T(u8   , binary_search_u8   , u8Array  )
DECL_BINARY_SEARCH_T(u16  , binary_search_u16  , u16Array )
DECL_BINARY_SEARCH_T(u32  , binary_search_u32  , u32Array )
DECL_BINARY_SEARCH_T(u64  , binary_search_u64  , u64Array )
DECL_BINARY_SEARCH_T(i8   , binary_search_i8   , i8Array  )
DECL_BINARY_SEARCH_T(i16  , binary_search_i16  , i16Array )
DECL_BINARY_SEARCH_T(i32  , binary_search_i32  , i32Array )
DECL_BINARY_SEARCH_T(i64  , binary_search_i64  , i64Array )
DECL_BINARY_SEARCH_T(uptr , binary_search_uptr , uptrArray)
DECL_BINARY_SEARCH_T(iptr , binary_search_iptr , iptrArray)
//DECL_BINARY_SEARCH_T(void*, binary_search_ptr  , ptrArray )
//DECL_BINARY_SEARCH_T(u8*  , binary_search_cstr , cstrArray)
