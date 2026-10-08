#ifndef TEST_H
#define TEST_H

#include <math.h>
#include <stdio.h>

extern int test_failures;
extern int test_checks;

#define CHECK(cond) do { \
    test_checks++; \
    if (!(cond)) { \
        test_failures++; \
        printf("  FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
    } \
} while (0)

#define CHECK_NEAR(a, b, tol) CHECK(fabsf((a) - (b)) <= (tol))

#define RUN(test_fn) do { \
    printf("%s\n", #test_fn); \
    test_fn(); \
} while (0)

#endif
