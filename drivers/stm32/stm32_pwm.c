/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#include <pwm.h>
#include <stm32.h>
#include <mcconfig.h>
#include <log.h>

#if defined(BSP_USING_PWM1) || defined(BSP_USING_PWM2) || defined(BSP_USING_PWM3) \
 || defined(BSP_USING_PWM4) || defined(BSP_USING_PWM14) || defined(BSP_USING_PWM15) \
 || defined(BSP_USING_PWM16) || defined(BSP_USING_PWM17) || defined(BSP_USING_PWM21) \
 || defined(BSP_USING_PWM22)

#if defined(STM32G0)
#define __PWM1_CLK_ENABLE()       (RCC->APBENR2 |= (1 << 11))
#define __PWM1_CLK_DISABLE()      (RCC->APBENR2 &= ~(1 << 11))
#define __PWM2_CLK_ENABLE()       (RCC->APBENR1 |= (1 << 0))
#define __PWM2_CLK_DISABLE()      (RCC->APBENR1 &= ~(1 << 0))
#define __PWM3_CLK_ENABLE()       (RCC->APBENR1 |= (1 << 1))
#define __PWM3_CLK_DISABLE()      (RCC->APBENR1 &= ~(1 << 1))
#define __PWM4_CLK_ENABLE()       (RCC->APBENR1 |= (1 << 2))
#define __PWM4_CLK_DISABLE()      (RCC->APBENR1 &= ~(1 << 2))
#define __PWM14_CLK_ENABLE()      (RCC->APBENR2 |= (1 << 15))
#define __PWM14_CLK_DISABLE()     (RCC->APBENR2 &= ~(1 << 15))
#define __PWM15_CLK_ENABLE()      (RCC->APBENR2 |= (1 << 16))
#define __PWM15_CLK_DISABLE()     (RCC->APBENR2 &= ~(1 << 16))
#define __PWM16_CLK_ENABLE()      (RCC->APBENR2 |= (1 << 17))
#define __PWM16_CLK_DISABLE()     (RCC->APBENR2 &= ~(1 << 17))
#define __PWM17_CLK_ENABLE()      (RCC->APBENR2 |= (1 << 18))
#define __PWM17_CLK_DISABLE()     (RCC->APBENR2 &= ~(1 << 18))
#elif defined (STM32L0)
#define __PWM2_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 0))
#define __PWM2_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 0))
#define __PWM3_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 1))
#define __PWM3_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 1))
#define __PWM21_CLK_ENABLE()      (RCC->APB2ENR |= (1 << 2))
#define __PWM21_CLK_DISABLE()     (RCC->APB2ENR &= ~(1 << 2))
#define __PWM22_CLK_ENABLE()      (RCC->APB2ENR |= (1 << 5))
#define __PWM22_CLK_DISABLE()     (RCC->APB2ENR &= ~(1 << 5))
#elif defined (STM32F0)
#define __PWM1_CLK_ENABLE()       (RCC->APB2ENR |= (1 << 11))
#define __PWM1_CLK_DISABLE()      (RCC->APB2ENR &= ~(1 << 11))
#define __PWM2_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 0))
#define __PWM2_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 0))
#define __PWM3_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 1))
#define __PWM3_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 1))
#define __PWM14_CLK_ENABLE()      (RCC->APB1ENR |= (1 << 8))
#define __PWM14_CLK_DISABLE()     (RCC->APB1ENR &= ~(1 << 8))
#define __PWM15_CLK_ENABLE()      (RCC->APB2ENR |= (1 << 16))
#define __PWM15_CLK_DISABLE()     (RCC->APB2ENR &= ~(1 << 16))
#define __PWM16_CLK_ENABLE()      (RCC->APB2ENR |= (1 << 17))
#define __PWM16_CLK_DISABLE()     (RCC->APB2ENR &= ~(1 << 17))
#define __PWM17_CLK_ENABLE()      (RCC->APB2ENR |= (1 << 18))
#define __PWM17_CLK_DISABLE()     (RCC->APB2ENR &= ~(1 << 18))
#endif

struct stm32_pwm
{
    TIM_TypeDef *tim;
};

enum
{
#if defined(TIM1) && defined(BSP_USING_PWM1)
    PWM1_INDEX,
#endif
#if defined(TIM2) && defined(BSP_USING_PWM2)
    PWM2_INDEX,
#endif
#if defined(TIM3) && defined(BSP_USING_PWM3)
    PWM3_INDEX,
#endif
#if defined(TIM4) && defined(BSP_USING_PWM4)
    PWM4_INDEX,
#endif
#if defined(TIM14) && defined(BSP_USING_PWM14)
    PWM14_INDEX,
#endif
#if defined(TIM15) && defined(BSP_USING_PWM15)
    PWM15_INDEX,
#endif
#if defined(TIM16) && defined(BSP_USING_PWM16)
    PWM16_INDEX,
#endif
#if defined(TIM17) && defined(BSP_USING_PWM17)
    PWM17_INDEX,
#endif
#if defined(TIM21) && defined(BSP_USING_PWM21)
    PWM21_INDEX,
#endif
#if defined(TIM22) && defined(BSP_USING_PWM22)
    PWM22_INDEX,
#endif
    PWM_INDEX_MAX
};

static struct stm32_pwm pwm_list[] = 
{
#if defined(TIM1) && defined(BSP_USING_PWM1)
    {TIM1},
#endif
#if defined(TIM2) && defined(BSP_USING_PWM2)
    {TIM2},
#endif
#if defined(TIM3) && defined(BSP_USING_PWM3)
    {TIM3},
#endif
#if defined(TIM4) && defined(BSP_USING_PWM4)
    {TIM4},
#endif
#if defined(TIM14) && defined(BSP_USING_PWM14)
    {TIM14},
#endif
#if defined(TIM15) && defined(BSP_USING_PWM15)
    {TIM15},
#endif
#if defined(TIM16) && defined(BSP_USING_PWM16)
    {TIM16},
#endif
#if defined(TIM17) && defined(BSP_USING_PWM17)
    {TIM17},
#endif
#if defined(TIM21) && defined(BSP_USING_PWM21)
    {PWM21},
#endif
#if defined(TIM22) && defined(BSP_USING_PWM22)
    {PWM22},
#endif
};

static void pwm_clk_enable(unsigned int pwmid)
{
    switch (pwmid)
    {
#if defined(TIM1) && defined(BSP_USING_PWM1)
    case PWM1_INDEX:
        __PWM1_CLK_ENABLE();
        break;
#endif
#if defined(TIM2) && defined(BSP_USING_PWM2)
    case PWM2_INDEX:
        __PWM2_CLK_ENABLE();
        break;
#endif
#if defined(TIM3) && defined(BSP_USING_PWM3)
    case PWM3_INDEX:
        __PWM3_CLK_ENABLE();
        break;
#endif
#if defined(TIM4) && defined(BSP_USING_PWM4)
    case PWM4_INDEX:
        __PWM4_CLK_ENABLE();
        break;
#endif
#if defined(TIM14) && defined(BSP_USING_PWM14)
    case PWM14_INDEX:
        __PWM14_CLK_ENABLE();
        break;
#endif
#if defined(TIM15) && defined(BSP_USING_PWM15)
    case PWM15_INDEX:
        __PWM15_CLK_ENABLE();
        break;
#endif
#if defined(TIM16) && defined(BSP_USING_PWM16)
    case PWM16_INDEX:
        __PWM16_CLK_ENABLE();
        break;
#endif
#if defined(TIM17) && defined(BSP_USING_PWM17)
    case PWM17_INDEX:
        __PWM17_CLK_ENABLE();
        break;
#endif
#if defined(TIM21) && defined(BSP_USING_PWM21)
    case PWM21_INDEX:
        __PWM21_CLK_ENABLE();
        break;
#endif
#if defined(TIM22) && defined(BSP_USING_PWM22)
    case PWM22_INDEX:
        __PWM22_CLK_ENABLE();
        break;
#endif
    default:
        break;
    }
}

static int stm32_pwm_open(pwm_device_t *dev)
{
    struct stm32_pwm *pwm = dev->user_data;

    pwm->tim->EGR = 1;
    pwm->tim->SR = 0;
#if defined(STM32G0) || defined(STM32F0)
    pwm->tim->BDTR = TIM_BDTR_MOE;
#endif
    pwm->tim->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE;

    return 0;
}

static int stm32_pwm_set_period(pwm_device_t *dev, unsigned int period)
{
    struct stm32_pwm *pwm = dev->user_data;
    unsigned int psc;
    unsigned int tim_clock = SystemCoreClock / 1000000UL;

    period = period * tim_clock / 1000UL;
    psc = period / 65535 + 1;
    period = period / psc - 1;

    pwm->tim->PSC = psc - 1;
    pwm->tim->ARR = period;
    return 0;
}

static int stm32_pwm_set_channel_pulse(pwm_device_t *dev, unsigned int channel, unsigned int pulse)
{
    struct stm32_pwm *pwm = dev->user_data;
    unsigned int psc;
    unsigned int tim_clock = SystemCoreClock / 1000000UL;

    psc = pwm->tim->PSC + 1;
    pulse = pulse * tim_clock / psc / 1000ULL;

    switch (channel)
    {
    case 0:
        pwm->tim->CCR1 = pulse;
        break;
    case 1:
        pwm->tim->CCR2 = pulse;
        break;
    case 2: 
        pwm->tim->CCR3 = pulse;
        break;
    default:
        pwm->tim->CCR4 = pulse;
        break;
    }
    return 0;
}

static int stm32_pwm_enable_channel(pwm_device_t *dev, unsigned int channel)
{
    struct stm32_pwm *pwm = dev->user_data;
    switch (channel)
    {
    case 0:
        pwm->tim->CCMR1 |= TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
        pwm->tim->CCER |= TIM_CCER_CC1E;
        break;
    case 1:
        pwm->tim->CCMR1 |= TIM_CCMR1_OC2PE | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1;
        pwm->tim->CCER |= TIM_CCER_CC2E;
        break;
    case 2:
        pwm->tim->CCMR2 |= TIM_CCMR2_OC3PE | TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1;
        pwm->tim->CCER |= TIM_CCER_CC3E;
        break;
    case 3:
        pwm->tim->CCMR2 |= TIM_CCMR2_OC4PE | TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1;
        pwm->tim->CCER |= TIM_CCER_CC4E;
        break;
    default:
        break;
    }
    return 0;
}

static int stm32_pwm_disable_channel(pwm_device_t *dev, unsigned int channel)
{
    struct stm32_pwm *pwm = dev->user_data;
    switch (channel)
    {
    case 0:
        pwm->tim->CCER &= ~TIM_CCER_CC1E;
        pwm->tim->CCMR1 &= ~(TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1);
        break;
    case 1:
        pwm->tim->CCER &= ~TIM_CCER_CC2E;
        pwm->tim->CCMR1 &= ~(TIM_CCMR1_OC2PE | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1);
        break;
    case 2:
        pwm->tim->CCER &= ~TIM_CCER_CC3E;
        pwm->tim->CCMR2 &= ~(TIM_CCMR2_OC3PE | TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1);
        break;
    case 3:
        pwm->tim->CCER &= ~TIM_CCER_CC4E;
        pwm->tim->CCMR2 &= ~(TIM_CCMR2_OC4PE | TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1);
        break;
    default:
        break;
    }
    return 0;
}

static int stm32_pwm_close(pwm_device_t *dev)
{
    struct stm32_pwm *pwm = dev->user_data;

    pwm->tim->CR1 = 0;
    pwm->tim->SR = 0;
    pwm->tim->CCER = 0;
    pwm->tim->CCMR1 = 0;
    pwm->tim->CCMR2 = 0;
#if defined(STM32G0) || defined(STM32F0)
    pwm->tim->BDTR = 0;
#endif
    return 0;
}

static const struct pwm_device_ops stm32_pwm_ops = 
{
    .open = stm32_pwm_open,
    .close = stm32_pwm_close,
    .set_period = stm32_pwm_set_period,
    .set_channel_pulse = stm32_pwm_set_channel_pulse,
    .enable_channel = stm32_pwm_enable_channel,
    .disable_channel = stm32_pwm_disable_channel
};

void pwm_device_init(pwm_device_t *dev, unsigned int pwm_id)
{
    LOG_ASSERT(pwm_id < PWM_INDEX_MAX);

    pwm_clk_enable(pwm_id);
    dev->ops = &stm32_pwm_ops;
    dev->user_data = &pwm_list[pwm_id];
}

#endif

