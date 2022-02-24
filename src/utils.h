#ifndef _UTILS_H_
#define _UTILS_H_

#define ASSERT_CONCAT_(a, b) a##b
#define ASSERT_CONCAT(a, b) ASSERT_CONCAT_(a, b)

#define STATIC_ASSERT(e)\
    enum { ASSERT_CONCAT(assert_line_, __LINE__) = 1/(!!(e)) }

#define UNUSED(var) (void)var;

#define ARRLEN(arr) (sizeof(arr)/sizeof(arr[0]))

#endif/*_UTILS_H*/
