#pragma once

// TODO: Maybe replace Time with i64, time varibles will be named with ns as suffix or prefix like ns_since_event. Units like T_S will be replaced by S_TO_NS.

typedef i64 Time;

#define T_NS (1ll)
#define T_US (1000ll * T_NS)
#define T_MS (1000ll * T_US)
#define T_S  (1000ll * T_MS)
#define T_MIN  (60ll * T_S)
#define T_HOUR (60ll * T_MIN)
#define T_DAY  (24ll * T_HOUR)

#ifdef _WIN32
#include <windows.h>

Time time_now() {
    LARGE_INTEGER t_;
    QueryPerformanceCounter(&t_);
    LARGE_INTEGER freq_;
    QueryPerformanceFrequency(&freq_);
    Time t = t_.QuadPart;
    t *= (T_S / freq_.QuadPart);
    assert(T_S / freq_.QuadPart * freq_.QuadPart == T_S && "Frequency must be base 10 and bigger than a nanosecond");
    
    return t;
    // Implementation using GetSystemTimeAsFileTime():
    /*
        FILETIME file_time;
        GetSystemTimeAsFileTime(&file_time);
        u64 time = ((u64)file_time.dwLowDateTime) | ((u64)file_time.dwHighDateTime << 32);
        u64 unix_time_start = 0x019DB1DED53E8000; // This number is used by alot of people, but couldn't find a good source to verify it.
        u64 ns = 100 * (time - unix_time_start);
        return (i64)ns;*/
}

void time_sleep(Time t) {
    Sleep(t / T_MS);
}
#else
#include <time.h>
#include <unistd.h>

Time time_now() {
    struct timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    u64 ns = time.tv_sec * T_S + time.tv_nsec * T_NS;
    return (i64)ns;
}

void time_sleep(Time t) {
    struct timespec s = {
        .tv_sec = t / T_S,
        .tv_nsec = (t % T_S) / T_NS,
    };
    int err;
    do {
        err = nanosleep(&s, &s);
    } while(err && errno == EINTR);
    /*
    struct timespec sleep_time;
    sleep_time.tv_sec = 0;
    sleep_time.tv_nsec = 1000000000 / target_fps;
    int err;
    do {
        err = nanosleep(&sleep_time, &sleep_time);
    } while(err && errno == EINTR);
   */
}
#endif
