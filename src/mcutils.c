/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#include <mcutils.h>

unsigned int mc_utoa(unsigned int val, char *str, unsigned int radix)
{
    unsigned int digit;
    char *start = str;

    if (radix == 16)
    {
        do
        {
            digit = val & 0xf;
            val >>= 4;
            *start++ = digit < 10 ? digit + '0' : digit + ('a' - 10);
        } while (val);
    }
    else
    {
        do
        {
            digit = val % radix;
            val /= radix;
            *start++ = digit + '0';
        } while (val);
    }

    unsigned int len = start - str;
    start--;

    while (str < start)
    {
        char tmp = *str;
        *str++ = *start;
        *start-- = tmp;
    }

    return len;
}

#define DAY_SEC (24 * 60 * 60)
#define YEAR_SEC (365 * DAY_SEC)
#define FOUR_YEAR_SEC (1461 * DAY_SEC)
#define BASE_DOW (4)

static const unsigned int days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static const unsigned int mon_yday[2][12] =
    {
        {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},
        {0, 31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335},
};

static unsigned int isleap(unsigned int year)
{
    return (year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0);
}

unsigned int mc_mktime(struct mc_tm *tm)
{
    unsigned int result;

    result = (tm->year - 1970) * 31536000 +
             (mon_yday[isleap(tm->year)][tm->mon - 1] + tm->mday - 1) * 86400 +
             tm->hour * 3600 + tm->min * 60 + tm->sec;

    for (unsigned int i = 1970; i < tm->year; i++)
    {
        if (isleap(i))
        {
            result += 86400;
        }
    }
    return result;
}

void mc_localtime(unsigned int timep, struct mc_tm *tm)
{
    unsigned int pass_four_year;
    unsigned int hours_per_year;

    tm->wday = (timep / DAY_SEC + BASE_DOW) % 7;

    tm->sec = (uint8_t)(timep % 60); // 获得秒
    timep /= 60;

    tm->min = (uint8_t)(timep % 60); // 获得分
    timep /= 60;
    // 取过去多少个四年，每四年有 1461*24 小时
    pass_four_year = timep / (1461 * 24);
    tm->year = (pass_four_year * 4) + 1970; // 得到年份
    // 四年中剩下的小时数
    timep %= 1461 * 24;
    // 校正闰年影响的年份，计算一年中剩下的小时数
    while (1)
    {
        hours_per_year = 365 * 24; // 一年的小时数
        if ((tm->year & 3) == 0)   // 判断闰年，是闰年，一年则多24小时，即一天
            hours_per_year += 24;
        if (timep < hours_per_year)
            break;
        tm->year++;
        timep -= hours_per_year;
    }
    tm->hour = timep % 24; // 获得小时
    timep /= 24;
    timep++;
    if ((tm->year & 3) == 0)
    {
        if (timep > 60)
        {
            timep--;
        }
        else
        {
            if (timep == 60)
            {
                tm->mon = 2;
                tm->mday = 29;
                return;
            }
        }
    }
    // 计算月日
    for (tm->mon = 0; days[tm->mon] < timep; tm->mon++)
    {
        timep -= days[tm->mon];
    }

    tm->mday = timep;
    tm->mon++;
}

unsigned char mc_bin2bcd(unsigned int val)
{
    return ((val / 10) << 4) + val % 10;
}

unsigned int mc_bcd2bin(unsigned char val)
{
    return (val & 0x0f) + ((val >> 4) * 10);
}
