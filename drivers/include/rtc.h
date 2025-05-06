/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __RTC_H__
#define __RTC_H__

#include <mctypes.h>

void rtc_init(void);
void rtc_wakeup_time_open(unsigned int ms, void (*callback)(void));
void rtc_wakeup_time_close(void);
void rtc_set_time(struct mc_tm *tm);
void rtc_get_time(struct mc_tm *tm);

#endif
