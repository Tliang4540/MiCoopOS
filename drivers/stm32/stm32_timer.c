/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#include <timer.h>
#include <stm32.h>
#include <mcconfig.h>
#include <log.h>

#if defined(BSP_USING_TIMER1) || defined(BSP_USING_TIMER2) || defined(BSP_USING_TIMER3) \
 || defined(BSP_USING_TIMER4) || defined(BSP_USING_TIMER6) || defined(BSP_USING_TIMER7) \
 || defined(BSP_USING_TIMER14) || defined(BSP_USING_TIMER15) || defined(BSP_USING_TIMER16) \
 || defined(BSP_USING_TIMER17) || defined(BSP_USING_TIMER21) || defined(BSP_USING_TIMER22)

#if defined(STM32G0)
#define __TIM1_IRQn               (13)
#define __TIM2_IRQn               (15)
#define __TIM3_IRQn               (16)
#define __TIM4_IRQn               (16)
#define __TIM6_IRQn               (17)
#define __TIM7_IRQn               (18)
#define __TIM14_IRQn              (19)
#define __TIM15_IRQn              (20)
#define __TIM16_IRQn              (21)
#define __TIM17_IRQn              (22)

#define __TIM1_CLK_ENABLE()       (RCC->APBENR2 |= (1 << 11))
#define __TIM1_CLK_DISABLE()      (RCC->APBENR2 &= ~(1 << 11))
#define __TIM2_CLK_ENABLE()       (RCC->APBENR1 |= (1 << 0))
#define __TIM2_CLK_DISABLE()      (RCC->APBENR1 &= ~(1 << 0))
#define __TIM3_CLK_ENABLE()       (RCC->APBENR1 |= (1 << 1))
#define __TIM3_CLK_DISABLE()      (RCC->APBENR1 &= ~(1 << 1))
#define __TIM4_CLK_ENABLE()       (RCC->APBENR1 |= (1 << 2))
#define __TIM4_CLK_DISABLE()      (RCC->APBENR1 &= ~(1 << 2))
#define __TIM6_CLK_ENABLE()       (RCC->APBENR1 |= (1 << 4))
#define __TIM6_CLK_DISABLE()      (RCC->APBENR1 &= ~(1 << 4))
#define __TIM7_CLK_ENABLE()       (RCC->APBENR1 |= (1 << 5))
#define __TIM7_CLK_DISABLE()      (RCC->APBENR1 &= ~(1 << 5))
#define __TIM14_CLK_ENABLE()      (RCC->APBENR2 |= (1 << 15))
#define __TIM14_CLK_DISABLE()     (RCC->APBENR2 &= ~(1 << 15))
#define __TIM15_CLK_ENABLE()      (RCC->APBENR2 |= (1 << 16))
#define __TIM15_CLK_DISABLE()     (RCC->APBENR2 &= ~(1 << 16))
#define __TIM16_CLK_ENABLE()      (RCC->APBENR2 |= (1 << 17))
#define __TIM16_CLK_DISABLE()     (RCC->APBENR2 &= ~(1 << 17))
#define __TIM17_CLK_ENABLE()      (RCC->APBENR2 |= (1 << 18))
#define __TIM17_CLK_DISABLE()     (RCC->APBENR2 &= ~(1 << 18))
#elif defined (STM32L0)
#define __TIM2_IRQn               (15)
#define __TIM3_IRQn               (16)
#define __TIM6_IRQn               (17)
#define __TIM7_IRQn               (18)
#define __TIM21_IRQn              (20)
#define __TIM22_IRQn              (22)

#define __TIM2_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 0))
#define __TIM2_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 0))
#define __TIM3_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 1))
#define __TIM3_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 1))
#define __TIM6_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 4))
#define __TIM6_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 4))
#define __TIM7_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 5))
#define __TIM7_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 5))
#define __TIM21_CLK_ENABLE()      (RCC->APB2ENR |= (1 << 2))
#define __TIM21_CLK_DISABLE()     (RCC->APB2ENR &= ~(1 << 2))
#define __TIM22_CLK_ENABLE()      (RCC->APB2ENR |= (1 << 5))
#define __TIM22_CLK_DISABLE()     (RCC->APB2ENR &= ~(1 << 5))
#elif defined (STM32F0)
#define __TIM1_IRQn               (13)
#define __TIM2_IRQn               (15)
#define __TIM3_IRQn               (16)
#define __TIM6_IRQn               (17)
#define __TIM7_IRQn               (18)
#define __TIM14_IRQn              (19)
#define __TIM15_IRQn              (20)
#define __TIM16_IRQn              (21)
#define __TIM17_IRQn              (22)

#define __TIM1_CLK_ENABLE()       (RCC->APB2ENR |= (1 << 11))
#define __TIM1_CLK_DISABLE()      (RCC->APB2ENR &= ~(1 << 11))
#define __TIM2_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 0))
#define __TIM2_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 0))
#define __TIM3_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 1))
#define __TIM3_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 1))
#define __TIM6_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 4))
#define __TIM6_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 4))
#define __TIM7_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 5))
#define __TIM7_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 5))
#define __TIM14_CLK_ENABLE()      (RCC->APB1ENR |= (1 << 8))
#define __TIM14_CLK_DISABLE()     (RCC->APB1ENR &= ~(1 << 8))
#define __TIM15_CLK_ENABLE()      (RCC->APB2ENR |= (1 << 16))
#define __TIM15_CLK_DISABLE()     (RCC->APB2ENR &= ~(1 << 16))
#define __TIM16_CLK_ENABLE()      (RCC->APB2ENR |= (1 << 17))
#define __TIM16_CLK_DISABLE()     (RCC->APB2ENR &= ~(1 << 17))
#define __TIM17_CLK_ENABLE()      (RCC->APB2ENR |= (1 << 18))
#define __TIM17_CLK_DISABLE()     (RCC->APB2ENR &= ~(1 << 18))
#endif

struct stm32_timer
{
    TIM_TypeDef *tim;
    IRQn_Type irq;
    void (*hdr)(void);
};

enum
{
#if defined(TIM1) && defined(BSP_USING_TIMER1)
    TIMER1_INDEX,
#endif
#if defined(TIM2) && defined(BSP_USING_TIMER2)
    TIMER2_INDEX,
#endif
#if defined(TIM3) && defined(BSP_USING_TIMER3)
    TIMER3_INDEX,
#endif
#if defined(TIM4) && defined(BSP_USING_TIMER4)
    TIMER4_INDEX,
#endif
#if defined(TIM6) && defined(BSP_USING_TIMER6)
    TIMER6_INDEX,
#endif
#if defined(TIM7) && defined(BSP_USING_TIMER7)
    TIMER7_INDEX,
#endif
#if defined(TIM14) && defined(BSP_USING_TIMER14)
    TIMER14_INDEX,
#endif
#if defined(TIM15) && defined(BSP_USING_TIMER15)
    TIMER15_INDEX,
#endif
#if defined(TIM16) && defined(BSP_USING_TIMER16)
    TIMER16_INDEX,
#endif
#if defined(TIM17) && defined(BSP_USING_TIMER17)
    TIMER17_INDEX,
#endif
#if defined(TIM21) && defined(BSP_USING_TIMER21)
    TIMER21_INDEX,
#endif
#if defined(TIM22) && defined(BSP_USING_TIMER22)
    TIMER22_INDEX,
#endif
    TIMER_INDEX_MAX
};

static struct stm32_timer timer_list[] = 
{
#if defined(TIM1) && defined(BSP_USING_TIMER1)
    {TIM1, __TIM1_IRQn, 0},
#endif
#if defined(TIM2) && defined(BSP_USING_TIMER2)
    {TIM2, __TIM2_IRQn, 0},
#endif
#if defined(TIM3) && defined(BSP_USING_TIMER3)
    {TIM3, __TIM3_IRQn, 0},
#endif
#if defined(TIM4) && defined(BSP_USING_TIMER4)
    {TIM4, __TIM4_IRQn, 0},
#endif
#if defined(TIM6) && defined(BSP_USING_TIMER6)
    {TIM6, __TIM6_IRQn, 0},
#endif
#if defined(TIM7) && defined(BSP_USING_TIMER7)
    {TIM7, __TIM7_IRQn, 0},
#endif
#if defined(TIM14) && defined(BSP_USING_TIMER14)
    {TIM14, __TIM14_IRQn, 0},
#endif
#if defined(TIM15) && defined(BSP_USING_TIMER15)
    {TIM15, __TIM15_IRQn, 0},
#endif
#if defined(TIM16) && defined(BSP_USING_TIMER16)
    {TIM16, __TIM16_IRQn, 0},
#endif
#if defined(TIM17) && defined(BSP_USING_TIMER17)
    {TIM17, __TIM17_IRQn, 0},
#endif
#if defined(TIM21) && defined(BSP_USING_TIMER21)
    {TIM21, __TIM21_IRQn, 0},
#endif
#if defined(TIM22) && defined(BSP_USING_TIMER22)
    {TIM22, __TIM22_IRQn, 0},
#endif
};

static void timer_clk_enable(unsigned int timerid)
{
    switch (timerid)
    {
#if defined(TIM1) && defined(BSP_USING_TIMER1)
    case TIMER1_INDEX:
        __TIM1_CLK_ENABLE();
        break;
#endif
#if defined(TIM2) && defined(BSP_USING_TIMER2)
    case TIMER2_INDEX:
        __TIM2_CLK_ENABLE();
        break;
#endif
#if defined(TIM3) && defined(BSP_USING_TIMER3)
    case TIMER3_INDEX:
        __TIM3_CLK_ENABLE();
        break;
#endif
#if defined(TIM4) && defined(BSP_USING_TIMER4)
    case TIMER4_INDEX:
        __TIM4_CLK_ENABLE();
        break;
#endif
#if defined(TIM6) && defined(BSP_USING_TIMER6)
    case TIMER6_INDEX:
        __TIM6_CLK_ENABLE();
        break;
#endif
#if defined(TIM7) && defined(BSP_USING_TIMER7)
    case TIMER7_INDEX:
        __TIM7_CLK_ENABLE();
        break;
#endif
#if defined(TIM14) && defined(BSP_USING_TIMER14)
    case TIMER14_INDEX:
        __TIM14_CLK_ENABLE();
        break;
#endif
#if defined(TIM15) && defined(BSP_USING_TIMER15)
    case TIMER15_INDEX:
        __TIM15_CLK_ENABLE();
        break;
#endif
#if defined(TIM16) && defined(BSP_USING_TIMER16)
    case TIMER16_INDEX:
        __TIM16_CLK_ENABLE();
        break;
#endif
#if defined(TIM17) && defined(BSP_USING_TIMER17)
    case TIMER17_INDEX:
        __TIM17_CLK_ENABLE();
        break;
#endif
#if defined(TIM21) && defined(BSP_USING_TIMER21)
    case TIMER21_INDEX:
        __TIM21_CLK_ENABLE();
        break;
#endif
#if defined(TIM22) && defined(BSP_USING_TIMER22)
    case TIMER22_INDEX:
        __TIM22_CLK_ENABLE();
        break;
#endif
    default:
        break;
    }
}

static int stm32_timer_start(timer_device_t *dev, unsigned int interval)
{
    struct stm32_timer *timer = dev->user_data;

    timer->tim->ARR = interval - 1;
    timer->tim->EGR = 1;
    timer->tim->SR = 0;
    if (timer->hdr)
    {
        NVIC_SetPriority(timer->irq, 3);
        NVIC_EnableIRQ(timer->irq);
        timer->tim->DIER = 1;
    }
    timer->tim->CR1 = 1;
    return 0;
}

static int stm32_timer_stop(timer_device_t *dev)
{
    struct stm32_timer *timer = dev->user_data;
    timer->tim->CR1 = 0;
    timer->tim->DIER = 0;
    timer->tim->SR = 0;
    NVIC_DisableIRQ(timer->irq);
    return 0;
}

static unsigned int stm32_timer_read(timer_device_t *dev)
{
    struct stm32_timer *timer = dev->user_data;

    return timer->tim->CNT;
}

static int stm32_timer_set_callback(timer_device_t *dev, void (*hdr)(void))
{
    struct stm32_timer *timer = dev->user_data;

    timer->hdr = hdr;
    return 0;
}

static const struct timer_device_ops stm32_timer_ops = 
{
    .start = stm32_timer_start,
    .stop = stm32_timer_stop,
    .read = stm32_timer_read,
    .set_callback = stm32_timer_set_callback,
};

void timer_device_init(timer_device_t *dev, unsigned int timerid, unsigned int freq)
{
    LOG_ASSERT(timerid < TIMER_INDEX_MAX);

    timer_clk_enable(timerid);
    dev->ops = &stm32_timer_ops;
    dev->user_data = &timer_list[timerid];

    timer_list[timerid].tim->PSC = SystemCoreClock / freq - 1;
}

static void timer_irq_handler(struct stm32_timer *timer)
{
    if (timer->tim->SR)
    {
        timer->tim->SR = 0;
        timer->hdr();
    }
}

#if defined(TIM1) && defined(BSP_USING_TIMER1)
#if defined(STM32G0) || defined(STM32F0)
void TIM1_BRK_UP_TRG_COM_IRQHandler(void)
{
    timer_irq_handler(&timer_list[TIMER1_INDEX]);
}
#endif
#endif

#if defined(TIM2) && defined(BSP_USING_TIMER2)
void TIM2_IRQHandler(void)
{
    timer_irq_handler(&timer_list[TIMER2_INDEX]);
}
#endif

#if defined(STM32G0B0xx) || defined(STM32G0B1xx) || defined(STM32G0C1xx)
#if defined(BSP_USING_TIMER3) || defined(BSP_USING_TIMER4)
void TIM3_TIM4_IRQHandler(void)
{
#ifdef BSP_USING_TIM3
    timer_irq_handler(&timer_list[TIMER3_INDEX]);
#endif
#ifdef BSP_USING_TIM4
    timer_irq_handler(&timer_list[TIMER4_INDEX]);
#endif
}
#endif
#else
#ifdef BSP_USING_TIMER3
void TIM3_IRQHandler(void)
{
    timer_irq_handler(&timer_list[TIMER3_INDEX]);
}
#endif
#ifdef BSP_USING_TIMER4
void TIM4_IRQHandler(void)
{
    timer_irq_handler(&timer_list[TIMER4_INDEX]);
}
#endif
#endif

#if defined(TIM6) && defined(BSP_USING_TIMER6)
#if defined(STM32G051xx) || defined(STM32G061xx) || defined(STM32G071xx)\
 || defined(STM32G081xx) || defined(STM32G0B1xx) || defined(STM32G0C1xx)
void TIM6_DAC_LPTIM1_IRQHandler(void)
#else 
void TIM6_IRQHandler(void)
#endif
{
    timer_irq_handler(&timer_list[TIMER6_INDEX]);
}
#endif

#if defined(TIM7) && defined(BSP_USING_TIMER7)
#if defined(STM32G051xx) || defined(STM32G061xx) || defined(STM32G071xx)\
 || defined(STM32G081xx) || defined(STM32G0B1xx) || defined(STM32G0C1xx)
void TIM7_LPTIM2_IRQHandler(void)
#else 
void TIM7_IRQHandler(void)
#endif
{
    timer_irq_handler(&timer_list[TIMER7_INDEX]);
}
#endif

#if defined(TIM14) && defined(BSP_USING_TIMER14)
void TIM14_IRQHandler(void)
{
    timer_irq_handler(&timer_list[TIMER14_INDEX]);
}
#endif

#if defined(TIM15) && defined(BSP_USING_TIMER15)
void TIM15_IRQHandler(void)
{
    timer_irq_handler(&timer_list[TIMER15_INDEX]);
}
#endif

#if defined(TIM16) && defined(BSP_USING_TIMER16)
#if defined(STM32G0B1xx) || defined(STM32G0C1xx)
void TIM16_FDCAN_IT0_IRQHandler(void)
#else 
void TIM16_IRQHandler(void)
#endif
{
    timer_irq_handler(&timer_list[TIMER16_INDEX]);
}
#endif

#if defined(TIM17) && defined(BSP_USING_TIMER17)
#if defined(STM32G0B1xx) || defined(STM32G0C1xx)
void TIM17_FDCAN_IT1_IRQHandler(void)
#else 
void TIM17_IRQHandler(void)
#endif
{
    timer_irq_handler(&timer_list[TIMER17_INDEX]);
}
#endif

#if defined(TIM21) && defined(BSP_USING_TIMER21)
void TIM21_IRQHandler(void)
{
    timer_irq_handler(&timer_list[TIMER21_INDEX]);
}
#endif

#if defined(TIM22) && defined(BSP_USING_TIMER22)
void TIM22_IRQHandler(void)
{
    timer_irq_handler(&timer_list[TIMER22_INDEX]);
}
#endif

#endif
