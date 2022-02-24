#ifndef _TEST_UTILS_
#define _TEST_UTILS_

#include <stdio.h> /* fprintf, fflush */
#include <stdlib.h> /* exit */
/******************************
 *  Testing mini-"framework"  *
 ******************************/

/* colors: */
#define CGRN "\033[32m"
#define CRED "\033[91m"
#define RESET "\033[0m"

/* declare global test stuff */
#define TEST_SETUP static int s_test_status = 0; /* 0 = success, -1 = fail */

/*
 *  val -- value or evaluated expression
 *  exp -- to be compared with
 *  cmp -- via that function
 *  succ -- format string for sucess message
 *  fail -- format string for fail message
 */
#define TEST_CASE(val, exp, cmp, succ, fail) \
    printf("TEST %s :: %s\n %s %s %s at:%i\n", __FILE__, __func__, #val, #cmp, #exp, __LINE__); fflush(stdout); \
    if (cmp(val, exp)) \
        fprintf(stdout, CGRN succ RESET "\n"); \
    else { \
        fprintf(stderr, CRED fail RESET "\n", exp, val); \
        s_test_status = -1; \
    }

/* does not continue to next tests on failure */
#define TEST_CASE_BREAK(val, exp, cmp, succ, fail) \
    TEST_CASE(val, exp, cmp, succ, fail) \
    if (!s_test_status) TEST_RETURN()

/* test exit point */
#define TEST_RETURN() exit(s_test_status);

/* common comparators */
#define EQ(a, b) a == b
#define NE(a, b) a != b
#define GT(a, b) a > b
#define LT(a, b) a < b
#define GTEQ(a, b) a >= b
#define LTEQ(a, b) a <= b


#define STREQ(a, b) strcmp(a, b) == 0

#endif /*_TEST_UTILS_*/
