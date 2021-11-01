#include "debug/timer.h"
#include <time.h>

struct TimeResult {
    const char *name;
    uint64_t elapsed;
};

struct TimeResult results[32];
static size_t results_size = 0;

static uint64_t get_nanoseconds(struct timespec *tm) {
    return (tm->tv_sec * 1000000000) + (tm->tv_nsec);
}

struct Timer *__timer_start_impl(const char *func_name) {
    struct Timer *timer = malloc(sizeof(struct Timer));

    struct timespec before;
    clock_gettime(CLOCK_MONOTONIC, &before);

    timer->start = get_nanoseconds(&before);
    timer->name = func_name;
    return timer;
}
void __timer_stop_impl(struct Timer **timer) {
    struct timespec after;
    clock_gettime(CLOCK_MONOTONIC, &after);

    const char *name = (*timer)->name;
    uint64_t start = (*timer)->start;
    uint64_t end = get_nanoseconds(&after);

    uint64_t elapsed_ns = end - start;

    free(*timer);
    *timer = NULL;

    results[results_size++] = (struct TimeResult){name, elapsed_ns};
}


void __timer_dump_impl() {
    for(int i = 0; i < results_size; i++) {
        struct TimeResult result = results[i];
        printf("[%s]: %ld ns\n", result.name, result.elapsed);
    }
}
