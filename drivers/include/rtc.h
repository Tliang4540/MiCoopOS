/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __RTC_H__
#define __RTC_H__

struct rtc_tm
{
    unsigned int sec;
    unsigned int min;
    unsigned int hour;
    unsigned int wday;
    unsigned int mday;
    unsigned int mon;
    unsigned int year;
};

void rtc_init(void);
void rtc_wakeup_time_open(unsigned int ms, void (*callback)(void));
void rtc_wakeup_time_close(void);
void rtc_set_time(struct rtc_tm *t);
void rtc_get_time(struct rtc_tm *t);
void rtc_localtime(unsigned int time, struct rtc_tm *t);
unsigned int rtc_mktime(struct rtc_tm *time);

#endif
