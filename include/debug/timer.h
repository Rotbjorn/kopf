#pragma once
#include "pch.h"

#define __TIMER_ENABLED

#ifdef __TIMER_ENABLED

#define TIMER_START() struct Timer *__timer = __timer_start_impl(__FUNCTION__)

#define TIMER_START_NAME(name) struct Timer *__timer_##name = __timer_start_impl(#name)

#define TIMER_END() __timer_stop_impl(&__timer)

#define TIMER_END_NAME(name) __timer_stop_impl(&__timer_##name)

#define TIMER_DUMP() __timer_dump_impl()

#else
#define TIMER_START()
#define TIMER_START_NAME(name, var)
#define TIMER_END(tm)
#define TIMER_END
#define TIMER_DUMP()
#endif

struct Timer {
    const char *name;
    uint64_t start;
    uint64_t end;
};

struct Timer *__timer_start_impl(const char *func_name);
void __timer_stop_impl(struct Timer **timer);
void __timer_dump_impl();
