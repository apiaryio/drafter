#include "ctesting.h"

#include <string.h>
#include <stdio.h>

size_t strlen_until_nl(const char* str)
{

    size_t i = 0;
    for (; str[i] != 0 && str[i] != '\n'; ++i)
        ;

    return i;
}

void drafter_ctest_fail(const char* expr, const char* file, size_t line)
{
    fprintf(stderr, "Assertion `%s` failed at %s:%d\n", expr, file, line);
    abort();
}

void drafter_ctest_assert(int a, const char* expr, const char* file, size_t line)
{
    if (!a)
        drafter_ctest_fail(expr, file, line);
}

void drafter_ctest_includes(const char* searched, const char* actual, const char* file, size_t line)
{
    const char* where = strstr(actual, searched);
    if (!where) {
        fprintf( //
            stderr,
            "Didn't find sub string\n"
            "\x1b[32m%s\x1b[0m\n"
            "in\n"
            "\x1b[31m%s\x1b[0m\n",
            searched,
            actual);
        drafter_ctest_fail("expected sub string not found", file, line);
    }
}

void drafter_ctest_excludes(const char* searched, const char* actual, const char* file, size_t line)
{
    const char* where = strstr(actual, searched);
    if (where) {
        while (where != actual) {
            if (where[0] == '\n') {
                ++where;
                break;
            }
            --where;
        }
        fprintf( //
            stderr,
            "Found prohibited sub string\n"
            "\x1b[32m%s\x1b[0m\n"
            "at\n"
            "\x1b[31m%s\x1b[0m\n",
            searched,
            where);
        drafter_ctest_fail("prohibited sub string found", file, line);
    }
}
