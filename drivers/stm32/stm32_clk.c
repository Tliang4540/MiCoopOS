/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#include <clk.h>
#include <stm32.h>

void clk_init(void)
{
#if defined(STM32G0)
    SET_BIT(RCC->APBENR2, LL_APB2_GRP1_PERIPH_SYSCFG);
    SET_BIT(RCC->APBENR1, LL_APB1_GRP1_PERIPH_PWR);
#if defined(SYSCFG_CFGR1_UCPD1_STROBE) || defined(SYSCFG_CFGR1_UCPD2_STROBE)
    LL_SYSCFG_DisableDBATT(LL_SYSCFG_UCPD1_STROBE | LL_SYSCFG_UCPD2_STROBE);
#endif

    LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);

    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLLM_DIV_1, 8, LL_RCC_PLLR_DIV_2);
    LL_RCC_PLL_Enable();
    LL_RCC_PLL_EnableDomain_SYS();
    while(!LL_RCC_PLL_IsReady());
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

    SystemCoreClock = 64000000;

    RCC->IOPENR = LL_IOP_GRP1_PERIPH_ALL;

#if defined(LPUART1)
    LL_RCC_SetLPUARTClockSource(LL_RCC_LPUART1_CLKSOURCE_HSI);
#endif
#if defined(LPUART2)
    LL_RCC_SetLPUARTClockSource(LL_RCC_LPUART2_CLKSOURCE_HSI);
#endif

#elif defined(STM32L0)
    SET_BIT(RCC->APB2ENR, LL_APB2_GRP1_PERIPH_SYSCFG);
    SET_BIT(RCC->APB1ENR, LL_APB1_GRP1_PERIPH_PWR);

    LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);
    LL_RCC_HSI_Enable();
    LL_PWR_SetRegulVoltageScaling(LL_PWR_REGU_VOLTAGE_SCALE1);
    while(!LL_RCC_HSI_IsReady());
    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI, LL_RCC_PLL_MUL_4, LL_RCC_PLL_DIV_2);
    LL_RCC_PLL_Enable();
    while(!LL_RCC_PLL_IsReady());
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    SystemCoreClock = 32000000;

    RCC->IOPENR = LL_IOP_GRP1_PERIPH_ALL;
#if defined(LPUART1)
    LL_RCC_SetLPUARTClockSource(LL_RCC_LPUART1_CLKSOURCE_HSI);
#endif

#elif defined(STM32F0)
    SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN);
    SET_BIT(RCC->APB1ENR, RCC_APB1ENR_PWREN);
    
    LL_FLASH_SetLatency(LL_FLASH_LATENCY_1);

    LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSI_DIV_2, LL_RCC_PLL_MUL_12);
    LL_RCC_PLL_Enable();
    while (!LL_RCC_PLL_IsReady());
    LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);
    SystemCoreClock = 48000000;

    RCC->IOPENR = LL_IOP_GRP1_PERIPH_ALL;

#endif
}

void clk_lsc_init(clk_lsc_t lsc)
{
    if (lsc == CLK_LSC_EXTERNAL)
    {
        LL_PWR_EnableBkUpAccess();
        LL_RCC_LSE_Enable();
        while (!LL_RCC_LSE_IsReady());
        LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSE);
    }
    else 
    {
        LL_RCC_SetRTCClockSource(LL_RCC_RTC_CLKSOURCE_LSI);
    }
}
