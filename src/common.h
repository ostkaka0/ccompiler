#ifndef COMMON_H
#define COMMON_H

#include <stdio.h>
#include <stdlib.h>

#define DEBUG_BREAK raise(SIGTRAP)
#define ERROR(MESSAGE) (printf("\nERROR: %s\n\tat '%s', line #%i\n\n", MESSAGE, __FILE__, __LINE__), DEBUG_BREAK, getchar(), exit(EXIT_FAILURE))
#define PARSE_ERROR(MESSAGE)  (printf("\nERROR: %s\n\tat '%s', line #%i\n\n", MESSAGE, __FILE__, __LINE__), DEBUG_BREAK)

static void runtime_error_simple(const char* str) {
    printf("_error: %s\n", str);
    exit(-1);
}

static void runtime_error(const int line_number, const char* str) {
    printf("Line: %i _error: %s\n", line_number, str);
    exit(-1);
}

#endif // COMMON_H