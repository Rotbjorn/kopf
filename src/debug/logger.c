#include "debug/logger.h"
#include "pch.h"
#include <stdarg.h>

void __flog_impl(const char* file_name, const char* func_name, const char *format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stdout, "[LOG] [%s:%s]: ", file_name, func_name);
    vfprintf(stdout, format, args);

    va_end(args);
}
void __fwarn_impl(const char* file_name, const char* func_name, const char *format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stdout, "[WARN] [%s:%s]: ", file_name, func_name);
    vfprintf(stdout, format, args);

    va_end(args);
}
void __ferror_impl(const char* file_name, const char* func_name, const char *format, ...) {
    va_list args;
    va_start(args, format);
    fprintf(stderr, "[ERROR] [%s:%s]: ", file_name, func_name);
    vfprintf(stderr, format, args);

    va_end(args);
}
