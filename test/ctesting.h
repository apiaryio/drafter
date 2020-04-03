#ifndef DRAFTER_CTESTING_H
#define DRAFTER_CTESTING_H

#include <stdlib.h>

#define FAIL(expr) drafter_ctest_fail(#expr, __FILE__, __LINE__)
void drafter_ctest_fail(const char* expr, const char* file, size_t line);

#define REQUIRE(expr) drafter_ctest_assert(expr != 0, #expr, __FILE__, __LINE__)
void drafter_ctest_assert(int a, const char* expr, const char* file, size_t line);

#define REQUIRE_INCLUDES(searched, actual) drafter_ctest_includes(searched, actual, __FILE__, __LINE__)
void drafter_ctest_includes(const char* expected, const char* actual, const char* file, size_t line);

#define REQUIRE_EXCLUDES(searched, actual) drafter_ctest_excludes(searched, actual, __FILE__, __LINE__)
void drafter_ctest_excludes(const char* expected, const char* actual, const char* file, size_t line);

#endif
