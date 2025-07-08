/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#include <systick.h>
#include <stm32.h>

static void (*systick_callback)(void);

void SysTick_Handler(void)
{
    systick_callback();
}

void systick_init(unsigned int period, void (*callback)(void))
{
    systick_callback = callback;
    SysTick_Config(period * (SystemCoreClock / 1000000));
}

void systick_stop(void)
{
   SysTick->CTRL &= ~(SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk);
}

void systick_start(void)
{
   SysTick->CTRL |= (SysTick_CTRL_ENABLE_Msk | SysTick_CTRL_TICKINT_Msk);
}
