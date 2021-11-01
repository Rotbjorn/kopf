#pragma once

#define __LOGGING_ENABLED

#ifdef __LOGGING_ENABLED
#define LOG(format) __flog_impl(__FILE__, __FUNCTION__, format)
#define FLOG(format, ...) __flog_impl(__FILE__, __FUNCTION__, format, __VA_ARGS__)
#define WARN(format) __fwarn_impl(__FILE__, __FUNCTION__, format)
#define FWARN(format, ...) __fwarn_impl(__FILE__, __FUNCTION__, format, __VA_ARGS__)
#define ERROR(format) __ferror_impl(__FILE__, __FUNCTION__, format)
#define FERROR(format, ...) __ferror_impl(__FILE__, __FUNCTION__, format, __VA_ARGS__)
#else
#define LOG(format, ...)
#define WARN(format, ...)
#define ERROR(format, ...)
#endif

void __flog_impl(const char* file_name, const char* func_name, const char* format, ...);
void __fwarn_impl(const char* file_name, const char* func_name, const char* format, ...);
void __ferror_impl(const char* file_name, const char* func_name, const char* format, ...);
