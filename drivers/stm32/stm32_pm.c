/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#include <stm32.h>
#include <pm.h>

void pm_set_mode(pm_mode_t mode)
{
    switch (mode)
    {
    case PM_MODE_RUN:
        LL_RCC_PLL_Enable();
        SCB->SCR &= ~SCB_SCR_SLEEPDEEP_Msk;
        while(!LL_RCC_PLL_IsReady());
        LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
        break;
    case PM_MODE_SLEEP:
        #if defined(STM32G0)
        LL_PWR_SetPowerMode(LL_PWR_MODE_STOP1);
        #elif defined(STM32L0)
        PWR->CR |= PWR_CR_FWU | PWR_CR_ULP | PWR_CR_LPSDSR;
        RCC->CFGR |= RCC_CFGR_STOPWUCK;
        #elif defined(STM32F0)
        PWR->CR |= PWR_CR_LPDS;
        #endif
        SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
        break;
    case PM_MODE_STANDBY:
        #if defined(STM32G0)
        LL_PWR_SetPowerMode(LL_PWR_MODE_STANDBY);
        #elif defined(STM32L0)
        PWR->CR |= PWR_CR_PDDS;
        #elif defined(STM32F0)
        PWR->CR |= PWR_CR_PDDS;
        #endif
        SCB->SCR |= SCB_SCR_SLEEPDEEP_Msk;
        break;
    default:
        break;
    }
}

void pm_enter_sleep(void)
{
    __WFI();
}
