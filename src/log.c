/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#include <log.h>
#include <mclib.h>
#include <mcos.h>
#include <serial.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>

#if LOG_LEVEL

static serial_handle_t m_serial;
static unsigned int log_lock = 0;
static char log_buf[128];    //防止任务栈溢出, 使用静态变量

void log_printf(const char *format, ...)
{
    unsigned int written = 0;
    char ch;
    va_list args;

    if (log_lock)
        mc_delay(1);

    va_start(args, format);

    while(*format)
    {
        // 防止溢出
        // if (written > (sizeof(log_buf) / 2))
        // {
        //     serial_write(m_serial, log_buf, written);
        //     written = 0;
        // }

        ch = *format;
        format++;
        if(ch != '%')
        {
            log_buf[written++] = ch;
            continue;
        }

        ch = *format;
        format++;

        switch (ch)
        {
        case 'd':
        {
            int tmp = va_arg(args, int);
            if(tmp < 0)
            {
                log_buf[written++] = '-';
                tmp = -tmp;
            }
            written += mc_utoa(tmp, &log_buf[written], 10);
            break;
        }
        case 'x':
        {
            unsigned int tmp = va_arg(args, unsigned int);
            unsigned int mask = 0xfffffff0;
            unsigned int count = 0;
            while(tmp & mask)
            {
                mask <<= 4;
                count++;
            }
            if((count & 1) == 0)
                log_buf[written++] = '0';
            written += mc_utoa(tmp, &log_buf[written], 16);
            break;
        }
        case 's':
        {
            char *str = va_arg(args, char*);
            while (*str)
            {
                log_buf[written++] = *str++;
            }
            break;
        }
        default:
            log_buf[written++] = '%';
            format--;
            break;
        }
    }

    va_end(args);
    serial_write(&m_serial, log_buf, written);
    log_lock = 0;
}

void log_dump(const void *data, unsigned size)
{
    unsigned i;
    unsigned buf_index = 0;
    unsigned char *dump = (unsigned char*)data;

    if (log_lock)
        mc_delay(1);

    for(i = 0; i < size; i++)
    {
        log_buf[buf_index++] = (*dump >> 4) < 10 ? '0' + (*dump >> 4) : 'a' + (*dump >> 4) - 10;
        log_buf[buf_index++] = (*dump & 15) < 10 ? '0' + (*dump & 15) : 'a' + (*dump & 15) - 10;

        if(((i & 0x07) == 7) || ((i + 1) == size))
            log_buf[buf_index] = '\n';
        else
            log_buf[buf_index] = ' ';

        buf_index++;
        dump++;
        // 防止溢出, 人为控制不要溢出
        // if (buf_index > (sizeof(log_buf) / 2))
        // {
        //     serial_write(m_serial, log_buf, buf_index);
        //     size = 0;
        // }
    }
    serial_write(&m_serial, log_buf, buf_index);
    log_lock = 0;
}

#ifndef LOG_ASSERT_ENABLE
void log_assert(const char *ex_str, const char *func, unsigned int line)
{
    log_printf("%s:%d:(%s)\n", func, line, ex_str);
    while(1);
}
#endif

void log_init(unsigned serial_id, unsigned baud)
{
    serial_handle_init(&m_serial, serial_id);
    serial_open(&m_serial, baud);
}

#endif
