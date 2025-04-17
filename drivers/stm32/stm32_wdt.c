/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#include <wdt.h>
#include <stm32.h>
#include <mcconfig.h>

#if defined(BSP_USING_WDT)

void wdt_init(unsigned int period)
{
    LL_IWDG_Enable(IWDG);
    LL_IWDG_EnableWriteAccess(IWDG);
    LL_IWDG_SetPrescaler(IWDG, LL_IWDG_PRESCALER_256);
    LL_IWDG_SetReloadCounter(IWDG, period * (LSE_VALUE / 1000) / 256);
}

void wdt_reset(void)
{
    LL_IWDG_ReloadCounter(IWDG);
}

#endif
