#pragma once
#include "core/types.h"
#include "core/str.h"

// TODO: Remove map_has_idx and map.loop, map.keys should contain no holes, that way looping will be simpler. Create a special IdMap for when indices can't change.
// TODO: Do something about the type-bloat. Perhaps a macro to generate map for a new type is the best solution after all.

////////////////////////////////////////////////////////////////
// Simple api:
////////////////////////////////////////////////////////////////
// map.len
// map.keys[i]
// void T_map_deinit(map)
// bool T_map_has_idx(map, idx)
// bool T_map_has(map, key)
// u32  T_map_find(map, key)
// u32  T_map_insert(map, key)
// u32  T_map_erase(map, key)
////////////////////////////////////////////////////////////////
// Complex api:
////////////////////////////////////////////////////////////////
// u32  map_alloc_idx(map);
// void map_free_idx(map, idx)
// u32* T_map_find_bucket(map, key)
// void T_map_set_key(map, idx, hash, key)
// void map_erase_bucket(map, hash, bucket)
////////////////////////////////////////////////////////////////
// Examples:
////////////////////////////////////////////////////////////////
// // Init, insert, erase and deinit
// u32Map map = {0};
// u32 err;
// err = u32_map_insert(&map, 123);
// assert(!err); // Check if key already was inserted
// err = u32_map_erase(&map, 123);
// assert(!err); // Check if key wasn't inserted
// map_deinit(&map);
//
// // Find
// u32 idx = map_find(&map, 123);
// if (idx != -1) printf("Index of key 123 is %i\n", idx);
//
// // Loop
// MAP_FOR(&map, i) {
//     i32 key = map.keys[i];
//     ...
// }
// // Explicit loop
// for (int i = 0; i < map.loop; i++) {
//     if (!i32_map_has_idx(&map, i)) continue;
//     i32 key = map.keys[i];
//     ...
// }
////////////////////////////////////////////////////////////////
// Complex examples: (OUTDATED)
////////////////////////////////////////////////////////////////
// // Insert
// u64 hash = hash_T(key);
// u32 idx = T_map_alloc_idx(map);
// u32* bucket = T_map_find_empty_bucket(map, key);
// assert(*bucket == -1); // Same key should not occur twice'
// *bucket = idx;
// T_map_set_key(map, idx, hash, key);
//
// // Erase
// u64 hash = hash_T(key);
// u32* bucket = T_map_find_bucket(map, key);
// assert(bucket); // key not found
// u32 idx = *bucket;
// T_map_erase_bucket(map, hash, bucket);
// T_map_free_idx(map, idx);
//
// // Change key
// u64 hash = hash_T(key);
// u32* bucket = T_map_find_bucket(map, key);
// assert(bucket); // key not found
// u32 idx = *bucket;
// T_map_erase_bucket(map, hash, bucket);
// u64 new_hash = hash_T(new_key);
// u32* new_bucket = T_map_find_empty_bucket(map, new_hash, new_key);
// T_map_set_key(map, idx, new_hash, new_key);
// u32 err = T_map_insert_bucket(map, idx, new_bucket);
// assert(!err); // The new key should not occur twice in the map
//
////////////////////////////////////////////////////////////////

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

#include "core/types.h"
#include "core/macro.h"
#include "core/sb.h"

#define map_hashes(m)      ((u64*)(m)->data)
#define map_buckets(m)     ((u32*)(m)->data + (m)->cap)
#define map_buckets_old(m, old_cap) ((u32*)(m)->data + old_cap)



//#define map_find_bucket(map, key)              map_find_bucket_(MAP_UNPACK(map), hash_array((void*)&(key), sizeof(key)), &(key))
//#define map_find(       map, key)              map_find_       (MAP_UNPACK(map), hash_array((void*)&(key), sizeof(key)), &(key))
//#define map_has(        map, key)              map_has_        (MAP_UNPACK(map), hash_array((void*)&(key), sizeof(key)), &(key))
//#define map_insert_at(  map, key, idx, bucket) map_insert_at_  (MAP_UNPACK(map), hash_array((void*)&(key), sizeof(key)), &(key), idx, bucket)
//#define map_insert(     map, key)              map_insert_     (MAP_UNPACK(map), hash_array((void*)&(key), sizeof(key)), &(key))
//#define map_erase(      map, key)              map_erase_      (MAP_UNPACK(map), hash_array((void*)&(key), sizeof(key)), &(key))


#define map_find_bucket_with_hash(map, key, hash)              map_find_bucket_(NONE_MAP_UNPACK(map), hash, &(key))
#define map_find_with_hash(       map, key, hash)              map_find_       (NONE_MAP_UNPACK(map), hash, &(key))
#define map_has_with_hash(        map, key, hash)              map_has_        (NONE_MAP_UNPACK(map), hash, &(key))
#define map_insert_at_with_hash(  map, key, hash, idx, bucket) map_insert_at_  (NONE_MAP_UNPACK(map), hash, &(key), idx, bucket)
#define map_insert_with_hash(     map, key, hash)              map_insert_     (NONE_MAP_UNPACK(map), hash, &(key))
#define map_erase_with_hash(      map, key, hash)              map_erase_      (NONE_MAP_UNPACK(map), hash, &(key))


//#define str_map_find_bucket(map, key)              map_find_bucket_(STR_MAP_UNPACK(map), hash_str(key), &(key))
//#define str_map_find(       map, key)              map_find_       (STR_MAP_UNPACK(map), hash_str(key), &(key))
//#define str_map_insert_at(  map, key, idx, bucket) map_insert_at_  (STR_MAP_UNPACK(map), hash_str(key), &(key), idx, bucket)
//#define str_map_has(        map, key)              map_has_        (STR_MAP_UNPACK(map), hash_str(key), &(key))
//#define str_map_insert(     map, key)              map_insert_     (STR_MAP_UNPACK(map), hash_str(key), &(key))
//#define str_map_erase(      map, key)              map_erase_      (STR_MAP_UNPACK(map), hash_str(key), &(key))



//#define map_change_key( map, keys, idx , new_hash, key)    map_change_key_ (map , (void**)keys, idx, new_hash, &(key), sizeof(key))
#define map_has_idx(map, idx) _map_has_idx((Map*)(map), idx)
#define map_next(map, idx) idx = _map_next((Map*)map, idx)
#define map_first(map) _map_first((Map*)map)
#define MAP_FOR(map, idx) for (u32 idx = map_first(map); idx < (map)->loop; map_next(map, idx))

#define map_init(map, allocator_val) (memset((map), 0, sizeof(*(map))), (map)->allocator = allocator_val)

// TODO: Test and compare experimental find-functions. Is performance good when using callbacks?
// TODO: TOFIX: Some code assumes each hash is unique, but hash collisions are possible. This cause rare bugs. Replace map_find_bucket_with_hash(...) with safer variants.
// TODO: map_change_key()
// TODO: Free strings


enum {
    MAP_HASH_TYPE_DEFAULT = 0,
    MAP_HASH_TYPE_STR,
    MAP_HASH_TYPE_VOID,
    //MAP_HASH_TYPE_CSTR,
    //MAP_HASH_TYPE_ARRAY,
};

typedef struct {
    void* data;
    u32   cap;
    u32   len;
    u32   loop;
    u32   free_head;
    Allocator allocator;
} Map;


#define MAP_WITH_HASH_TYPE(KEY, HASH_TYPE) struct { \
    union { \
        Map base; \
        struct { \
            void* data; \
            u32 cap; \
            u32 len; \
            u32 loop; \
            u32 free_head; \
            Allocator allocator; \
        }; \
        void* HASH_TYPE; \
    }; \
    KEY* keys; \
}

#define MAP(KEY) MAP_WITH_HASH_TYPE(KEY, _hash_type_default)

// TODO: Delete voidmap, or perhaps don't make it allocate keys






#ifndef MAP3_HASH
#define MAP3_HASH(key, key_stride) hash_array(key, key_stride)
#endif


#define MAP_ARGS Map* map, void** keys, u32 key_stride, u32 key_size, u32 key_alignment, void* trait
#define MAP_UNPACK_WITH_HASH_TYPE(map, HASH_TYPE) &(map)->base, (void**)&(map)->keys, MAX(sizeof(u32), sizeof(*(map)->keys)), sizeof(*(map)->keys), alignof(*(map)->keys), (void*)(map)->HASH_TYPE
#define MAP_UNPACK(map) MAP_UNPACK_WITH_HASH_TYPE(map, _hash_type_default)
#define STR_MAP_UNPACK(map) MAP_UNPACK_WITH_HASH_TYPE(map, _hash_type_str)
#define VOID_MAP_UNPACK(map) MAP_UNPACK_WITH_HASH_TYPE(map, _hash_type_void)
// This is a hack, turns pointer into a negative value which is invalid. Negative because we don't want't to use NULL.
#define NONE_MAP_UNPACK(map) MAP_UNPACK_WITH_HASH_TYPE(map, data - (uptr)(map)->data)
#define MAP_PASS_ARGS map, keys, key_stride, key_size, key_alignment, trait



//typedef i32Map u32Map;
//typedef i64Map u64Map;


#define MAP_DECL_IMPL_EXT(T, t, TMap, t_map) \
    u32* t_map ##_find_bucket(TMap* map, T key)              { return map_find_bucket_(MAP_UNPACK(map), hash_array((void*)&key, sizeof(T)), &key); }\
    u32  t_map ##_find       (TMap* map, T key)              { return map_find_       (MAP_UNPACK(map), hash_array((void*)&key, sizeof(T)), &key); }\
    bool t_map ##_has        (TMap* map, T key)              { return map_has_        (MAP_UNPACK(map), hash_array((void*)&key, sizeof(T)), &key); }\
    u32  t_map ##_insert     (TMap* map, T key)              { return map_insert_     (MAP_UNPACK(map), hash_array((void*)&key, sizeof(T)), &key); }\
    u32  t_map ##_erase      (TMap* map, T key)              { return map_erase_      (MAP_UNPACK(map), hash_array((void*)&key, sizeof(T)), &key); }\
    bool t_map ##_has_idx    (TMap* map, u32 idx)            { return _map_has_idx    (&map->base, idx); }

#define MAP_DECL_IMPL(T, t) MAP_DECL_IMPL_EXT(T, t, T ## Map, t ## _map)



u32* map_find_bucket_(MAP_ARGS, u64 hash, void* key);
u32  map_find_       (MAP_ARGS, u64 hash, void* key);
bool map_has_        (MAP_ARGS, u64 hash, void* key);
//void _map_insert_at_  (MAP_ARGS, void* key, u32 idx, u32* bucket);
u32  map_insert_     (MAP_ARGS, u64 hash, void* key);
u32  map_erase_      (MAP_ARGS, u64 hash, void* key);
bool _map_has_idx    (Map* map, u32 idx);
u32 _map_first       (Map* map);
u32 _map_next        (Map* map, u32 idx);


#define DECL_MAP_EXT(T, t, TMap, t_map) \
    typedef MAP(T) TMap; \
    MAP_DECL_IMPL_EXT(T, t, TMap, t_map)

#define DECL_MAP(T, t) DECL_MAP_EXT(T, t, T ## Map, t ## _map)

DECL_MAP(u8 , u8 )
DECL_MAP(u16, u16)
DECL_MAP(u32, u32)
DECL_MAP(u64, u64)
DECL_MAP(i8 , i8 )
DECL_MAP(i16, i16)
DECL_MAP(i32, i32)
DECL_MAP(i64, i64)
DECL_MAP(f32, f32)
DECL_MAP(f64, f64)
DECL_MAP(int, int)
DECL_MAP(uptr, uptr)
DECL_MAP(iptr, iptr)

DECL_MAP_EXT(void*, ptr, PtrMap, ptr_map)
typedef MAP_WITH_HASH_TYPE(void, _hash_type_void) VoidMap;
typedef MAP_WITH_HASH_TYPE(Str, _hash_type_str) StrMap;





#define map_deinit(m) map_deinit_(MAP_UNPACK(m))
#define void_map_deinit(m) map_deinit_(VOID_MAP_UNPACK(m))
void map_deinit_(MAP_ARGS) {
    if (!map->data) return;
    map->allocator(FREE, map->data, 0);//free(map->data);
    map->allocator(FREE, *keys, 0);//free(map->data);
}
/*void str_map_deinit(StrMap* map) {
    map_deinit(map);
    sb_free(map->keys);
}*/



#define map_clear(m) map_clear_(MAP_UNPACK(m))
#define str_map_clear_and_leak(m) map_clear_(STR_MAP_UNPACK(m))
void map_clear_(MAP_ARGS);


bool str_map_has_idx(StrMap* map, u32 idx);


void str_map_clear(StrMap* map) {
    for (int i = 0; map->loop; i++) {
        if (!str_map_has_idx(map, i)) continue;
        str_free(&map->keys[i]);
    }
    str_map_clear_and_leak(map);
}

void str_map_deinit(StrMap* map) {
    str_map_clear(map);
    map_deinit_(STR_MAP_UNPACK(map));
}

void str_map_deinit_but_leak_strs(StrMap* map) {
    str_map_clear(map);
    map_deinit_(STR_MAP_UNPACK(map));
}

u32* map_find_empty_bucket(Map* map, u64 hash) {
    assert(map->data != NULL);
    u32 bucket_idx = (u32)hash % map->cap;
    u32* bucket = &map_buckets(map)[bucket_idx];
    while (true) {
        if (*bucket == -1)
            return bucket;
        bucket_idx = (bucket_idx + 1) % map->cap;
        bucket = &map_buckets(map)[bucket_idx];
    }
}

u32* map_find_bucket_with_idx(Map* map, u64 hash, u32 idx) {
    assert(map->data != NULL);
    assert(idx != -1);
    u32 bucket_idx = (u32)hash % map->cap;
    u32* bucket = &map_buckets(map)[bucket_idx];
    while (true) {
        if (*bucket == -1)
            return bucket;
        if (*bucket == idx)
            return bucket;
        
        bucket_idx = (bucket_idx + 1) % map->cap;
        bucket = &map_buckets(map)[bucket_idx];
    }
}

#define str_map_erase_bucket(m, bucket) map_erase_bucket_(STR_MAP_UNPACK(m), bucket)
#define map_erase_bucket(m, bucket) map_erase_bucket_(MAP_UNPACK(m), bucket)
void map_erase_bucket_(MAP_ARGS, u32* bucket) {
    *bucket = -1;
    int next_bucket_idx = (u32)(bucket - (map_buckets(map)) + 1) % map->cap;
    if (map_buckets(map)[next_bucket_idx] == -1)
        return;
    int idx = map_buckets(map)[next_bucket_idx];
    u64 hash = map_hashes(map)[idx];
    if (hash % map->cap == next_bucket_idx)
        return;
    map_erase_bucket_(MAP_PASS_ARGS, &map_buckets(map)[next_bucket_idx]);
    u32* newBucket = map_find_empty_bucket(map, hash);
    assert(*newBucket == -1);
    *newBucket = idx;
}

bool map_is_prime(u32 i) {
    for (u32 j = 2; j * j < i; j++) {
        if (i % j == 0) return false;
    }
    return true;
}

// Size of bucket array is a prime number to reduce hash collisions
// TODO: Perhaps finding previous prime number is better. With a size slightly more than power of two the allocator might allocate a page just for the last few bytes at the end of the array which is wasteful.
u32 map_find_next_prime(u32 i) {
    return (map_is_prime(i))? i : map_find_next_prime(i + 1);
}

#define str_map_expand(m) map_expand_(STR_MAP_UNPACK(m))
#define map_expand(m) map_expand_(MAP_UNPACK(m))
void map_expand_(MAP_ARGS) {


    if (map->free_head != -1 && map->data) return;
    assert(map->loop == map->len);
    if (2 * map->len + 2 <= map->cap && map->data) return;
    if (!map->allocator) map->allocator = heap_allocator; // TODO: Remove this hack
    
    Map new_map_;
    Map* new_map = &new_map_;
    new_map->cap = (map->cap == 0) ? 17 : map_find_next_prime(map->cap * 2);
    new_map->len = map->len;
    new_map->loop = map->loop;
    new_map->free_head = -1;
    new_map->allocator = map->allocator;
    new_map->data = map->allocator(ALLOC, NULL, (uptr)new_map->cap * 8); //malloc((uptr)new_map->cap * 8);
    void* new_keys = (key_size == 0)? NULL : map->allocator(REALLOC, *keys, (uptr)new_map->cap * key_stride * 2); // * 2 because cap is weird.
    if (!new_map->data       ) RUNTIME_ERROR("Out of memory.");
    if (!new_keys && key_size) RUNTIME_ERROR("Out of memory.");
    if (map->cap) {
        assert(map->data);
        memcpy(map_hashes(new_map), map_hashes(map), (uptr)map->cap * 4); // TODO: use map->loop(used_cap) instead of map->cap
    }
    memset(map_buckets(new_map), -1, (uptr)new_map->cap * 4);
    
    for (int idx = 0; idx < map->loop; ++idx) {
        assert(map->data);
        u64 hash = map_hashes(map)[idx];
        u32* old_bucket = map_find_bucket_with_idx(map, hash, idx);
        if (*old_bucket != idx) continue;
        u32* new_bucket = map_find_empty_bucket(new_map, hash);
        *new_bucket = idx;
    }
    map->allocator(FREE, map->data, 0);//free(map->data);
    *map = *new_map;
    *keys = new_keys;
    return;
}

#define str_map_alloc_idx(m) map_alloc_idx_(STR_MAP_UNPACK(m))
#define map_alloc_idx(m) map_alloc_idx_(MAP_UNPACK(m))
u32 map_alloc_idx_(MAP_ARGS) {
    map_expand_(MAP_PASS_ARGS);
    if (map->free_head == -1) {
        assert(map->loop == map->len);
        u32 idx = map->len;
        map->loop++;
        map->len++;
        assert(map->loop == map->len);
        return idx;
    } else {
        u32 idx = map->free_head;
        map->free_head = map_hashes(map)[map->free_head];
        map->len++;
        return idx;
    }
}

#define str_map_free_idx(m, idx) map_free_idx_(STR_MAP_UNPACK(m), idx)
#define map_free_idx(m, idx) map_free_idx_(MAP_UNPACK(m), idx)
void map_free_idx_(MAP_ARGS, u32 idx) {
    map_hashes(map)[idx] = map->free_head;
    map->free_head = idx;
    map->len--;
}

// Functions simplifying usage:
// Generic:
//#define str_map_find_bucket(m) map_find_bucket_(MAP_UNPACK(m))
//#define map_find_bucket(m) map_find_bucket_(MAP_UNPACK(m))
u32* map_find_bucket_(MAP_ARGS, u64 hash, void* key) {
    assert(map->data);
    u32 bucket_idx = (u32)hash % map->cap;
    u32* bucket = &map_buckets(map)[bucket_idx];
    while (true) {
        if (*bucket == -1)
            return bucket;
        if (map_hashes(map)[*bucket] == hash && memcmp((u8*)*keys + *bucket * key_stride, key, key_size) == 0)
            return bucket;
        
        bucket_idx = (bucket_idx + 1) % map->cap;
        bucket = &map_buckets(map)[bucket_idx];
    }
}

u32 map_find_(MAP_ARGS, u64 hash, void* key) {
    if (!map->data) return -1;
    return *map_find_bucket_(MAP_PASS_ARGS, hash, key);
}


bool map_has_(MAP_ARGS, u64 hash, void* key) {
    return (map_find_(MAP_PASS_ARGS, hash, key) != -1);
}


u32 map_insert_(MAP_ARGS, u64 hash, void* key) {
    map_expand_(MAP_PASS_ARGS);

    
    u32* bucket = map_find_bucket_(MAP_PASS_ARGS, hash, key);
    if (*bucket != -1) return -1;
    u32 idx = map_alloc_idx_(MAP_PASS_ARGS);
    *bucket = idx;
    map_hashes(map)[idx] = hash;
    void* keys2 = *keys;
    u8* keys3 = keys2;
    u8* keys4 = keys3 + key_stride * idx;
    memcpy(keys4, key, key_size);
    return idx;
}

u32 map_erase_(MAP_ARGS, u64 hash, void* key) {
    if (!map->data) return -1;
    u32* bucket = map_find_bucket_(MAP_PASS_ARGS, hash, key);
    if (*bucket == -1) return -1;
    map_free_idx_(MAP_PASS_ARGS, *bucket);
    map_erase_bucket_(MAP_PASS_ARGS, bucket);
#ifdef _NDEBUG
    memset((u8*)*keys + *bucket * size, -1, size);
#endif
    return 0;
}

#define map_set_key(m, idx, hash, key) map_set_key_(MAP_UNPACK(m), idx, hash, &key)
void map_set_key_(MAP_ARGS, u32 idx, u64 hash, void* key) {
    memcpy((u8*)*keys + idx * key_stride, key, key_size);
    map_hashes(map)[idx] = hash;
}



u32* str_map_find_bucket(StrMap* map, Str key) {
    assert(map->data);
    u64 hash = hash_str(key);
    u32 bucket_idx = (u32)hash % map->cap;
    u32* bucket = &map_buckets(map)[bucket_idx];
    while (true) {
        if (*bucket == -1)
            return bucket;
        if (map_hashes(map)[*bucket] == hash && str_equals(map->keys[*bucket], key))
            return bucket;
        
        bucket_idx = (bucket_idx + 1) % map->cap;
        bucket = &map_buckets(map)[bucket_idx];
    }
}

u32 str_map_find(StrMap* map, Str key) {
    if (!map->data) return -1;
    return *str_map_find_bucket(map, key);
}

bool str_map_has(StrMap* map, Str key) {
    return (str_map_find(map, key) != -1);
}


u32 str_map_insert(StrMap* map, Str key) {
    u64 hash = hash_str(key);
    str_map_expand(map);
    u32* bucket = str_map_find_bucket(map, key);
    if (*bucket != -1) return -1;
    u32 idx = str_map_alloc_idx(map);
    *bucket = idx;
    map_hashes(map)[idx] = hash;
    map->keys[idx] = key;
    return idx;
}

u32 str_map_erase(StrMap* map, Str key) {
    u64 hash = hash_str(key);
    u32* bucket = str_map_find_bucket(map, key);
    if (*bucket == -1) return -1;
    str_free(&map->keys[*bucket]);
    
    str_map_free_idx(map, *bucket);
    str_map_erase_bucket(map, bucket);
#ifdef _NDEBUG
    map->keys[*bucket] = NULL;
#endif
    return 0;
}

u32* cstr_map_find_bucket(StrMap* map, u64 hash, const char* key) {
    Str str_key = str(key);
    return str_map_find_bucket(map, str_key);
}

u32 cstr_map_find(StrMap* map, const char* key) {
    Str str_key = str(key);
    return str_map_find(map, str_key);
}

bool cstr_map_has(StrMap* map, const char* key) {
    Str str_key = str(key);
    return (str_map_find(map, str_key) != -1);
}

u32 cstr_map_dup_and_insert(StrMap* map, const char* key, Allocator allocator) {
    Str str_key = str_dup(str(key), allocator);
    return str_map_insert(map, str_key);
}

u32 cstr_map_erase(StrMap* map, const char* key) {
    Str str_key = str(key);
    return str_map_erase(map, str_key);
}

bool _map_has_idx(Map* map, u32 idx) {
    assert(idx < map->loop);
    return (*map_find_bucket_with_idx(map, map_hashes(map)[idx], idx) == idx);
}

u32 _map_first(Map* map) {
    u32 idx = 0;
    while(idx < map->loop && !map_has_idx(map, idx)) {
        idx++;
    }
    return idx;
}

u32 _map_next(Map* map, u32 idx) {
    do {
        idx++;
    } while(idx < (map)->loop && !map_has_idx(map, idx));
    return idx;
}

bool str_map_has_idx(StrMap* map, u32 idx) {
    return map_has_idx(map, idx);
}

// Deprecated
u32* map_find_bucket_with_only_hash(Map* map, u64 hash) {
    static bool _b = false;
    if (!_b) printf("TODO: Don't use map_find_bucket_with_only_hash()!\n");
    _b = true;
    // assert(0); // TODO: Replace this unsafe function with a type specific one or one with memcmp of key
    assert(map->data != NULL);
    u32 bucket_idx = (u32)hash % map->cap;
    u32* bucket = &map_buckets(map)[bucket_idx];
    while (true) {
        if (*bucket == -1)
            return bucket;
        if (map_hashes(map)[*bucket] == hash)
            return bucket;
        
        bucket_idx = (bucket_idx + 1) % map->cap;
        bucket = &map_buckets(map)[bucket_idx];
    }
}

u32 map_find_hash(Map* map, u64 hash) {
    static bool _b = false;
    if (!_b) printf("TODO: Don't use map_find_hash()!\n");
    _b = true;
    if (!map->data) return -1;
    return *map_find_bucket_with_only_hash(map, hash);
}

bool map_has_hash(Map* map, u64 hash) {
    static bool _b = false;
    if (!_b) printf("TODO: Don't use map_has_hash()!\n");
    _b = true;
    return (map_find_hash(map, hash) != -1);
}

void map_clear_(MAP_ARGS) {
    static bool _b = false;
    if (!_b) printf("TODO: Don't use map_clear()!\n");
    _b = true;
    // TODO: Optimize clear when the number of elements is low.
    memset(map_buckets(map), -1, (uptr)map->cap * sizeof(u32));
    map->len = 0;
    map->loop = 0;
    map->free_head = -1;
}



void map_insert_hash_at(Map* map, u64 hash, u32 idx, u32* bucket) {
    static bool _b = false;
    if (!_b) printf("TODO: Don't use map_insert_hash_at()!\n");
    _b = true;
    assert(bucket);
    assert(*bucket == -1);
    assert(idx != -1);
    *bucket = idx;
    map_hashes(map)[idx] = hash;
}

/*u32 map_insert_hash(Map* map, u64 hash) {
    static bool _b = false;
    if (!_b) printf("TODO: Don't use map_insert_hash()!\n");
    _b = true;
    map_expand(map);
    u32* bucket = map_find_empty_bucket(map, hash);
    if (*bucket != -1) return -1;
    u32 idx = map_alloc_idx(map);
    map_insert_hash_at(map, hash, idx, bucket);
    return idx;
}*/

/*void map_erase_hash_at_bucket(Map* map, u32* bucket) {
    static bool _b = false;
    if (!_b) printf("TODO: Don't use map_erase_hash_at_bucket()!\n");
    _b = true;
    assert(bucket);
    assert(*bucket < map->cap / 2);
    map_hashes(map)[*bucket] = map->free_head;
    map->free_head = *bucket;
    map_erase_bucket(map, bucket);
}*/

/*u32 map_erase_hash(Map* map, u64 hash) {
    static bool _b = false;
    if (!_b) printf("TODO: Don't use map_erase_hash()!\n");
    _b = true;
    if (!map->data) return -1;
    u32* bucket = map_find_bucket_with_hash(map, hash);
    if (!bucket) return -1;
    map_erase_hash_at_bucket(map, bucket);
    return *bucket;
}*/

/*void map_change_hash(Map* map, u32 idx, u64 new_hash) {
    static bool _b = false;
    if (!_b) printf("TODO: Don't use map_change_hash()!\n");
    _b = true;
    u64 old_hash = map_hashes(map)[idx];
    u32* bucket = map_find_bucket_with_idx(map, old_hash, idx);
    assert(*bucket == idx);
    map_erase_bucket(map, bucket);
    bucket = map_find_empty_bucket(map, new_hash);
    *bucket = idx;
    map_hashes(map)[idx] = new_hash;
}*/

// Experimental:
/*
u32* map_find_bucket_experimental(Map* map, void* keys, u64 hash, void* key, int(*cmp)(void* keys, u32 idx, void* key)) {
    assert(0);
    assert(map->data != NULL);
    u32 bucket_idx = (u32)hash % map->cap;
    u32* bucket = &map_buckets(map)[bucket_idx];
    while (true) {
        if (*bucket == -1)
            return bucket;
        if (map_hashes(map)[*bucket] == hash && cmp(keys, *bucket, key) == 0)
            return bucket;

        bucket_idx = (bucket_idx + 1) % map->cap;
        bucket = &map_buckets(map)[bucket_idx];
    }
}

u32 map_find_experimental(Map* map, void* keys, u64 hash, void* key, int(*cmp)(void* keys, u32 idx, void* key)) {
    assert(0);
    if (!map->data) return -1;
    return *map_find_bucket_experimental(map, key, cmp);
}

int map_find_str2_cmp_function(void* keys, u32 idx, void* key) {
    assert(0);
    return strcmp(((u8**)keys)[idx], (u8*)key);
}

u32 map_find_str2(Map* map, u8** keys, u8* key) {
    assert(0);
    return map_find_experimental(map, keys, hash_str(key), key, map_find_str2_cmp_function);
}
*/
