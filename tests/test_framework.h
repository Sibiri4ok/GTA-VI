#ifndef TEST_FRAMEWORK_H
#define TEST_FRAMEWORK_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#define TEST_ASSERT(condition, message) \
  do { \
    if (!(condition)) { \
      fprintf(stderr, "FAIL: %s:%d: %s\n", __FILE__, __LINE__, message); \
      test_context.failed++; \
      return; \
    } \
  } while(0)

#define TEST_ASSERT_EQ(a, b, message) TEST_ASSERT((a) == (b), message)
#define TEST_ASSERT_NEQ(a, b, message) TEST_ASSERT((a) != (b), message)
#define TEST_ASSERT_FLOAT_EQ(a, b, epsilon, message) TEST_ASSERT(fabsf((a) - (b)) < (epsilon), message)
#define TEST_ASSERT_VECTOR2_EQ(v1, v2, epsilon, message) \
  do { \
    TEST_ASSERT_FLOAT_EQ((v1).x, (v2).x, epsilon, message); \
    TEST_ASSERT_FLOAT_EQ((v1).y, (v2).y, epsilon, message); \
  } while(0)
#define TEST_ASSERT_NOT_NULL(ptr, message) TEST_ASSERT((ptr) != NULL, message)
#define TEST_ASSERT_NULL(ptr, message) TEST_ASSERT((ptr) == NULL, message)

typedef struct {
  int total;
  int passed;
  int failed;
  const char *current_test;
} TestContext;

extern TestContext test_context;
typedef void (*TestFunc)(void);

#define REGISTER_TEST(name) \
  void test_##name(void); \
  static void __attribute__((constructor)) __register_test_##name(void) { \
    register_test(#name, test_##name); \
  } \
  void test_##name(void)

void register_test(const char *name, TestFunc func);
void run_all_tests(void);
int test_runner_main(void);

#endif
