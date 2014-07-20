#ifndef __ARCH_PERF_H__
#define __ARCH_PERF_H__

#ifdef PERF
   #define PERF_START
   #define PERF_STOP(x)

/*#define PERF_START do { \
                     struct tms __perf_start, __perf_end; \
                     times(&__perf_start)
#define PERF_STOP(x) times(&__perf_end); \
                     perf_print_times(&__perf_start, &__perf_end, x);\
                     } while(0)*/
#else /* PERF */
   #define PERF_START /* null definition */
   #define PERF_STOP(x) /* null definition */
#endif /* PERF */

void perf_print(unsigned long c1l, unsigned long c1h,
                unsigned long c2l, unsigned long c2h,
                char *key);

void perf_print_times(struct tms *start, struct tms *end, char *key);

void perf_init(char *fname);

#endif /* __ARCH_PERF_H__ */
