/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#include <pin.h>
#include <stm32.h>
#include <log.h>

#define PIN_PORT(pin)   ((pin) >> 4u)
#define PIN_NO(pin)     ((pin) & 0xFu)
#define PIN_STPORT(pin) ((GPIO_TypeDef *)(GPIOA_BASE + (0x400u * PIN_PORT(pin))))
#define PIN_STPIN(pin)  (1u << PIN_NO(pin))

#if defined(GPIOP)
#define __STM32_PORT_MAX 15u
#elif defined(GPIOO)
#define __STM32_PORT_MAX 14u
#elif defined(GPION)
#define __STM32_PORT_MAX 13u
#elif defined(GPIOM)
#define __STM32_PORT_MAX 12u
#elif defined(GPIOK)
#define __STM32_PORT_MAX 11u
#elif defined(GPIOJ)
#define __STM32_PORT_MAX 10u
#elif defined(GPIOI)
#define __STM32_PORT_MAX 9u
#elif defined(GPIOH)
#define __STM32_PORT_MAX 8u
#elif defined(GPIOG)
#define __STM32_PORT_MAX 7u
#elif defined(GPIOF)
#define __STM32_PORT_MAX 6u
#elif defined(GPIOE)
#define __STM32_PORT_MAX 5u
#elif defined(GPIOD)
#define __STM32_PORT_MAX 4u
#elif defined(GPIOC)
#define __STM32_PORT_MAX 3u
#elif defined(GPIOB)
#define __STM32_PORT_MAX 2u
#elif defined(GPIOA)
#define __STM32_PORT_MAX 1u
#else
#define __STM32_PORT_MAX 0u
#error Unsupported STM32 GPIO peripheral.
#endif

#define PIN_STPORT_MAX __STM32_PORT_MAX

static void (*irq_cb_list[16])(void);

void pin_mode(unsigned int pin, pin_mode_t mode)
{
    LOG_ASSERT(PIN_PORT(pin) < PIN_STPORT_MAX);

    GPIO_TypeDef *gpio = PIN_STPORT(pin);

    pin = PIN_STPIN(pin);

    switch (mode)
    {
    case PIN_MODE_NONE:
        LL_GPIO_SetPinMode(gpio, pin, LL_GPIO_MODE_ANALOG);
        break;
    case PIN_MODE_INPUT:
        LL_GPIO_SetPinMode(gpio, pin, LL_GPIO_MODE_INPUT);
        break;
    case PIN_MODE_OUTPUT_PP:
        LL_GPIO_SetPinMode(gpio, pin, LL_GPIO_MODE_OUTPUT);
        LL_GPIO_SetPinOutputType(gpio, pin, LL_GPIO_OUTPUT_PUSHPULL);
        break;
    case PIN_MODE_OUTPUT_OD:
        LL_GPIO_SetPinMode(gpio, pin, LL_GPIO_MODE_OUTPUT);
        LL_GPIO_SetPinOutputType(gpio, pin, LL_GPIO_OUTPUT_OPENDRAIN);
        break;
    case PIN_MODE_FUNCTION_PP:
        LL_GPIO_SetPinMode(gpio, pin, LL_GPIO_MODE_ALTERNATE);
        LL_GPIO_SetPinOutputType(gpio, pin, LL_GPIO_OUTPUT_PUSHPULL);
        break;
    case PIN_MODE_FUNCTION_OD:
        LL_GPIO_SetPinMode(gpio, pin, LL_GPIO_MODE_ALTERNATE);
        LL_GPIO_SetPinOutputType(gpio, pin, LL_GPIO_OUTPUT_OPENDRAIN);
        break;
    default:
        break;
    }
}

void pin_pull(unsigned int pin, pin_pull_t pull)
{
    LOG_ASSERT(PIN_PORT(pin) < PIN_STPORT_MAX);

    GPIO_TypeDef *gpio = PIN_STPORT(pin);

    pin = PIN_STPIN(pin);

    LL_GPIO_SetPinPull(gpio, pin, pull);
}

void pin_function(unsigned int pin, unsigned int func)
{
    LOG_ASSERT(PIN_PORT(pin) < PIN_STPORT_MAX);

    GPIO_TypeDef *gpio = PIN_STPORT(pin);

    pin = pin & 0xFu;
    
    if(pin < 8)
        LL_GPIO_SetAFPin_0_7(gpio, 1 << pin, func);
    else 
        LL_GPIO_SetAFPin_8_15(gpio, 1 << pin, func);
}

void pin_speed(unsigned int pin, unsigned int speed)
{
    LOG_ASSERT(PIN_PORT(pin) < PIN_STPORT_MAX);

    GPIO_TypeDef *gpio = PIN_STPORT(pin);

    pin = PIN_STPIN(pin);
    LL_GPIO_SetPinSpeed(gpio, pin, speed);
}

static void exti_set_source(unsigned port, unsigned pin)
{
#if defined(STM32G0)
    unsigned index = pin / 4;
    unsigned pos = (pin % 4) * 8;
    // 配置EXTI的GPIO源
    MODIFY_REG(EXTI->EXTICR[index], EXTI_EXTICR1_EXTI0 << pos, port << pos);
#elif defined(STM32L0) || defined(STM32F0)
    unsigned index = pin / 4;
    unsigned pos = (pin % 4) * 4;
    // 配置EXTI的GPIO源
    MODIFY_REG(SYSCFG->EXTICR[index], SYSCFG_EXTICR1_EXTI0 << pos, port << pos);
#endif
}

static void exti_clear_flag(unsigned pin)
{
#if defined(STM32G0)
    LL_EXTI_ClearFallingFlag_0_31(pin);
    LL_EXTI_ClearRisingFlag_0_31(pin);
#elif defined(STM32L0) || defined(STM32F0)
    LL_EXTI_ClearFlag_0_31(pin);
#endif
}

static unsigned int exti_get_flag(void)
{
#if defined(STM32G0)
    return EXTI->IMR1;
#elif defined(STM32L0) || defined(STM32F0)
    return EXTI->IMR;
#endif
}

void pin_attach_irq(unsigned int pin, void (*callback)(void), pin_irq_mode_t mode)
{
    unsigned port = PIN_PORT(pin);
    unsigned pin_mask;

    // pin超出IO范围
    LOG_ASSERT(PIN_PORT(pin) < PIN_STPORT_MAX);

    // 配置回调函数
    pin = pin & 0xFu;
    irq_cb_list[pin] = callback;

    pin_mask = 1 << pin;
    switch (mode)
    {
    case PIN_IRQ_MODE_RISING:
        LL_EXTI_DisableFallingTrig_0_31(pin_mask);
        LL_EXTI_EnableRisingTrig_0_31(pin_mask);
        break;
    case PIN_IRQ_MODE_FALLING:
        LL_EXTI_DisableRisingTrig_0_31(pin_mask);
        LL_EXTI_EnableFallingTrig_0_31(pin_mask);
        break;
    case PIN_IRQ_MODE_RISING_FALLING:
        LL_EXTI_EnableFallingTrig_0_31(pin_mask);
        LL_EXTI_EnableRisingTrig_0_31(pin_mask);
        break;
    default:
        LL_EXTI_DisableRisingTrig_0_31(pin_mask);
        LL_EXTI_DisableFallingTrig_0_31(pin_mask);
        LL_EXTI_DisableIT_0_31(pin_mask);
        pin_mask = exti_get_flag();
    #if defined(STM32G0) || defined(STM32L0) || defined(STM32F0)
        if ((pin_mask & 0x03) == 0)
        {
            NVIC_DisableIRQ(EXTI0_1_IRQn);
        }
        else if ((pin_mask & 0x0c) == 0)
        {
            NVIC_DisableIRQ(EXTI2_3_IRQn);
        }
        else if ((pin_mask & 0xfff0) == 0)
        {
            NVIC_DisableIRQ(EXTI4_15_IRQn);
        }
    #endif
        return;
    }

    // 设置中断源
    exti_set_source(port, pin);
    
    // 使能中断 
    exti_clear_flag(pin_mask);
    LL_EXTI_EnableIT_0_31(pin_mask);
#if defined(STM32G0) || defined(STM32L0) || defined(STM32F0)
    int irqn;
    if (pin < 2)
    {
        irqn = EXTI0_1_IRQn;
    }
    else if (pin < 4)
    {
        irqn = EXTI2_3_IRQn;
    }
    else
    {
        irqn = EXTI4_15_IRQn;
    }
    NVIC_SetPriority(irqn, 3);
    NVIC_EnableIRQ(irqn);
#endif
}

void pin_write(unsigned int pin, unsigned int value)
{
    LOG_ASSERT(PIN_PORT(pin) < PIN_STPORT_MAX);

    GPIO_TypeDef *gpio = PIN_STPORT(pin);

    pin = PIN_STPIN(pin);
    
    if(value == 0)
        LL_GPIO_ResetOutputPin(gpio, pin);
    else
        LL_GPIO_SetOutputPin(gpio, pin);
}

int pin_read(unsigned int pin)
{
    LOG_ASSERT(PIN_PORT(pin) < PIN_STPORT_MAX);

    GPIO_TypeDef *gpio = PIN_STPORT(pin);

    pin = PIN_STPIN(pin);
    
    return ((gpio->IDR & pin) != 0);
}

static void exti_irq_handler(unsigned int pin)
{
    if (irq_cb_list[pin])
        irq_cb_list[pin]();
}

static unsigned int exti_get_pending(void)
{
#if defined(STM32G0)
    return (EXTI->FPR1 | EXTI->RPR1);
#elif defined(STM32L0) || defined(STM32F0)
    return EXTI->PR;
#endif
}

static void exti_clear_pending(unsigned int pin)
{
#if defined(STM32G0)
    EXTI->FPR1 = pin;
    EXTI->RPR1 = pin;
#elif defined(STM32L0) || defined(STM32F0)
    EXTI->PR = pin;
#endif
}

#if defined(STM32G0) || defined(STM32L0) || defined(STM32F0)
void EXTI0_1_IRQHandler(void)
{
    unsigned int tmp = exti_get_pending() & 0x03;

    if (tmp & 0x01)
    {
        exti_irq_handler(0);
    }
    if (tmp & 0x02)
    {
        exti_irq_handler(1);
    }
    
    exti_clear_pending(tmp);
}

void EXTI2_3_IRQHandler(void)
{
    unsigned int tmp = exti_get_pending() & 0x0c;

    if (tmp & 0x04)
    {
        exti_irq_handler(2);
    }
    if (tmp & 0x08)
    {
        exti_irq_handler(3);
    }
    
    exti_clear_pending(tmp);
}

void EXTI4_15_IRQHandler(void)
{
    unsigned int tmp = exti_get_pending() & 0xfff0;

    for (int i = 4, n = tmp; n; i++, n >>= 1)
    {
        if (n & 0x10)
        {
            exti_irq_handler(i);
        }
    }
    
    exti_clear_pending(tmp);
}
#endif
