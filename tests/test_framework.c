#include "test_framework.h"

#define MAX_TESTS 256

typedef struct {
  const char *name;
  TestFunc func;
} TestEntry;

static TestEntry tests[MAX_TESTS];
static int test_count = 0;

TestContext test_context = {0, 0, 0, NULL};

void register_test(const char *name, TestFunc func) {
  if (test_count >= MAX_TESTS) return;
  tests[test_count].name = name;
  tests[test_count].func = func;
  test_count++;
}

void run_all_tests(void) {
  test_context.total = test_count;
  test_context.passed = 0;
  test_context.failed = 0;

  for (int i = 0; i < test_count; i++) {
    test_context.current_test = tests[i].name;
    printf("Running test: %s\n", tests[i].name);
    int prev_failed = test_context.failed;
    tests[i].func();
    if (test_context.failed == prev_failed) {
      printf("  ✓ PASSED\n");
      test_context.passed++;
    } else {
      printf("  ✗ FAILED\n");
    }
    printf("\n");
  }

  if (test_context.failed == 0) {
    printf("\n✓ All tests passed!\n");
  } else {
    printf("\n✗ Some tests failed!\n");
  }
}

int test_runner_main(void) {
  run_all_tests();
  return test_context.failed > 0 ? 1 : 0;
}
