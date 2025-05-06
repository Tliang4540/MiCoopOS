/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __TIMER_H__
#define __TIMER_H__

typedef struct
{
    void *user_data;
}timer_handle_t;

void timer_handle_init(timer_handle_t *timer_handle, unsigned int timerid);
void timer_open(timer_handle_t *timer_handle, unsigned int freq, void (*callback)(void));
void timer_start(timer_handle_t *timer, unsigned int interval);
void timer_stop(timer_handle_t *timer);
void timer_close(timer_handle_t *timer);
unsigned int timer_read(timer_handle_t *timer);

#endif
