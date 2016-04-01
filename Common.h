#pragma once

#include <cstdio>
#include <cstdlib>

static void runtimeError(const char* str) {
    printf("Error: %s\n", str);
    exit(-1);
}

static void runtimeError(const int lineNumber, const char* str) {
    printf("Line: %i Error: %s\n", lineNumber, str);
    exit(-1);
}