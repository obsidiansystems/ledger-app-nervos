#pragma once

#include <string.h>

#include "exception.h"


#define COMPARE(a, b) ({ \
    _Static_assert(sizeof(*a) == sizeof(*b), "Size mismatch in COMPARE"); \
    check_null(a); \
    check_null(b); \
    memcmp(a, b, sizeof(*a)); \
})
#define NUM_ELEMENTS(a) (sizeof(a)/sizeof(*a))


// Macro that produces a compile-error showing the sizeof the argument.
#define ERROR_WITH_NUMBER_EXPANSION(x) char (*__kaboom)[x] = 1;
#define SIZEOF_ERROR(x)  ERROR_WITH_NUMBER_EXPANSION(sizeof(x));

#ifdef NERVOS_DEBUG
// Aid for tracking down app crashes
#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)
#define AT __FILE__ ":" TOSTRING(__LINE__)
#define DBGOUT {int i; PRINTF(AT " - sp %p spg %p %d\n", &i, &app_stack_canary, app_stack_canary); }
#endif

