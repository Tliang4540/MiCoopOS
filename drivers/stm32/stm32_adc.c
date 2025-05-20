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

static int stm32_adc_open(adc_device_t *dev)
{
    struct stm32_adc *adc = dev->user_data;
    // 效准
    adc->adc->CR = ADC_CR_ADVREGEN;
    for (volatile unsigned int i = 0; i < 1000; i++);   //必须加入延时，否则会死机
    adc->adc->CR |= ADC_CR_ADCAL;
    while (adc->adc->CR & ADC_CR_ADCAL);
    // 配置
    adc->adc->CFGR2 = ADC_CFGR2_CKMODE_0;
    adc->adc->SMPR  = 4;    //19.5 clock cycles
    adc->adc->CR    |= ADC_CR_ADEN;    //ADC enable
    return 0;
}

static int stm32_adc_read(adc_device_t *dev)
{
    struct stm32_adc *adc = dev->user_data;

    adc->adc->CR |= ADC_CR_ADSTART;
    while (!(adc->adc->ISR & ADC_ISR_EOC));

    return adc->adc->DR * 3300ul / 4096ul;
}

static int stm32_adc_set_channel(adc_device_t *dev, unsigned int channel)
{
    struct stm32_adc *adc = dev->user_data;
    adc->adc->CHSELR = 1 << channel;
    return 0;
}

static int stm32_adc_close(adc_device_t *dev)
{
    struct stm32_adc *adc = dev->user_data;
    adc->adc->CR |= ADC_CR_ADDIS;
    return 0;
}

static const struct adc_device_ops stm32_adc_ops = {
    .open = stm32_adc_open,
    .close = stm32_adc_close,
    .read = stm32_adc_read,
    .set_channel = stm32_adc_set_channel
};

void adc_device_init(adc_device_t *dev, unsigned int adc_id)
{ 
    LOG_ASSERT(adc_id < ADC_INDEX_MAX);

    adc_clk_enable(adc_id);
    dev->ops = &stm32_adc_ops;
    dev->user_data = &adc_list[adc_id];
}

#endif
