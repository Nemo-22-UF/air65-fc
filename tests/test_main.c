#include "test.h"

int test_failures = 0;
int test_checks = 0;

void run_mixer_tests(void);

int main(void)
{
    run_mixer_tests();

    printf("\n%d checks, %d failed\n", test_checks, test_failures);
    return test_failures == 0 ? 0 : 1;
}
