#pragma once

#include <assert.h>

#include "core/types.h"

// TODO: Add debug assertions to check correct usage.

#if (_WIN32 && !EMSCRIPTEN)
#include <winsock2.h>
#include <windows.h>
#else
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdlib.h>
#include <semaphore.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/syscall.h>
#endif


#ifdef _WIN32
typedef HANDLE Semaphore;
typedef CRITICAL_SECTION Mtx;
typedef CONDITION_VARIABLE CndVar;
#else
typedef sem_t Semaphore;
typedef pthread_mutex_t Mtx;
typedef pthread_cond_t CndVar;
#endif

#ifdef _MSC_VER
#define THREAD_LOCAL __declspec(thread)
#else
#define THREAD_LOCAL __thread
#endif

typedef struct {
#ifdef _WIN32
    void* handle;
#else
    pthread_t handle;
#endif
    void (*function)(void*);
    void* data;
} Thread;

#ifdef _WIN32
void semaphore_init(Semaphore* semaphore) {
    *semaphore = CreateSemaphoreA(NULL, 0, INT_MAX, NULL);
}

void semaphore_deinit(Semaphore* semaphore) {
    assert(*semaphore);
    CloseHandle(*semaphore);
}

void semaphore_signal(Semaphore* semaphore, u32 count) {
    assert(*semaphore);
    ReleaseSemaphore(*semaphore, count, NULL);
}

void semaphore_wait_once(Semaphore* semaphore) {
    assert(*semaphore);
    WaitForSingleObject(*semaphore, INFINITE);
}

#else
void semaphore_init(Semaphore* semaphore) {
    sem_init(semaphore, 0, 0);
}

void semaphore_deinit(Semaphore* semaphore) {
    sem_destroy(semaphore);
}

void semaphore_signal(Semaphore* semaphore, u32 count) {
    while(count-- > 0) sem_post(semaphore);
}

void semaphore_wait_once(Semaphore* semaphore) {
    int r;
    do {
        r = sem_wait(semaphore);
    } while(r == -1 && errno == EINTR);
    assert(r == 0);
}
#endif

#ifdef _WIN32
void mtx_init(Mtx* mtx) {
    InitializeCriticalSection(mtx);
}

void mtx_deinit(Mtx* mtx) {
    DeleteCriticalSection(mtx);
}

void mtx_lock(Mtx* mtx) {
    EnterCriticalSection(mtx);
}

void mtx_unlock(Mtx* mtx) {
    LeaveCriticalSection(mtx);
}

#else
void mtx_init(Mtx* mtx) {
    if (pthread_mutex_init(mtx, NULL)) {
        assert(0);
    }
}

void mtx_deinit(Mtx* mtx) {
    pthread_mutex_destroy(mtx);
}

void mtx_lock(Mtx* mtx) {
    pthread_mutex_lock(mtx);
}

void mtx_unlock(Mtx* mtx) {
    pthread_mutex_unlock(mtx);
}
#endif

#ifdef _WIN32
void cnd_var_init(CndVar* cnd_var) {
    InitializeConditionVariable(cnd_var);
}
void cnd_var_deinit(CndVar* cnd_var) {
}
void cnd_var_signal(CndVar* cnd_var) {
    WakeConditionVariable(cnd_var);
}
void cnd_var_signal_all(CndVar* cnd_var) {
    WakeAllConditionVariable(cnd_var);
}
void cnd_var_wait(CndVar* cnd_var, Mtx* mtx) {
    SleepConditionVariableCS(cnd_var, mtx, INFINITE);
}
#else
void cnd_var_init(CndVar* cnd_var) {
    int err = pthread_cond_init(cnd_var, NULL);
    assert(err == 0);
}
void cnd_var_deinit(CndVar* cnd_var) {
    int err = pthread_cond_destroy(cnd_var);
    assert(err == 0);
}
void cnd_var_signal(CndVar* cnd_var) {
    int err = pthread_cond_signal(cnd_var);
    assert(err == 0);
}
void cnd_var_signal_all(CndVar* cnd_var) {
    int err = pthread_cond_broadcast(cnd_var);
    assert(err == 0);
}
void cnd_var_wait(CndVar* cnd_var, Mtx* mtx) {
    int err = pthread_cond_wait(cnd_var, mtx);
    assert(err == 0);
}
#endif

static inline void thread_run_function(Thread* thread) {
    thread->function(thread->data);
}

#ifdef _WIN32
static inline DWORD __stdcall thread_start_function(void* arg) {
    Thread* thread = arg;
    thread_run_function(thread);
    return 0;
}
#else
static inline void* thread_start_function(void* arg) {
    Thread* thread = arg;
    thread_run_function(thread);
    return NULL;
}
#endif

void thread_init(Thread* thread) {
    thread->function = NULL;
    thread->data = NULL;
#ifdef _WIN32
    thread->handle = INVALID_HANDLE_VALUE;
#else
    thread->handle = 0;
#endif
}

void thread_start(Thread* thread, void (*function)(void*), void* data) {
    assert(function);
    thread->function = function;
    thread->data = data;
#ifdef _WIN32
    thread->handle = CreateThread(NULL, 0, thread_start_function, thread, 0, NULL);
    assert(thread->handle);
#else
    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_create(&thread->handle, &attr, thread_start_function, thread);
    pthread_attr_destroy(&attr);
#endif
}

void thread_join(Thread* thread) {
#ifdef _WIN32
    WaitForSingleObject(thread->handle, INFINITE);
    thread->handle = INVALID_HANDLE_VALUE;
#else
    pthread_join(thread->handle, NULL);
    thread->handle = 0;
#endif
}



u32 thread_get_id() {
#if defined(_WIN32)
    return GetCurrentThreadId();
#else
    return (u32)pthread_self();//return syscall(SYS_gettid);
#endif
}


void semaphore_wait(Semaphore* semaphore, u32 count) {
    for (u32 i = 0; i < count; i++) {
        semaphore_wait_once(semaphore);
    }
}