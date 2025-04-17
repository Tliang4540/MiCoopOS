/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#include <adc.h>
#include <stm32.h>
#include <mcconfig.h>
#include <log.h>

#if defined(BSP_USING_ADC1)

#if defined(STM32G0)
#define __ADC1_CLK_ENABLE()       (RCC->APBENR2 |= (1 << 20))
#define __ADC1_CLK_DISABLE()      (RCC->APBENR2 &= ~(1 << 20))
#elif defined (STM32L0) || defined (STM32F0)
#define __ADC1_CLK_ENABLE()       (RCC->APB2ENR |= (1 << 9))
#define __ADC1_CLK_DISABLE()      (RCC->APB2ENR &= ~(1 << 9))
#endif

struct stm32_adc
{
    ADC_TypeDef *adc;
};

enum
{
#if defined(ADC1) && defined(BSP_USING_ADC1)
    ADC1_INDEX,
#endif
    ADC_INDEX_MAX
};

static struct stm32_adc adc_list[] = 
{
#if defined(ADC1) && defined(BSP_USING_ADC1)
    {ADC1},
#endif
};

static void adc_clk_enable(unsigned int adcid)
{
    switch (adcid)
    {
#if defined(ADC1) && defined(BSP_USING_ADC1)
    case ADC1_INDEX:
        __ADC1_CLK_ENABLE();
        break;
#endif
    default:
        break;
    }
}

adc_handle_t adc_open(unsigned int adcid)
{
    struct stm32_adc *adc;

    LOG_ASSERT(adcid < ADC_INDEX_MAX);

    adc = &adc_list[adcid];
    if (adc->adc->CR)
        return adc;

    adc_clk_enable(adcid);
    // 效准
    adc->adc->CR = ADC_CR_ADVREGEN;
    for (volatile unsigned int i = 0; i < 1000; i++);   //必须加入延时，否则会死机
    adc->adc->CR |= ADC_CR_ADCAL;
    while (adc->adc->CR & ADC_CR_ADCAL);
    // 配置
    adc->adc->CFGR2 = ADC_CFGR2_CKMODE_0;
    adc->adc->SMPR  = 4;    //19.5 clock cycles
    adc->adc->CR    |= ADC_CR_ADEN;    //ADC enable

    return adc;
}

unsigned int adc_read(adc_handle_t adc)
{
    struct stm32_adc *p;
    LOG_ASSERT(adc != 0);
    p = adc;

    p->adc->CR |= ADC_CR_ADSTART;

    while (!(p->adc->ISR & ADC_ISR_EOC));

    return p->adc->DR * 3300ul / 4096ul;
}

void adc_set_channel(adc_handle_t adc, unsigned int channel)
{
    struct stm32_adc *p;
    LOG_ASSERT(adc != 0);
    p = adc;

    p->adc->CHSELR = 1 << channel;
}

void adc_close(adc_handle_t adc)
{
    struct stm32_adc *p;
    LOG_ASSERT(adc != 0);
    p = adc;

    p->adc->CR |= ADC_CR_ADDIS;
}

#endif
