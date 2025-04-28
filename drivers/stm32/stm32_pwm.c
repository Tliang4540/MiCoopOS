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

void pwm_set(pwm_handle_t pwm, unsigned int channel, unsigned int period, unsigned int pulse)
{
    LOG_ASSERT(pwm != 0);
    LOG_ASSERT(channel < 4);

    struct stm32_pwm *p = pwm;
    unsigned int psc;
    unsigned int tim_clock = SystemCoreClock / 1000000UL;

    period = period * tim_clock / 1000UL;
    psc = period / 65535 + 1;
    period = period / psc - 1;
    pulse = pulse * tim_clock / psc / 1000ULL;

    p->tim->PSC = psc - 1;
    p->tim->ARR = period;

    switch (channel)
    {
    case 0:
        p->tim->CCR1 = pulse;
        p->tim->CCMR1 |= TIM_CCMR1_OC1PE | TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
        break;
    case 1:
        p->tim->CCR2 = pulse;
        p->tim->CCMR1 |= TIM_CCMR1_OC2PE | TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1;
        break;
    case 2: 
        p->tim->CCR3 = pulse;
        p->tim->CCMR2 |= TIM_CCMR2_OC3PE | TIM_CCMR2_OC3M_2 | TIM_CCMR2_OC3M_1;
        break;
    default:
        p->tim->CCR4 = pulse;
        p->tim->CCMR2 |= TIM_CCMR2_OC4PE | TIM_CCMR2_OC4M_2 | TIM_CCMR2_OC4M_1;
        break;
    }
}

void pwm_set_pulse(pwm_handle_t pwm, unsigned int channel, unsigned int pulse)
{
    LOG_ASSERT(pwm != 0);
    LOG_ASSERT(channel < 4);

    struct stm32_pwm *p = pwm;
    unsigned int psc;
    unsigned int tim_clock = SystemCoreClock / 1000000UL;

    psc = p->tim->PSC + 1;
    pulse = pulse * tim_clock / psc / 1000ULL;

    switch (channel)
    {
    case 0:
        p->tim->CCR1 = pulse;
        break;
    case 1:
        p->tim->CCR2 = pulse;
        break;
    case 2: 
        p->tim->CCR3 = pulse;
        break;
    default:
        p->tim->CCR4 = pulse;
        break;
    }
}

pwm_handle_t pwm_open(unsigned int pwmid)
{
    struct stm32_pwm *pwm;

    LOG_ASSERT(pwmid < PWM_INDEX_MAX);

    pwm = &pwm_list[pwmid];

    pwm_clk_enable(pwmid);

    pwm->tim->EGR = 1;
    pwm->tim->SR = 0;
#if defined(STM32G0) || defined(STM32F0)
    pwm->tim->BDTR = TIM_BDTR_MOE;
#endif
    return pwm;
}

void pwm_enable(pwm_handle_t pwm, unsigned int channel)
{
    LOG_ASSERT(pwm != 0);
    LOG_ASSERT(channel < 4);

    struct stm32_pwm *p = pwm;

    switch (channel)
    {
    case 0:
        p->tim->CCER |= TIM_CCER_CC1E;
        break;
    case 1:
        p->tim->CCER |= TIM_CCER_CC2E;
        break;
    case 2:
        p->tim->CCER |= TIM_CCER_CC3E;
        break;
    case 3:
        p->tim->CCER |= TIM_CCER_CC4E;
        break;
    default:
        break;
    }

    p->tim->CR1 = TIM_CR1_CEN;
}

void pwm_disable(pwm_handle_t pwm, unsigned int channel)
{
    LOG_ASSERT(pwm != 0);
    LOG_ASSERT(channel < 4);

    struct stm32_pwm *p = pwm;

    switch (channel)
    {
    case 0:
        p->tim->CCER &= ~TIM_CCER_CC1E;
        break;
    case 1:
        p->tim->CCER &= ~TIM_CCER_CC2E;
        break;
    case 2:
        p->tim->CCER &= ~TIM_CCER_CC3E;
        break;
    case 3:
        p->tim->CCER &= ~TIM_CCER_CC4E;
        break;
    default:
        break;
    }
}

void pwm_close(pwm_handle_t pwm)
{
    LOG_ASSERT(pwm != 0);

    struct stm32_pwm *p = pwm;

    p->tim->CR1 = 0;
    p->tim->SR = 0;
    p->tim->CCER = 0;
#if defined(STM32G0) || defined(STM32F0)
    p->tim->BDTR = 0;
#endif
}

#endif

