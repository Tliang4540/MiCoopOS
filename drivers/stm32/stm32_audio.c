/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#include <audio.h>
#include <stm32.h>
#include <pwm.h>
#include <timer.h>
#include <mcconfig.h>
#include <stddef.h>
#include <string.h>

#if defined(BSP_USING_PWM_AUDIO)

#if defined(STM32G0)
#define AUDIO_DMA_CHANNEL            LL_DMA_CHANNEL_1
#define AUDIO_DMA_IRQ                (9)
#if defined(AUDIO_USING_TIMER6)
#define AUDIO_DMA_REQ                LL_DMAMUX_REQ_TIM6_UP
#define AUDIO_TIM_CLK_ENABLE()       (RCC->APBENR1 |= (1 << 4))
#define AUDIO_TIM_CLK_DISABLE()      (RCC->APBENR1 &= ~(1 << 4))
#elif defined(AUDIO_USING_TIMER7)
#define AUDIO_DMA_REQ                LL_DMAMUX_REQ_TIM7_UP
#define AUDIO_TIM_CLK_ENABLE()       (RCC->APBENR1 |= (1 << 5))
#define AUDIO_TIM_CLK_DISABLE()      (RCC->APBENR1 &= ~(1 << 5))
#endif

#elif defined(STM32L0)
#if defined(AUDIO_USING_TIMER6)
#define AUDIO_DMA_IRQ                (10)
#define AUDIO_DMA_CHANNEL            LL_DMA_CHANNEL_2
#define AUDIO_DMA_REQ                LL_DMA_REQUEST_9
#define AUDIO_TIM_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 4))
#define AUDIO_TIM_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 4))
#elif defined(AUDIO_USING_TIMER7)
#define AUDIO_DMA_IRQ                (11)
#define AUDIO_DMA_CHANNEL            LL_DMA_CHANNEL_4
#define AUDIO_DMA_REQ                LL_DMA_REQUEST_15
#define AUDIO_TIM_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 5))
#define AUDIO_TIM_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 5))
#endif

#elif defined(STM32F0)
#if defined(AUDIO_USING_TIMER6)
#define AUDIO_DMA_IRQ                (10)
#define AUDIO_DMA_CHANNEL            LL_DMA_CHANNEL_3
#define AUDIO_DMA_REQ                (0)
#define AUDIO_TIM_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 4))
#define AUDIO_TIM_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 4))
#elif defined(AUDIO_USING_TIMER7)
#define AUDIO_DMA_IRQ                (11)
#define AUDIO_DMA_CHANNEL            LL_DMA_CHANNEL_4
#define AUDIO_DMA_REQ                (0)
#define AUDIO_TIM_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 5))
#define AUDIO_TIM_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 5))
#endif
#endif

#define AUDIO_DMA_CLK_ENABLE()      (RCC->AHBENR |= RCC_AHBENR_DMA1EN)
#define AUDIO_DMA_CLK_DISABLE()     (RCC->AHBENR &= ~RCC_AHBENR_DMA1EN)
#if defined(AUDIO_USING_TIMER6)
#define AUDIO_TIM              TIM6
#elif defined(AUDIO_USING_TIMER7)
#define AUDIO_TIM              TIM7
#endif

#define AUDIO_RATE              (14650)
#define AUDIO_BITS              (10)

struct audio_dev
{
    pwm_handle_t pwm;
    unsigned int channel;
    audio_callback_t callback;
    unsigned int index;
    unsigned short buffer[2][256];
};

static struct audio_dev audio;

void audio_init(unsigned int dac_id, unsigned int channel)
{
    uint32_t ccr_addr;
    unsigned int pwm_period;
    AUDIO_DMA_CLK_ENABLE();
    AUDIO_TIM_CLK_ENABLE();
    LL_TIM_SetAutoReload(AUDIO_TIM, SystemCoreClock / AUDIO_RATE - 1);
    LL_TIM_EnableDMAReq_UPDATE(AUDIO_TIM);

    audio.pwm = pwm_open(dac_id);
    audio.channel = channel;
    pwm_period = 1000000000 / (SystemCoreClock / (1 << AUDIO_BITS));
    pwm_set(audio.pwm, channel, pwm_period, pwm_period / 2);
    switch(channel)
    {
    case 0:
        ccr_addr = (uint32_t)&((TIM_TypeDef*)(*(uint32_t*)audio.pwm))->CCR1;
        break;
    case 1:
        ccr_addr = (uint32_t)&((TIM_TypeDef*)(*(uint32_t*)audio.pwm))->CCR2;
        break;
    case 2:
        ccr_addr = (uint32_t)&((TIM_TypeDef*)(*(uint32_t*)audio.pwm))->CCR3;
        break;
    default:
        ccr_addr = (uint32_t)&((TIM_TypeDef*)(*(uint32_t*)audio.pwm))->CCR4;
        break;
    }

    LL_DMA_SetPeriphRequest(DMA1, AUDIO_DMA_CHANNEL, AUDIO_DMA_REQ);
    LL_DMA_SetDataTransferDirection(DMA1, AUDIO_DMA_CHANNEL, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
    LL_DMA_SetChannelPriorityLevel(DMA1, AUDIO_DMA_CHANNEL, LL_DMA_PRIORITY_VERYHIGH);
    LL_DMA_SetMode(DMA1, AUDIO_DMA_CHANNEL, LL_DMA_MODE_CIRCULAR);
    LL_DMA_SetPeriphIncMode(DMA1, AUDIO_DMA_CHANNEL, LL_DMA_PERIPH_NOINCREMENT);
    LL_DMA_SetMemoryIncMode(DMA1, AUDIO_DMA_CHANNEL, LL_DMA_MEMORY_INCREMENT);
    LL_DMA_SetPeriphSize(DMA1, AUDIO_DMA_CHANNEL, LL_DMA_PDATAALIGN_HALFWORD);
    LL_DMA_SetMemorySize(DMA1, AUDIO_DMA_CHANNEL, LL_DMA_MDATAALIGN_HALFWORD);
    LL_DMA_ConfigAddresses(DMA1, AUDIO_DMA_CHANNEL, (uint32_t)audio.buffer, ccr_addr, LL_DMA_DIRECTION_MEMORY_TO_PERIPH);
    //开启DMA中断
    NVIC_SetPriority(AUDIO_DMA_IRQ, 2);
    NVIC_EnableIRQ(AUDIO_DMA_IRQ);
    LL_DMA_EnableIT_TC(DMA1, AUDIO_DMA_CHANNEL);
    LL_DMA_EnableIT_HT(DMA1, AUDIO_DMA_CHANNEL);
}

void audio_play_start(audio_callback_t callback)
{
    audio.callback = callback;
    callback(audio.buffer[1], sizeof(audio.buffer[1]));
    for (unsigned int i = 0; i < (sizeof(audio.buffer[0]) / sizeof(audio.buffer[0][0])); i++)
        audio.buffer[0][i] = (1 << AUDIO_BITS) / 2;

    LL_DMA_SetDataLength(DMA1, AUDIO_DMA_CHANNEL, sizeof(audio.buffer) / sizeof(audio.buffer[0][0]));
    LL_TIM_GenerateEvent_UPDATE(AUDIO_TIM);
    LL_DMA_EnableChannel(DMA1, AUDIO_DMA_CHANNEL);
    LL_TIM_EnableCounter(AUDIO_TIM);
    pwm_enable(audio.pwm, audio.channel);
}

void audio_play_stop(void)
{
    LL_TIM_DisableCounter(AUDIO_TIM);
    LL_DMA_DisableChannel(DMA1, AUDIO_DMA_CHANNEL);
    LL_DMA_SetDataLength(DMA1, AUDIO_DMA_CHANNEL, 0);
    pwm_disable(audio.pwm, audio.channel);
}

#if defined(STM32G0)
void DMA1_Channel1_IRQHandler(void)
{
    if(DMA1->ISR & DMA_ISR_HTIF1) 
    {
        audio.callback(audio.buffer[0], sizeof(audio.buffer[0]));
    }
    else if(DMA1->ISR & DMA_ISR_TCIF1)
    {
        audio.callback(audio.buffer[1], sizeof(audio.buffer[1]));
    }
    DMA1->IFCR = DMA_IFCR_CHTIF1 | DMA_IFCR_CTCIF1;
}
#elif defined(STM32L0) || defined(STM32F0)
#if defined(AUDIO_USING_TIMER6)
void DMA1_Channel2_3_IRQHandler(void)
{
#if defined(STM32F0)
    if(DMA1->ISR & DMA_ISR_HTIF3) 
    {
        audio.callback(audio.buffer[0], sizeof(audio.buffer[0]));
    }
    else if(DMA1->ISR & DMA_ISR_TCIF3)
    {
        audio.callback(audio.buffer[1], sizeof(audio.buffer[1]));
    }
    DMA1->IFCR = DMA_IFCR_CHTIF3 | DMA_IFCR_CTCIF3;
#else
    if(DMA1->ISR & DMA_ISR_HTIF2) 
    {
        audio.callback(audio.buffer[0], sizeof(audio.buffer[0]));
    }
    else if(DMA1->ISR & DMA_ISR_TCIF2)
    {
        audio.callback(audio.buffer[1], sizeof(audio.buffer[1]));
    }
    DMA1->IFCR = DMA_IFCR_CHTIF2 | DMA_IFCR_CTCIF2;
#endif
}
#elif defined(AUDIO_USING_TIMER7)
void DMA1_Channel4_5_6_7_IRQHandler(void)
{
    if(DMA1->ISR & DMA_ISR_HTIF4) 
    {
        audio.callback(audio.buffer[0], sizeof(audio.buffer[0]));
    }
    else if(DMA1->ISR & DMA_ISR_TCIF4)
    {
        audio.callback(audio.buffer[1], sizeof(audio.buffer[1]));
    }
    DMA1->IFCR = DMA_IFCR_CHTIF4 | DMA_IFCR_CTCIF4;
}
#endif
#endif

#endif
