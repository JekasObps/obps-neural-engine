#ifndef _BENCHMARK_H_
#define _BENCHMARK_H_

#include "test_utils.h"

#include <time.h>
#include <math.h>

#define BENCH_SETUP static clock_t s_delta, s_diff;

#define MEASURE(code) { \
        clock_t start = clock(), end; \
        code; \
        end = clock(); \
        s_delta = end - start; \
        printf("time: %Lf ms\n", CONVERT_TO_MS(s_delta)); \
    }

#define PRINT_RESULT(first, second) \
        printf(s_diff < 0 ? CGRN "%.10s ... < %Lfms" CRED " %.10s ... \n" : CRED "%s > %Lfms" CGRN " %s \n", first, CONVERT_TO_MS(s_diff), second); 


#define CONQUER(code1, code2, iterations) {\
        MEASURE({int i; for(i=0; i < iterations; ++i) {code1;}}) \
        s_diff = s_delta; \
        MEASURE({int i; for(i=0; i < iterations; ++i) {code2;}}) \
        s_diff -= s_delta; \
        PRINT_RESULT(#code1, #code2) \
    }

            
#define CONVERT_TO_MS(clock) (long double)clock / CLOCKS_PER_SEC * 1000


#endif /*_BENCHMARK_H_*/
