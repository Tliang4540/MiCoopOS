/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#include <i2c.h>
#include <stm32.h>
#include <mcconfig.h>
#include <mcos.h>
#include <log.h>

#if defined(BSP_USING_I2C1) || defined(BSP_USING_I2C2) || defined(BSP_USING_I2C3)

#if defined(STM32G0)
#define __I2C_TIMING_100K         (0x10707DBC)
#define __I2C_TIMING_400K         (0x00602173)

#define __I2C1_IRQn               (23)
#define __I2C2_IRQn               (24)
#define __I2C3_IRQn               (24)

#define __I2C1_CLK_ENABLE()       (RCC->APBENR1 |= (1 << 21))
#define __I2C1_CLK_DISABLE()      (RCC->APBENR1 &= ~(1 << 21))
#define __I2C2_CLK_ENABLE()       (RCC->APBENR1 |= (1 << 22))
#define __I2C2_CLK_DISABLE()      (RCC->APBENR1 &= ~(1 << 22))
#define __I2C3_CLK_ENABLE()       (RCC->APBENR1 |= (1 << 23))
#define __I2C3_CLK_DISABLE()      (RCC->APBENR1 &= ~(1 << 23))
#elif defined (STM32L0) || defined (STM32F0)
#define __I2C_TIMING_100K         (0x10707DBC)
#define __I2C_TIMING_400K         (0x00602173)

#define __I2C1_IRQn               (23)
#define __I2C2_IRQn               (24)

#define __I2C1_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 21))
#define __I2C1_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 21))
#define __I2C2_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 22))
#define __I2C2_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 22))
#endif

struct stm32_i2c
{
    I2C_TypeDef *i2c;
    mc_msg_t transfer_msg;
    unsigned char *buf;
    unsigned int interrupt_flag;
    unsigned int lock;
};

enum
{
#if defined(I2C1) && defined(BSP_USING_I2C1)
    I2C1_INDEX,
#endif
#if defined(I2C2) && defined(BSP_USING_I2C2)
    I2C2_INDEX,
#endif
#if defined(I2C3) && defined(BSP_USING_I2C3)
    I2C3_INDEX,
#endif
    I2C_INDEX_MAX
};

static struct stm32_i2c i2c_list[] = 
{
#if defined(I2C1) && defined(BSP_USING_I2C1)
    {I2C1, {NULL, MC_MSG_INVALID_VAL}, 0, 0, 0},
#endif
#if defined(I2C2) && defined(BSP_USING_I2C2)
    {I2C2, {NULL, MC_MSG_INVALID_VAL}, 0, 0, 0},
#endif
#if defined(I2C3) && defined(BSP_USING_I2C3)
    {I2C3, {NULL, MC_MSG_INVALID_VAL}, 0, 0, 0},
#endif
};

static void i2c_clk_enable(unsigned int i2c)
{
    switch (i2c)
    {
#if defined(I2C1) && defined(BSP_USING_I2C1)
    case I2C1_INDEX:
        __I2C1_CLK_ENABLE();
        NVIC_SetPriority(__I2C1_IRQn, 3);
        NVIC_EnableIRQ(__I2C1_IRQn);
        break;
#endif
#if defined(I2C2) && defined(BSP_USING_I2C2)
    case I2C2_INDEX:
        __I2C2_CLK_ENABLE();
        NVIC_SetPriority(__I2C2_IRQn, 3);
        NVIC_EnableIRQ(__I2C2_IRQn);
        break;
#endif
#if defined(I2C3) && defined(BSP_USING_I2C3)
    case I2C3_INDEX:
        __I2C3_CLK_ENABLE();
        NVIC_SetPriority(__I2C3_IRQn, 3);
        NVIC_EnableIRQ(__I2C3_IRQn);
        break;
#endif
    default:
        break;
    }
}

static int stm32_i2c_transfer(struct i2c_device *dev, unsigned int flags, void *data, size_t size)
{
    struct stm32_i2c *i2c = dev->bus->user_data;
    unsigned int end_flag;
    unsigned int start_flag;
    mc_ubase_t irq_msg;
    int ret = 0;

    while(i2c->lock)
        mc_delay(0);
    i2c->lock = 1;

    if ((flags & I2C_FLAG_NO_STOP) == 0)
        end_flag = LL_I2C_MODE_AUTOEND;
    else if (flags & I2C_FLAG_NEXT_RD)
        end_flag = LL_I2C_MODE_SOFTEND;     //发送完NByte后必须是Stop或Start
    else
        end_flag = LL_I2C_MODE_RELOAD;      //发送数据不能有restart

    i2c->buf = data;
    if (flags & I2C_FLAG_NO_START)
    {
        start_flag = LL_I2C_GENERATE_NOSTARTSTOP;
    }
    else if (flags & I2C_FLAG_RD)
    {
        start_flag = LL_I2C_GENERATE_START_READ;
        i2c->interrupt_flag = I2C_CR1_RXIE;
    }
    else 
    {
        start_flag = LL_I2C_GENERATE_START_WRITE;
        i2c->interrupt_flag = I2C_CR1_TXIE;
        i2c->i2c->TXDR = *i2c->buf++;
    }

    LL_I2C_HandleTransfer(i2c->i2c, dev->addr, LL_I2C_ADDRESSING_MODE_7BIT, size, end_flag, start_flag);
    i2c->i2c->CR1 |= I2C_CR1_ERRIE | I2C_CR1_TCIE | I2C_CR1_STOPIE | i2c->interrupt_flag;
    if (MC_FALSE == mc_msg_recv(&i2c->transfer_msg, &irq_msg, 1000))
    {
        LOG_E("i2c timeout:%x", i2c->i2c->ISR);
        i2c->i2c->ICR = I2C_ICR_ARLOCF | I2C_ICR_BERRCF | I2C_ICR_STOPCF | I2C_ICR_NACKCF;
        i2c->i2c->CR1 &= ~(I2C_CR1_ERRIE | I2C_CR1_TXIE | I2C_CR1_TCIE | I2C_CR1_RXIE | I2C_CR1_STOPIE);
    }
    else if (irq_msg)
    {
        LOG_E("i2c rs:%x", irq_msg);
    }
    else
    {
        goto stm32_i2c_exit;
    }

    if (i2c->i2c->ISR & I2C_ISR_BUSY)
    {
        i2c->i2c->CR1 = 0;
        LOG_E("i2c rb reset");
        mc_delay(1);
        i2c->i2c->CR1 = 1;
    }
    ret = MC_ERROR;

stm32_i2c_exit:
    i2c->lock = 0;
    return ret;
}

void i2c_bus_init(i2c_bus_t *bus, unsigned int i2c_id, unsigned int freq)
{
    LOG_ASSERT(i2c_id < I2C_INDEX_MAX);

    i2c_clk_enable(i2c_id);
    bus->transfer = stm32_i2c_transfer;
    bus->user_data = &i2c_list[i2c_id];

    if (freq <= 100000)
        freq = __I2C_TIMING_100K;
    else
        freq = __I2C_TIMING_400K;

    LL_I2C_SetTiming(i2c_list[i2c_id].i2c, freq);
    LL_I2C_Enable(i2c_list[i2c_id].i2c);
}

static void i2c_irq(struct stm32_i2c *i2c)
{
    unsigned int flags = i2c->i2c->ISR;

    if (flags & I2C_ISR_RXNE)
    {
        *i2c->buf++ = i2c->i2c->RXDR;
    }
    else if (flags & I2C_ISR_TXIS)
    {
        i2c->i2c->TXDR = *i2c->buf++;
    }

    if (flags & (I2C_ISR_TC | I2C_ISR_TCR | I2C_ISR_STOPF | I2C_ISR_ARLO | I2C_ISR_NACKF | I2C_ISR_BERR))
    {
        if(flags & (I2C_ISR_ARLO | I2C_ISR_NACKF | I2C_ISR_BERR))
        {
            mc_msg_send(&i2c->transfer_msg, i2c->i2c->ISR);
        }
        else
        {
            mc_msg_send(&i2c->transfer_msg, 0);
        }
        i2c->i2c->ICR = I2C_ICR_ARLOCF | I2C_ICR_BERRCF | I2C_ICR_STOPCF | I2C_ICR_NACKCF;
        i2c->i2c->CR1 &= ~(I2C_CR1_ERRIE | I2C_CR1_TXIE | I2C_CR1_TCIE | I2C_CR1_RXIE | I2C_CR1_STOPIE);
    }
}

#if defined(I2C1) && defined(BSP_USING_I2C1)
void I2C1_IRQHandler(void)
{
    i2c_irq(&i2c_list[I2C1_INDEX]);
}
#endif

#if defined(I2C2) && defined(BSP_USING_I2C2)
void I2C2_IRQHandler(void)
{
    i2c_irq(&i2c_list[I2C2_INDEX]);
}
#endif

#endif
