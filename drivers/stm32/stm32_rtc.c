/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#include <rtc.h>
#include <stm32.h>
#include <log.h>
#include <mcconfig.h>

#if defined(BSP_USING_RTC)

#define __RTC_IRQn                (2)

#if defined(STM32G0)
#define __RTC_EXTI                (LL_EXTI_LINE_19)
#elif defined (STM32L0) || defined (STM32F0)
#define __RTC_EXTI              (LL_EXTI_LINE_20)
#endif

#define DAY_SEC             (24 * 60 * 60)
#define YEAR_SEC            (365 * DAY_SEC)
#define FOUR_YEAR_SEC       (1461 * DAY_SEC)
#define BASE_DOW            (4)

static const unsigned int days[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
static const unsigned int mon_yday[2][12] =
{
    {0,31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334},
    {0,31, 60, 91, 121, 152, 182, 213, 244, 274, 305, 335},
};

static void (*rtc_callback)(void);

static unsigned int isleap(unsigned int year)
{
    return (year) % 4 == 0 && ((year) % 100 != 0 || (year) % 400 == 0);
}

unsigned int rtc_mktime(struct rtc_tm *time)
{
    unsigned int result;
    
    result = (time->year - 1970) * 31536000 +
    (mon_yday[isleap(time->year)][time->mon - 1] + time->mday - 1) * 86400 +
    time->hour * 3600 + time->min * 60 + time->sec;

    for(unsigned int i = 1970; i < time->year; i++)
    {
        if(isleap(i))
        {
            result += 86400;
        }
    }
    return result;
}

void rtc_localtime(unsigned int time, struct rtc_tm *t)
{
    unsigned int pass_four_year;
    unsigned int hours_per_year;

    t->wday = (time / DAY_SEC + BASE_DOW) % 7;

    t->sec = (uint8_t)(time % 60);   //获得秒
    time /= 60;

    t->min = (uint8_t)(time % 60);  //获得分
    time /= 60;
    //取过去多少个四年，每四年有 1461*24 小时
    pass_four_year = time / (1461 * 24);
    t->year = (pass_four_year * 4) + 1970;   //得到年份
    //四年中剩下的小时数
    time %= 1461 * 24;
    //校正闰年影响的年份，计算一年中剩下的小时数
    while(1)
    {
        hours_per_year = 365 * 24;      //一年的小时数
        if((t->year & 3) == 0)       //判断闰年，是闰年，一年则多24小时，即一天
            hours_per_year += 24;
        if(time < hours_per_year)
            break;
        t->year++;
        time -= hours_per_year;
    }
    t->hour = time % 24;    //获得小时
    time /= 24;
    time++;
    if((t->year & 3) == 0)
    {
        if(time > 60)
        {
            time--;
        }
        else
        {
            if(time == 60)
            {
                t->mon = 2;
                t->mday = 29;
                return;
            }
        }
    }
    //计算月日
	for (t->mon = 0; days[t->mon] < time; t->mon++)
	{
		time -= days[t->mon];
	}

	t->mday = time;
	t->mon++;
}

void rtc_init(void)
{
    LL_RCC_EnableRTC();
#if defined (STM32G0)
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_RTC);
#endif
}

void rtc_wakeup_time_open(unsigned int ms, void (*callback)(void))
{
    rtc_callback = callback;

    LL_RTC_DisableWriteProtection(RTC);
    LL_RTC_WAKEUP_Disable(RTC);
    while (!LL_RTC_IsActiveFlag_WUTW(RTC));
    if (LL_RCC_GetRTCClockSource() == LL_RCC_RTC_CLKSOURCE_LSE)
        LL_RTC_WAKEUP_SetAutoReload(RTC, ms * (LSE_VALUE / 1000 / 16));
    else
        LL_RTC_WAKEUP_SetAutoReload(RTC, ms * (LSI_VALUE / 1000 / 16));
    LL_RTC_EnableIT_WUT(RTC);
    LL_RTC_WAKEUP_Enable(RTC);
    LL_RTC_EnableWriteProtection(RTC);

    LL_RTC_ClearFlag_WUT(RTC);
    LL_EXTI_EnableIT_0_31(__RTC_EXTI);
    LL_EXTI_EnableRisingTrig_0_31(__RTC_EXTI);
    NVIC_SetPriority(__RTC_IRQn, 3);
    NVIC_EnableIRQ(__RTC_IRQn);
}

void rtc_wakeup_time_close(void)
{
    LL_EXTI_DisableIT_0_31(__RTC_EXTI);
    LL_EXTI_DisableRisingTrig_0_31(__RTC_EXTI);
    NVIC_DisableIRQ(__RTC_IRQn);
    LL_RTC_DisableWriteProtection(RTC);
    LL_RTC_DisableIT_WUT(RTC);
    LL_RTC_WAKEUP_Disable(RTC);
    LL_RTC_EnableWriteProtection(RTC);
}

static uint8_t rtc_bin2bcd(uint8_t value)
{
  uint32_t bcdhigh = 0U;

  while(value >= 10U)
  {
    bcdhigh++;
    value -= 10U;
  }

  return  ((uint8_t)(bcdhigh << 4U) | value);
}

static uint8_t rtc_bcd2bin(uint8_t value)
{
  uint32_t tmp;
  tmp = (((uint32_t)value & 0xF0U) >> 4U) * 10U;
  return (uint8_t)(tmp + ((uint32_t)value & 0x0FU));
}

void rtc_get_time(struct rtc_tm *t)
{
    t->hour = rtc_bcd2bin(LL_RTC_TIME_GetHour(RTC));
    t->min = rtc_bcd2bin(LL_RTC_TIME_GetMinute(RTC));
    t->sec = rtc_bcd2bin(LL_RTC_TIME_GetSecond(RTC));
    t->year = 2000 + rtc_bcd2bin(LL_RTC_DATE_GetYear(RTC));
    t->mon = rtc_bcd2bin(LL_RTC_DATE_GetMonth(RTC));
    t->mday = rtc_bcd2bin(LL_RTC_DATE_GetDay(RTC));
    t->wday = rtc_bcd2bin(LL_RTC_DATE_GetWeekDay(RTC));
    if(t->wday == 7)
        t->wday = 0;
}

void rtc_set_time(struct rtc_tm *t)
{
    if(t->wday == 0)
        t->wday = 7;
    LL_RTC_DisableWriteProtection(RTC);
    LL_RTC_EnableInitMode(RTC);
    while(0 == LL_RTC_IsActiveFlag_INIT(RTC));
    //必须先写时间，否则会出现日期延迟更新的情况
    LL_RTC_TIME_Config(RTC, LL_RTC_HOURFORMAT_24HOUR, rtc_bin2bcd(t->hour), rtc_bin2bcd(t->min), rtc_bin2bcd(t->sec));
    LL_RTC_DATE_Config(RTC, t->wday, rtc_bin2bcd(t->mday), rtc_bin2bcd(t->mon), rtc_bin2bcd(t->year - 2000));
    LL_RTC_DisableInitMode(RTC);
    LL_RTC_EnableWriteProtection(RTC);
}

#if defined (STM32G0)
void RTC_TAMP_IRQHandler(void)
{
    rtc_callback();
    LL_RTC_ClearFlag_WUT(RTC);
    LL_EXTI_ClearRisingFlag_0_31(__RTC_EXTI);
}
#elif defined (STM32L0) || defined (STM32F0)
void RTC_IRQHandler(void)
{
    if (rtc_callback)
        rtc_callback();
    LL_RTC_ClearFlag_WUT(RTC);
    LL_EXTI_ClearFlag_0_31(__RTC_EXTI);
}
#endif

#endif
