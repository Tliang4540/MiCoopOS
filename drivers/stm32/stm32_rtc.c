/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#include <rtc.h>
#include <stm32.h>
#include <log.h>
#include <mcconfig.h>
#include <mclib.h>

#if defined(BSP_USING_RTC)

#define __RTC_IRQn                (2)

#if defined(STM32G0)
#define __RTC_EXTI                (LL_EXTI_LINE_19)
#elif defined (STM32L0) || defined (STM32F0)
#define __RTC_EXTI              (LL_EXTI_LINE_20)
#endif

static void (*rtc_callback)(void);

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

void rtc_get_time(struct mc_tm *tm)
{
    tm->hour = mc_bcd2bin(LL_RTC_TIME_GetHour(RTC));
    tm->min = mc_bcd2bin(LL_RTC_TIME_GetMinute(RTC));
    tm->sec = mc_bcd2bin(LL_RTC_TIME_GetSecond(RTC));
    tm->year = 2000 + mc_bcd2bin(LL_RTC_DATE_GetYear(RTC));
    tm->mon = mc_bcd2bin(LL_RTC_DATE_GetMonth(RTC));
    tm->mday = mc_bcd2bin(LL_RTC_DATE_GetDay(RTC));
    tm->wday = mc_bcd2bin(LL_RTC_DATE_GetWeekDay(RTC));
    if(tm->wday == 7)
        tm->wday = 0;
}

void rtc_set_time(struct mc_tm *tm)
{
    if(tm->wday == 0)
        tm->wday = 7;
    LL_RTC_DisableWriteProtection(RTC);
    LL_RTC_EnableInitMode(RTC);
    while(0 == LL_RTC_IsActiveFlag_INIT(RTC));
    //必须先写时间，否则会出现日期延迟更新的情况
    LL_RTC_TIME_Config(RTC, LL_RTC_HOURFORMAT_24HOUR, mc_bin2bcd(tm->hour), mc_bin2bcd(tm->min), mc_bin2bcd(tm->sec));
    LL_RTC_DATE_Config(RTC, tm->wday, mc_bin2bcd(tm->mday), mc_bin2bcd(tm->mon), mc_bin2bcd(tm->year - 2000));
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
