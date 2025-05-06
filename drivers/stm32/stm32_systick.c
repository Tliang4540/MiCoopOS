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
