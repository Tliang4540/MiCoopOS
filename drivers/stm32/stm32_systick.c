/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#include <systick.h>
#include <stm32.h>

#ifndef BSP_SYSTICK_PER_SECOND
#define BSP_SYSTICK_PER_SECOND 1000
#endif

static void (*systick_hdr)(void);

void SysTick_Handler(void)
{
    systick_hdr();
}

void systick_init(void (*hdr)(void))
{
    systick_hdr = hdr;
    SysTick_Config(SystemCoreClock / BSP_SYSTICK_PER_SECOND);
}
