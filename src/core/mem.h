#pragma once

#ifdef _WIN32
#include <windows.h>
#else
#endif

#include "core/types.h"
#include "core/macro.h"

#ifdef _WIN32

uptr mem_get_page_size() {
    SYSTEM_INFO sys_info;
    GetSystemInfo(&sys_info);
    return sys_info.dwPageSize;
}

void* mem_reserve(uptr size) {
    void *ptr = VirtualAlloc(NULL, (SIZE_T)size, MEM_RESERVE, PAGE_READWRITE);
    if (!ptr) {
        DWORD err = GetLastError();
        RUNTIME_ERROR("Could not reserve memory. VirtualAlloc() returned %lu.", err);
    }
    return ptr;
}

void mem_commit(void* ptr, uptr size) {
    void* ptr2 = VirtualAlloc(ptr, (SIZE_T)size, MEM_COMMIT, PAGE_READWRITE);
    if (ptr2 == NULL) {//if (ptr2 != ptr) {
        DWORD err = GetLastError();
        if (err == ERROR_INVALID_ADDRESS) {
            RUNTIME_ERROR("Memory was not reserved");
        } else {
            RUNTIME_ERROR("Out of memory. VirtualAlloc() returned %lu.", err);
        }
    }
}

void mem_free(void* ptr, uptr size) {
    VirtualFree(ptr, 0, MEM_RELEASE);
}

void mem_advice_free(void* ptr, uptr size) {
    //DWORD err = DiscardVirtualMemory(ptr, size);
    //if (err != ERROR_SUCCESS) {
    //    err = GetLastError();
    //    RUNTIME_ERROR("DiscardVirtualMemory() returned %lu.", err);
    //}
    DWORD err = VirtualAlloc(ptr, size, MEM_RESET, PAGE_READWRITE);
    if (err != ERROR_SUCCESS) {
        err = GetLastError();
        RUNTIME_ERROR("VirtualAlloc() returned %lu.", err);
    }
}

#else
#include <sys/mman.h>

uptr mem_get_page_size() {
    return getpagesize();
}

void* mem_alloc(uptr size) {
    void *ptr = mmap(NULL, (size_t)size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
    if (ptr == MAP_FAILED) {
        RUNTIME_ERROR("Could not reserve memory. mmap() returned %i.", errno);
    }
    return ptr;
}

void* mem_reserve(uptr size) {
    void *ptr = mmap(NULL, (size_t)size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS | MAP_NORESERVE, -1, 0);
    if (ptr == MAP_FAILED) {
        RUNTIME_ERROR("Could not reserve memory. mmap() returned %i.", errno);
    }
    return ptr;
}

void mem_commit(void* ptr, uptr size) {
    int err = mprotect(ptr, size, PROT_READ | PROT_WRITE);
    if (err == -1) {
        RUNTIME_ERROR("Out of memory. mprotect() returned %i.", errno);
    }
}

void mem_free(void* ptr, uptr size) {
    int err = munmap(ptr, (size_t)size);
    if (err == -1) {
        RUNTIME_ERROR("munmap() returned %i.", errno);
    }
}

void mem_advice_free(void* ptr, uptr size) {
    int err = madvise(ptr, size, MADV_FREE);
    if (err == -1) {
        RUNTIME_ERROR("madvise() returned  %i.", errno);
    }
}
#endif
