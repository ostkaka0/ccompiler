#pragma once

#include "core/thread.h"
#include "core/mem.h"
#include "core/macro.h"

#include <stdlib.h>

// TODO: Alignment!!!
// TODO: Perhaps pass size when freeing, that will make temporary allocator more efficient.
// TODO: Fallback allocator for temporary allocator, malloc can be used and a linked list can be used to free memory when temp_allocator_free_all() is called. The program should probably write a warning once fallback allocator is used, for example when a user is running a server, the server might require more temporary storage than the default settings, then the user can change the max temporary storage in a config-file to improve the performance.
// TODO: Out of memory callback and perhaps a defragment callback to let user code defragment and free uneeded memory.
// TODO: heap_allocator_disable()/enable(). Disabling heap allocations after initialization might be a good idea for some applications.

typedef enum AllocatorMode {
    ALLOC = 1,
    FREE = 2,
    REALLOC = 3,
} AllocatorMode;

typedef void* (*Allocator)(AllocatorMode mode, void* old, uptr size);

/*INLINE Allocator allocator_or(Allocator allocator, Allocator default) {
    return (allocator)? allocator : default;
}*/

INLINE void* heap_allocator(AllocatorMode mode, void* old, uptr size) {
#ifdef _WIN32
    // TODO: Replace 32 with alignment paremeter.
    if (mode == ALLOC) {
        return _aligned_malloc(size, 32);
    } else if (mode == FREE) {
        _aligned_free(old);
    } else if (mode == REALLOC) {
        return _aligned_realloc(old, size, 32);
    } else assert(0);
    return NULL;
#else
    // TODO: Replace 32 with alignment paremeter.
    if (mode == ALLOC) {
        return aligned_alloc(32, size);
    } else if (mode == FREE) {
        free(old);
    } else if (mode == REALLOC) {
        return realloc(old, size);
    } else assert(0);
    return NULL;
#endif
}
INLINE void* heap_alloc   (uptr size           ) { return heap_allocator(ALLOC   , NULL, size); }
INLINE void* heap_free    (void* old           ) { return heap_allocator(FREE    , old , 0   ); }
INLINE void* heap_realloc (void* old, uptr size) { return heap_allocator(REALLOC , old , size); }

// temp_storage is the memory used by stack_allocator and temp_allocator
THREAD_LOCAL u8*  tl_temp_storage              = NULL;
THREAD_LOCAL uptr tl_temp_storage_cap          = 0;
THREAD_LOCAL u8*  tl_temp_allocator_ptr        = NULL;
THREAD_LOCAL u8*  tl_stack_allocator_ptr       = NULL;
THREAD_LOCAL u8*  tl_stack_allocator_scope_ptr = NULL;

//THREAD_LOCAL Allocator tl_mem_context_allocator;
//typedef struct alloctor_context_node {
//
//}
//THREAD_LOCAL struct {}* tl_mem_context_allocator_prev;

//#define push_allocator(a) do {  } while(0)


INLINE void temp_storage_init(uptr cap) {
    if (cap == 0) return;
    assert(tl_temp_storage == NULL);
    tl_temp_storage = malloc(cap);
    tl_temp_storage_cap = cap;
    tl_temp_allocator_ptr = tl_temp_storage;
    tl_stack_allocator_ptr = tl_temp_storage + cap;
    tl_stack_allocator_scope_ptr = NULL;
    if (!tl_temp_storage) RUNTIME_ERROR("Out of memory");
}

INLINE void temp_storage_deinit() {
    free(tl_temp_storage);
}


// Allocates memory that is freed at pop of scope. Do not confuse this with alloca() or "the stack", it's different. Scopes must be explicity declared with stack_allocator_push() + stack_allocator_pop(). Freeing is not required and does nothing. Memory can be allocated in parent scope or even parent functions by omitting stack_allocator_push() + scope_allocator_pop().
INLINE void* stack_allocator(AllocatorMode mode, void* old, uptr size) {
    assert(tl_temp_storage);
    assert(tl_temp_allocator_ptr);
    assert(tl_stack_allocator_ptr);
    size = (size + 7) / 8 * 8;
    assert(mode == ALLOC || mode == FREE || mode == REALLOC);
    if (mode == FREE) {
        // TODO: Set freed memory to -1
        return NULL;
    }
    uptr old_size = -1;
    void* ptr = NULL;
    if (mode == REALLOC && old) {
        old_size = *((uptr*)old - 1);
        if (old_size >= size) return old;
    }
    if (mode == ALLOC || mode == REALLOC) {
        if ((iptr)(tl_stack_allocator_ptr - tl_temp_allocator_ptr - size - sizeof(uptr)) < 0) {
            RUNTIME_ERROR("Out of memory for temporary storage");
        }
        tl_stack_allocator_ptr -= size + sizeof(uptr);
        ptr = tl_stack_allocator_ptr;
        *(uptr*)ptr = size;
        ptr = (uptr*)ptr + 1;
    }
    if (mode == REALLOC && old) {
        memcpy(ptr, old, old_size);
    }
    return ptr;
}
INLINE void* stack_alloc   (uptr size           ) { return stack_allocator(ALLOC   , NULL, size); }
INLINE void* stack_realloc (void* old, uptr size) { return stack_allocator(REALLOC , old , size); }

INLINE void stack_allocator_push() {
    assert(tl_temp_storage);
    assert(tl_temp_allocator_ptr);
    assert(tl_stack_allocator_ptr);
    u8** old_ptr = stack_alloc(sizeof(u8*));
    *old_ptr = tl_stack_allocator_scope_ptr;
    tl_stack_allocator_scope_ptr = tl_stack_allocator_ptr;
}

INLINE void stack_allocator_pop() {
    u8* old_stack_allocator_ptr = tl_stack_allocator_ptr;
    
    tl_stack_allocator_ptr = tl_stack_allocator_scope_ptr;
    assert(*(uptr*)tl_stack_allocator_ptr == sizeof(uptr));
    tl_stack_allocator_ptr += sizeof(u8*);
    u8* old_ptr = *(u8**)tl_stack_allocator_ptr;
    tl_stack_allocator_ptr += sizeof(u8*);
    tl_stack_allocator_scope_ptr = old_ptr;
    
    memset(old_stack_allocator_ptr, -1, (uptr)(tl_stack_allocator_ptr - old_stack_allocator_ptr));
}

INLINE void temp_allocator_free_all() {
    tl_temp_allocator_ptr = tl_temp_storage;
}

// Allocates temporary memory that is freed once temp_allocator_free_all() is called, usually at the end of a main loop like at the end of every frame. Freeing is not required and does nothing. Shares memory(temp_storage) with stack_allocator.
INLINE void* temp_allocator(AllocatorMode mode, void* old, uptr size) {
    assert(tl_temp_storage);
    assert(tl_temp_allocator_ptr);
    assert(tl_stack_allocator_ptr);
    size = (size + 7) / 8 * 8;
    assert(mode == ALLOC || mode == FREE || mode == REALLOC);
    size = (size + sizeof(uptr) - 1) / sizeof(uptr) * sizeof(uptr);
    uptr old_size = -1;
    void* ptr = NULL;
    if (mode == REALLOC && old) {
        old_size = *((uptr*)old - 1);
        if (old_size >= size) return old;
    }
    if (mode == ALLOC || mode == REALLOC) {
        if ((iptr)(tl_stack_allocator_ptr - tl_temp_allocator_ptr - size - sizeof(uptr)) < 0) { //if (tl_temp_allocator_idx + size + sizeof(uptr) > tl_temp_allocator_cap)
            RUNTIME_ERROR("Out of memory for temporary storage");
        }
        ptr = tl_temp_allocator_ptr;
        *(uptr*)ptr = size;
        ptr = (uptr*)ptr + 1;
        tl_temp_allocator_ptr += size + sizeof(uptr);
    }
    if (mode == REALLOC && old) {
        memcpy(ptr, old, old_size);
    }
    return ptr;
}
INLINE void* temp_alloc   (uptr size           ) { return temp_allocator(ALLOC   , NULL, size); }
INLINE void* temp_realloc (void* old, uptr size) { return temp_allocator(REALLOC , old , size); }



// Experimented with overcommit for temporary storage, but I don't think this is a good solution so it's commented out.
/*
#define TEMP_STORAGE_CAPACITY (16 * GB) // Why would you need more than this?
THREAD_LOCAL u8* tl_temp_allocator_base  = NULL;
THREAD_LOCAL u8* tl_temp_allocator_ptr   = NULL;
THREAD_LOCAL u8* tl_stack_allocator_base = NULL;
THREAD_LOCAL u8* tl_stack_allocator_ptr  = NULL;
THREAD_LOCAL u8*  tl_stack_allocator_scope_ptr = NULL;

INLINE void temp_storage_init() {
    assert(tl_temp_allocator_base == NULL);
    tl_temp_allocator_base = mem_alloc(TEMP_STORAGE_CAPACITY);
    tl_stack_allocator_base = mem_alloc(TEMP_STORAGE_CAPACITY);
    tl_temp_allocator_ptr = tl_temp_allocator_base;
    tl_stack_allocator_ptr = tl_stack_allocator_base;
    if (!tl_temp_allocator_base) RUNTIME_ERROR("Out of memory");
    if (!tl_stack_allocator_base) RUNTIME_ERROR("Out of memory");
}

INLINE void temp_storage_deinit() {
    mem_free(tl_temp_allocator_base, TEMP_STORAGE_CAPACITY);
    mem_free(tl_stack_allocator_base, TEMP_STORAGE_CAPACITY);
}

INLINE void* stack_allocator(AllocatorMode mode, void* old, uptr size) {
    assert(tl_stack_allocator_base);
    assert(tl_temp_allocator_base);
    assert(tl_temp_allocator_ptr);
    assert(tl_stack_allocator_ptr);
    size = (size + 7) / 8 * 8;
    assert(mode == ALLOC || mode == FREE || mode == REALLOC);
    if (mode == FREE) {
        // TODO: Set freed memory to -1
        return NULL;
    }
    uptr old_size = -1;
    void* ptr = NULL;
    if (mode == REALLOC && old) {
        uptr* old_size_ptr = (uptr*)old - 1;
        old_size = *(old_size_ptr);
        if (old_size >= size) return old;
        if (old_size <  size && old + old_size == tl_stack_allocator_ptr) {
            tl_stack_allocator_ptr += size - old_size;
            *old_size_ptr = size;
            return old;
        }
    }
    if (mode == ALLOC || mode == REALLOC) {
        //u8* aligned_location = (tl_stack_allocator_ptr + sizeof(uptr)) / alignemnt * alignement;
        //uptr alignment_bytes = tl_stack_allocator_ptr + sizeof(uptr) - aligned_location;
        if ((iptr)(tl_stack_allocator_ptr - tl_stack_allocator_base + size + sizeof(uptr)) >= TEMP_STORAGE_CAPACITY) {
            RUNTIME_ERROR("Out of memory for temporary storage");
        }
        ptr = tl_stack_allocator_ptr;
        *(uptr*)ptr = size;
        ptr = (uptr*)ptr + 1;
        tl_stack_allocator_ptr += size + sizeof(uptr);
    }
    if (mode == REALLOC && old) {
        memcpy(ptr, old, old_size);
    }
    return ptr;
}
INLINE void* stack_alloc   (uptr size           ) { return stack_allocator(ALLOC   , NULL, size); }
INLINE void* stack_realloc (void* old, uptr size) { return stack_allocator(REALLOC , old , size); }

INLINE void stack_allocator_push() {
    assert(tl_stack_allocator_base);
    assert(tl_temp_allocator_base);
    assert(tl_temp_allocator_ptr);
    assert(tl_stack_allocator_ptr);
    u8** old_ptr = stack_alloc(sizeof(u8*));
    *old_ptr = tl_stack_allocator_scope_ptr;
    tl_stack_allocator_scope_ptr = tl_stack_allocator_ptr;
}

INLINE void stack_allocator_pop() {
    u8* old_stack_allocator_ptr = tl_stack_allocator_ptr;

    tl_stack_allocator_ptr = tl_stack_allocator_scope_ptr;
    assert(*((uptr*)tl_stack_allocator_ptr) == sizeof(uptr));
    tl_stack_allocator_ptr += sizeof(u8*);
    u8* old_ptr = *(u8**)tl_stack_allocator_ptr;
    tl_stack_allocator_ptr += sizeof(u8*);
    tl_stack_allocator_scope_ptr = old_ptr;

#if _NDEBUG
    memset(old_stack_allocator_ptr, -1, (uptr)(tl_stack_allocator_ptr - old_stack_allocator_ptr));
#endif

    uptr page_size = mem_get_page_size();
    u8* begin = tl_stack_allocator_base +(tl_stack_allocator_ptr - tl_stack_allocator_base + page_size - 1) / page_size * page_size;
    u8* end = tl_stack_allocator_base + TEMP_STORAGE_CAPACITY;
    mem_advice_free(begin, end - begin);
}

INLINE void temp_allocator_free_all() {
    u8* old_temp_allocator_ptr = tl_temp_allocator_ptr;

    tl_temp_allocator_ptr = tl_temp_allocator_base;

#if _NDEBUG
    memset(old_stack_allocator_ptr, -1, (uptr)(tl_stack_allocator_ptr - old_stack_allocator_ptr));
#endif

    mem_advice_free(tl_temp_allocator_ptr, TEMP_STORAGE_CAPACITY);
    //uptr page_size = mem_get_page_size();
    //uptr begin = (tl_temp_allocator_ptr + page_size - 1) / page_size * page_size;
    //uptr end = tl_temp_allocator_base + TEMP_STORAGE_CAPACITY;
    //mem_advice_free(begin, end - begin);
}


// Allocates temporary memory that is freed once temp_allocator_free_all() is called, usually at the end of a main loop like at the end of every frame. Freeing is not required and does nothing. Shares memory(temp_storage) with stack_allocator.
INLINE void* temp_allocator(AllocatorMode mode, void* old, uptr size) {
    assert(tl_stack_allocator_base);
    assert(tl_temp_allocator_base);
    assert(tl_temp_allocator_ptr);
    assert(tl_stack_allocator_ptr);
    size = (size + 7) / 8 * 8;
    assert(mode == ALLOC || mode == FREE || mode == REALLOC);
    size = (size + sizeof(uptr) - 1) / sizeof(uptr) * sizeof(uptr);
    uptr old_size = -1;
    void* ptr = NULL;
    if (mode == REALLOC && old) {
        uptr* old_size_ptr = (uptr*)old - 1;
        old_size = *(old_size_ptr);
        if (old_size >= size) return old;
        if (old_size <  size && old + old_size == tl_stack_allocator_ptr) {
            tl_stack_allocator_ptr += size - old_size;
            *old_size_ptr = size;
            return old;
        }
    }
    if (mode == ALLOC || mode == REALLOC) {
        if ((iptr)(tl_stack_allocator_ptr - tl_stack_allocator_base + size + sizeof(uptr)) >= TEMP_STORAGE_CAPACITY) { //if (tl_temp_allocator_idx + size + sizeof(uptr) > tl_temp_allocator_cap)
            RUNTIME_ERROR("Out of memory for temporary storage");
        }
        ptr = tl_temp_allocator_ptr;
        *(uptr*)ptr = size;
        ptr = (uptr*)ptr + 1;
        tl_temp_allocator_ptr += size + sizeof(uptr);
    }
    if (mode == REALLOC && old) {
        memcpy(ptr, old, old_size);
    }
    return ptr;
}
INLINE void* temp_alloc   (uptr size           ) { return temp_allocator(ALLOC   , NULL, size); }
INLINE void* temp_realloc (void* old, uptr size) { return temp_allocator(REALLOC , old , size); }
*/
