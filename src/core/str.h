#pragma once
#include <stdarg.h>
#include <stdbool.h>

#include "core/array.h"
#include "core/alloc.h"

// TODO: Make strings resizeable, unless allocator is NULL like for string literals or string is slice

/*typedef struct StrSlice {
    u8* at;
    u32 len;
} StrSlice;*/

#define STR_SLICE_ALLOCATOR ((Allocator)-1)

typedef struct Str {
#if 0 // #ifndef NDEBUG // Doesn't work!
    union { u8* at; u8* cstr; };
    uptr cap;
    uptr len;
    Allocator allocator;
#else
    u8* at;

    uptr cap;
    uptr len;

    // allocator is NULL for string literals and strings cretaed from c-strings using str()
    // allocator is -1 for slices
    Allocator allocator;
    // cstr is NULL for slices, because slices are not NULL terminated.
    // Use this instead of str.at when you want the c-string.
    const char* cstr;

#endif
} Str;


typedef Array(Str) StrArray;
typedef Array(Str*) StrPtrArray;


INLINE Str str_with_allocator(const char* cstr, Allocator allocator) {
    return (Str){.at = (u8*)cstr, .cstr = cstr, .cap = 0, .len = strlen(cstr), .allocator = allocator};
}

INLINE Str str(const char* cstr) {
    return str_with_allocator(cstr, NULL);
}

// Even empty strings need allocators, they are used for assertions.
INLINE Str str_empty(Allocator allocator) {
    return (Str){.at = "", .cstr = "", .cap = 0, .len = 0, .allocator = allocator};
}

INLINE Str str_empty_slice() {
    return (Str){.at = "", .cstr = "", .cap = 0, .len = 0, .allocator = (void*)-1};
}

INLINE bool str_is_slice(Str a) {
    if (a.allocator == (Allocator)-1) {
        assert(!a.cstr);
        return true;
    }
    assert(a.cstr);
    assert(a.at[a.len] == '\0');
    assert(strlen(a.at) == a.len);
    return false;
}

INLINE Str str_slice(Str str, uptr idx, uptr len) {
    assert(idx + len <= str.len);
    return (Str){.at = str.at + idx, .cstr = NULL, .cap = 0, .len = len, .allocator = (void*)-1};
}

INLINE Str str_slice_cstr(u8* cstr, uptr idx, uptr len) {
    assert(idx + len <= strlen(cstr));
    return (Str){.at = cstr + idx, .cstr = NULL, .cap = 0, .len = len, .allocator = (void*)-1};
}

INLINE bool str_equals(Str a, Str b) {
    if (a.len != b.len)           return false;
    if (a.at == b.at)             return true;
    if (a.len == 0 && b.len == 0) return true;
    printf("%i\n", memcmp(a.at, b.at, a.len));
    return (memcmp(a.at, b.at, a.len) == 0);
}

INLINE bool str_is(Str a, const u8* cstr) {
    return str_equals(a, str(cstr));
}

INLINE Str str_alloc(uptr len, Allocator allocator) {
    if (len == 0) return str_empty(allocator);
    assert(allocator);
    uptr size = len + 1;
    void* ptr = allocator(ALLOC, NULL, size);
    if (!ptr) RUNTIME_ERROR("Out of memory");
    Str str = { .at = ptr, .cstr = ptr, .cap = len, .len = len, .allocator = allocator };
    str.at[len] = '\0';
    return str;
}

INLINE void str_free(Str* str) {
    assert(!str_is_slice(*str));
    assert(str->allocator);
    if (str->len == 0) return;
    assert(str->at != (u8*)-1);
    str->allocator(FREE, str->at, str->len + 1);
#ifndef NDEBUG
    memset(str, -1, sizeof(Str));
#endif
}

#define cstr_dup(s, allocator) str_dup(str(s), allocator)
INLINE Str str_dup(Str str, Allocator allocator) {
    assert(allocator);
    if (str.len == 0) return str_empty(allocator);
    Str str2 = str_alloc(str.len, allocator);
    memcpy(str2.at, str.at, str.len);
    assert(strlen(str2.cstr) == str.len);
    return str2;
}

Str str_concat(Str a, Str b, Allocator allocator) {
    assert(strlen(a.at) >= a.len);
    assert(strlen(b.at) >= b.len);
    Str r = str_alloc(a.len + b.len, allocator);
    if (a.len) memcpy(r.at         , a.at, a.len);
    if (b.len) memcpy(r.at + a.len, b.at, b.len);
    return r;
}

Str str_concat_char(Str a, const char c, Allocator allocator) {
    assert(strlen(a.at) >= a.len);
    Str r = str_alloc(a.len + 1, allocator);
    if (a.len) memcpy(r.at         , a.at, a.len);
    r.at[a.len] = c;
    return r;
}

Str str_print(Allocator allocator, u8* format, ...) {
    va_list args;
    va_start(args, format);
    uptr len =  vsnprintf(NULL, 0, format, args);
    Str str = str_alloc(len, allocator);
    va_start(args, format);
    vsprintf(str.at, format, args);
    va_end(args);
    return str;
}

bool str_starts_with(Str a, Str b) {
    if (a.len < b.len) return false;
    return (memcmp(a.at, b.at, b.len) == 0);
}

bool str_ends_with(Str a, Str b) {
    if (a.len < b.len) return false;
    return (memcmp(a.at + a.len - b.len, b.at, b.len) == 0);
}


/*INLINE void str_resize(Str* str, u32 len) {
    assert(str);
    assert(str->allocator);
    if(!str->at) {
        *str = str_alloc(len, temp_allocator);
        return;
    } else if (len > str->cap) {
        str->cap = len;
        str->at = str->allocator(REALLOC, str->at, len + 1);
        if (!str->at) RUNTIME_ERROR("Out of memory");
        str->cstr = str->at;
        str->len = len;
        str->at[len] = '\0';
    }
}*/













// OLD:
/*

#define str_len_ptr(a)       ((u32*)(a) - 1)
#define str_cap_ptr(a)       ((u32*)(a) - 2)
#define str_scope_idx_ptr(a) ((u32*)(a) - 3)

#define str_len(a) ((a)? *str_len_ptr(a) : 0)
#define str_cap(a) ((a)? *str_cap_ptr(a) : 0)


THREAD_LOCAL u8** tl_sb_scope_refs = NULL;
THREAD_LOCAL u32* tl_sb_scope_idxs = NULL;


void str_scope_push() {
    sb_push(tl_sb_scope_idxs, sb_len(tl_sb_scope_refs));
}

void str_scope_pop() {
    u32 idx = sb_pop(tl_sb_scope_idxs);
    u32 len =  sb_len(tl_sb_scope_refs);
    assert(idx <= len);
    for (u32 i = idx; i < len; i++) {
        u8* str = tl_sb_scope_refs[i];
#ifdef DEBUG
        if (str) {
            assert(*str_scope_idx_ptr(str) == i);
            *str_scope_idx_ptr(str) = -1;
        }
#endif
        str_free(str);
    }
}


u8* str_alloc(u32 len) {
    assert(sb_len(tl_sb_scope_idxs) != 0 && "no string scopes to push string u32o");
    u32 size = len + 1 + sizeof(u32) * 3;
    void* ptr = malloc(size);
    if (!ptr) RUNTIME_ERROR("Out of memory");
    u8* str = (u8*)ptr + sizeof(u32) * 3;

    *str_len_ptr(str) = len;
    *str_cap_ptr(str) = len;
    *str_scope_idx_ptr(str) = sb_len(tl_sb_scope_refs);
    sb_push(tl_sb_scope_refs, str);
    str[len] = '\0';
    return str;
}

void str_free(u8* str) {
    if (!str) return;
#ifdef DEBUG
    assert(*str_scope_idx_ptr(str) == -1 && "str_keep() must be called before str_free()");
#endif
    void* ptr = (u32*)str - 3;
    free(ptr);
}

void str_keep(u8* str) {
    if (!str) return;
    u32 scope_idx = *str_cope_idx_ptr(str);
    assert(scope_idx < sb_len(tl_sb_scope_refs));
    tl_sb_scope_refs[scope_idx] = NULL;
#ifdef DEBUG
    *str_scope_idx_ptr(str) = -1;
#endif
}

u8* str_dup_slice(const u8* cstr, u32 len) {
    if (len == 0) return NULL;
    assert(cstr);
    u8* str = str_alloc(len);
    memcpy(str, cstr, len);
    return str;
}
u8* str_dup_cstr(const u8* cstr) {
    if (cstr == NULL) return NULL;
    return str_from_slice(cstr, strlen(cstr));
}
u8* str_dup(const u8* str) {
    if (str == NULL) return NULL;
    return str_from_slice(str, str_len(str));
}
void str_concat(u8** a, const u8* b) {
    sb_resize(*a, str_len(a) + str_len(b) + 1);
    memcpy((*a) + str_len(a), b, str_len(b) + 1);
}
void str_concat_cstr(u8** a, const u8* b) {
    u32 strlen_b = strlen(b);
    sb_resize(*a, str_len(a) + strlen_b + 1);
    memcpy((*a) + str_len(a), b, strlen_b + 1);
}

void str_resize(u8** str, u32 len) {
    assert(str);
    if(!*str) {
        *str = str_create(len);
    }
    u32 idx = *str_scope_idx_ptr(*str);
    u32 ref_cnt = *str_ref_cnt_ptr(*str);
    assert(ref_cnt == 0 && "String must not yet have owners when resizing");
    if (len > *str_cap_ptr(*str)) {
        void* ptr = *str - 4 * sizeof(u32);
        ptr = realloc(ptr);
        if (!ptr) RUNTIME_ERROR("Out of memory");
        *str = (u8*)ptr + 4 * sizeof(u32);
        if (idx != -1) {
            tl_sb_scope_refs[idx] = *str;
        }
    }
    *str_len_ptr(*str) = len;
    (*str)[len] = '\0';
}

u8* str_print(u8* format, ...) {
    va_list args;
    va_start(args, format);
    u32 len = old_len + vsnprintf(NULL, -1, format, args);
    u8* str = str_alloc(len);
    vsprintf(out->cstr + old_len, format, args);
    va_end(args);
    return str;
}

//#define str_concat(a, b) _str_concat(&a, b)
//#define str_concat_cstr(a, cstr) _str_concat_cstr(&a, cstr)
*/
