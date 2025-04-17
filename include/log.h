/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __LOG_H__
#define __LOG_H__

#define LOG_LEVEL_ERROR   1
#define LOG_LEVEL_WARN    2
#define LOG_LEVEL_INFO    3

#ifdef LOG_COLOR_ENABLE
#define LOG_COLOR_NONE      "\033[0m"
#define LOG_COLOR_RED       "\033[31m"
#define LOG_COLOR_GREEN     "\033[33m"
#define LOG_COLOR_YELLOW    "\033[32m"
#define LOG_COLOR_BLUE      "\033[34m"
#else
#define LOG_COLOR_NONE
#define LOG_COLOR_RED
#define LOG_COLOR_GREEN
#define LOG_COLOR_YELLOW
#define LOG_COLOR_BLUE
#endif

#if LOG_LEVEL
void log_init(unsigned serial_id, unsigned baud);
void log_printf(const char *format, ...);
void log_dump(const void *data, unsigned size);
void log_assert(const char *ex_str, const char *func, unsigned int line);
#define LOG_E(format, ...)    log_printf(LOG_COLOR_RED"[E]:"format""LOG_COLOR_NONE"\n", ##__VA_ARGS__)
#else
#define LOG_E(...)
#define log_init(a, b)
#endif

#if LOG_LEVEL > 1
#define LOG_W(format, ...)     log_printf(LOG_COLOR_YELLOW"[W]:"format""LOG_COLOR_NONE"\n", ##__VA_ARGS__)
#else 
#define LOG_W(format, ...)
#endif

#if LOG_LEVEL > 2
#define LOG_I(format, ...)      log_printf("[I]:"format"\n", ##__VA_ARGS__)
#define LOG_OUT(format, ...)    log_printf(format, ##__VA_ARGS__)
#define LOG_DUMP(data, size)    log_dump(data, size)
#ifndef NDEBUG
#define LOG_ASSERT(EX)          if (!(EX)) { log_assert(#EX, __func__, __LINE__); }
#else
#define LOG_ASSERT(EX)
#endif
#else
#define LOG_I(format, ...)
#define LOG_OUT(format, ...)
#define LOG_DUMP(data, size)
#define LOG_ASSERT(EX)
#endif

#endif
