#pragma once

#if defined(_MSC_VER) && !defined(__clang__)
#define IS_MSVC
#endif

#ifdef IS_MSVC
#include <intrin.h>
#endif

// TODO: atomics for pointers
// TODO: Perhaps change from signed to unsigned as signed can be more unpredictable

#ifdef _IS_MSVC
void atomic_read_fence() {
#ifndef __x86_64
    LONG barrier;
    _InterlockedOr(&barrier, 0);
#endif
    _ReadBarrier();
}
void atomic_write_fence() {
#ifndef __x86_64
    LONG barrier;
    _InterlockedOr(&barrier, 0);
#endif
    _WriteBarrier();
}
void atomic_read_write_fence() {
#ifndef __x86_64
    LONG barrier;
    _InterlockedOr(&barrier, 0);
#endif
    _ReadWriteBarrier();
}
#else
void atomic_read_write_fence() {
    __sync_synchronize();
}
void atomic_read_fence() {
    // TODO: implement atomic_read_fence()
    atomic_read_write_fence();
}
void atomic_write_fence() {
    // TODO: implement atomic_write_fence()
    atomic_read_write_fence();
}
#endif

#ifdef IS_MSVC
u32 atomic_load_u32(volatile u32* target) {
    return _InterlockedOr(target, 0);
}
u64 atomic_load_u64(volatile u64* target) {
    return _InterlockedOr64(target, 0);
}
u32 atomic_store_u32(volatile u32* target, u32 val) {
    return _InterlockedExchange(target, val);
}
u64 atomic_store_u64(volatile u64* target, u64 val) {
    return _InterlockedExchange64(target, val);
}
u32 atomic_exchange_u32(volatile u32* target, u32 val) {
    return _InterlockedExchange(target, val);
}
u64 atomic_exchange_u64(volatile u64* target, u64 val) {
    return _InterlockedExchange64(target, val);
}
u32 atomic_compare_exchange_u32(volatile u32* target, u32 expected, u32 val) {
    return _InterlockedCompareExchange(target, val, expected);
}
u64 atomic_compare_exchange_u64(volatile u64* target, u64 expected, u64 val) {
    return _InterlockedCompareExchange64(target, val, expected);
}
u32 atomic_fetch_and_increment_u32(volatile u32* target) {
    return _InterlockedIncrement(target) - 1;
}
u64 atomic_fetch_and_increment_u64(volatile u64* target) {
    return _InterlockedIncrement64(target) - 1;
}
u32 atomic_fetch_and_decrement_u32(volatile u32* target) {
    return _InterlockedDecrement(target) + 1;
}
u64 atomic_fetch_and_decrement_u64(volatile u64* target) {
    return _InterlockedDecrement64(target) + 1;
}
u32 atomic_fetch_and_add_u32(volatile u32* target, u32 val) {
    return _InterlockedAdd(target, val);
}
u64 atomic_fetch_and_add_u64(volatile u64* target, u64 val) {
    return _InterlockedAdd64(target, val);
}
u32 atomic_fetch_and_or_u32(volatile u32* target, u32 val) {
    return _InterlockedOr(target, val);
}
u64 atomic_fetch_and_or_u64(volatile u64* target, u64 val) {
    return _InterlockedOr64(target, val);
}
u32 atomic_fetch_and_and_u32(volatile u32* target, u32 val) {
    return _InterlockedAnd(target, val);
}
u64 atomic_fetch_and_and_u64(volatile u64* target, u64 val) {
    return _InterlockedAnd64(target, val);
}
u32 atomic_fetch_and_xor_u32(volatile u32* target, u32 val) {
    return _InterlockedXor(target, val);
}
u64 atomic_fetch_and_xor_u64(volatile u64* target, u64 val) {
    return _InterlockedXor(target, val);
}
#else

u32 atomic_load_u32(volatile u32* target) {
    return __sync_fetch_and_or(target, 0);
}
u64 atomic_load_u64(volatile u64* target) {
    return __sync_fetch_and_or(target, 0);
}
u32 atomic_store_u32(volatile u32* target, u32 val) {
    u32 r = __sync_lock_test_and_set(target, val);
    __sync_synchronize(); // TODO: Check if this is needed.
    return r;
}
u64 atomic_store_u64(volatile u64* target, u64 val) {
    u64 r = __sync_lock_test_and_set(target, val);
    __sync_synchronize(); // TODO: Check if this is needed.
    return r;
}
u32 atomic_exchange_u32(volatile u32* target, u32 val) {
    u32 r = __sync_lock_test_and_set(target, val);
    __sync_synchronize(); // TODO: Check if this is needed.
    return r;
}
u64 atomic_exchange_u64(volatile u64* target, u64 val) {
    u64 r = __sync_lock_test_and_set(target, val);
    __sync_synchronize(); // TODO: Check if this is needed.
    return r;
}
u32 atomic_compare_exchange_u32(volatile u32* target, u32 expected, u32 val) {
    return __sync_val_compare_and_swap(target, expected, val);
}
u64 atomic_compare_exchange_u64(volatile u64* target, u64 expected, u64 val) {
    return __sync_val_compare_and_swap(target, expected, val);
}
u32 atomic_fetch_and_increment_u32(volatile u32* target) {
    return __sync_fetch_and_add(target, 1);
}
u64 atomic_fetch_and_increment_u64(volatile u64* target) {
    return __sync_fetch_and_add(target, 1);
}
u32 atomic_fetch_and_decrement_u32(volatile u32* target) {
    return __sync_fetch_and_add(target, -1);
}
u64 atomic_fetch_and_decrement_u64(volatile u64* target) {
    return __sync_fetch_and_add(target, -1);
}
u32 atomic_fetch_and_add_u32(volatile u32* target, u32 val) {
    return __sync_fetch_and_add(target, val);
}
u64 atomic_fetch_and_add_u64(volatile u64* target, u64 val) {
    return __sync_fetch_and_add(target, val);
}
u32 atomic_fetch_and_or_u32(volatile u32* target, u32 val) {
    return __sync_fetch_and_or(target, val);
}
u64 atomic_fetch_and_or_u64(volatile u64* target, u64 val) {
    return __sync_fetch_and_or(target, val);
}
u32 atomic_fetch_and_and_u32(volatile u32* target, u32 val) {
    return __sync_fetch_and_and(target, val);
}
u64 atomic_fetch_and_and_u64(volatile u64* target, u64 val) {
    return __sync_fetch_and_and(target, val);
}
u32 atomic_fetch_and_xor_u32(volatile u32* target, u32 val) {
    return __sync_fetch_and_xor(target, val);
}
u64 atomic_fetch_and_xor_u64(volatile u64* target, u64 val) {
    return __sync_fetch_and_xor(target, val);
}
#endif
