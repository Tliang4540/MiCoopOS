/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#include <serial.h>
#include <stm32.h>
#include <ringbuffer.h>
#include <mcconfig.h>
#include <log.h>
#include <mcos.h>

#if defined(STM32G0)
#define __UART1_IRQn               (27)
#define __UART2_IRQn               (28)
#define __UART3_IRQn               (29)
#define __UART4_IRQn               (29)
#define __UART5_IRQn               (29)
#define __UART6_IRQn               (29)
#define __LPUART1_IRQn             (29)
#define __LPUART2_IRQn             (28)

#define __UART1_CLK_ENABLE()       (RCC->APBENR2 |= (1 << 14))
#define __UART1_CLK_DISABLE()      (RCC->APBENR2 &= ~(1 << 14))
#define __UART2_CLK_ENABLE()       (RCC->APBENR1 |= (1 << 17))
#define __UART2_CLK_DISABLE()      (RCC->APBENR1 &= ~(1 << 17))
#define __UART3_CLK_ENABLE()       (RCC->APBENR1 |= (1 << 18))
#define __UART3_CLK_DISABLE()      (RCC->APBENR1 &= ~(1 << 18))
#define __UART4_CLK_ENABLE()       (RCC->APBENR1 |= (1 << 19))
#define __UART4_CLK_DISABLE()      (RCC->APBENR1 &= ~(1 << 19))
#define __UART5_CLK_ENABLE()       (RCC->APBENR1 |= (1 << 8))
#define __UART5_CLK_DISABLE()      (RCC->APBENR1 &= ~(1 << 8))
#define __UART6_CLK_ENABLE()       (RCC->APBENR1 |= (1 << 9))
#define __UART6_CLK_DISABLE()      (RCC->APBENR1 &= ~(1 << 9))
#define __LPUART1_CLK_ENABLE()     (RCC->APBENR1 |= (1 << 20))
#define __LPUART1_CLK_DISABLE()    (RCC->APBENR1 &= ~(1 << 20))
#define __LPUART2_CLK_ENABLE()     (RCC->APBENR1 |= (1 << 7))
#define __LPUART2_CLK_DISABLE()    (RCC->APBENR1 &= ~(1 << 7))
#elif defined (STM32L0)
#define __UART1_IRQn               (27)
#define __UART2_IRQn               (28)
#define __UART4_IRQn               (14)
#define __UART5_IRQn               (14)
#define __LPUART1_IRQn             (29)

#define __UART1_CLK_ENABLE()       (RCC->APB2ENR |= (1 << 14))
#define __UART1_CLK_DISABLE()      (RCC->APB2ENR &= ~(1 << 14))
#define __UART2_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 17))
#define __UART2_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 17))
#define __UART4_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 19))
#define __UART4_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 19))
#define __UART5_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 20))
#define __UART5_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 20))
#define __LPUART1_CLK_ENABLE()     (RCC->APB1ENR |= (1 << 18))
#define __LPUART1_CLK_DISABLE()    (RCC->APB1ENR &= ~(1 << 18))
#elif defined (STM32F0)
#define __UART1_IRQn               (27)
#define __UART2_IRQn               (28)
#define __UART3_IRQn               (29)
#define __UART4_IRQn               (29)
#define __UART5_IRQn               (29)
#define __UART6_IRQn               (29)
#define __UART7_IRQn               (29)
#define __UART8_IRQn               (29)

#define __UART1_CLK_ENABLE()       (RCC->APB2ENR |= (1 << 14))
#define __UART1_CLK_DISABLE()      (RCC->APB2ENR &= ~(1 << 14))
#define __UART2_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 17))
#define __UART2_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 17))
#define __UART3_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 18))
#define __UART3_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 18))
#define __UART4_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 19))
#define __UART4_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 19))
#define __UART5_CLK_ENABLE()       (RCC->APB1ENR |= (1 << 20))
#define __UART5_CLK_DISABLE()      (RCC->APB1ENR &= ~(1 << 20))
#define __UART6_CLK_ENABLE()       (RCC->APB2ENR |= (1 << 5))
#define __UART6_CLK_DISABLE()      (RCC->APB2ENR &= ~(1 << 5))
#define __UART7_CLK_ENABLE()       (RCC->APB2ENR |= (1 << 6))
#define __UART7_CLK_DISABLE()      (RCC->APB2ENR &= ~(1 << 6))
#define __UART8_CLK_ENABLE()       (RCC->APB2ENR |= (1 << 7))
#define __UART8_CLK_DISABLE()      (RCC->APB2ENR &= ~(1 << 7))
#endif

#if defined(USART1) && defined(BSP_USING_UART1)
#ifndef BSP_UART1_TX_BUFSIZE
#define BSP_UART1_TX_BUFSIZE        BSP_UART_BUFFER_SIZE
#endif
#ifndef BSP_UART1_RX_BUFSIZE
#define BSP_UART1_RX_BUFSIZE        BSP_UART_BUFFER_SIZE
#endif
static unsigned char uart1_tx_buf[BSP_UART1_TX_BUFSIZE];
static unsigned char uart1_rx_buf[BSP_UART1_RX_BUFSIZE];
#endif

#if defined(USART2) && defined(BSP_USING_UART2)
#ifndef BSP_UART2_TX_BUFSIZE
#define BSP_UART2_TX_BUFSIZE        BSP_UART_BUFFER_SIZE
#endif
#ifndef BSP_UART2_RX_BUFSIZE
#define BSP_UART2_RX_BUFSIZE        BSP_UART_BUFFER_SIZE
#endif
static unsigned char uart2_tx_buf[BSP_UART2_TX_BUFSIZE];
static unsigned char uart2_rx_buf[BSP_UART2_RX_BUFSIZE];
#endif

#if defined(USART3) && defined(BSP_USING_UART3)
#ifndef BSP_UART3_TX_BUFSIZE
#define BSP_UART3_TX_BUFSIZE        BSP_UART_BUFFER_SIZE
#endif
#ifndef BSP_UART3_RX_BUFSIZE
#define BSP_UART3_RX_BUFSIZE        BSP_UART_BUFFER_SIZE
#endif
static unsigned char uart3_tx_buf[BSP_UART3_TX_BUFSIZE];
static unsigned char uart3_rx_buf[BSP_UART3_RX_BUFSIZE];
#endif

#if defined(USART4) && defined(BSP_USING_UART4)
#ifndef BSP_UART4_TX_BUFSIZE
#define BSP_UART4_TX_BUFSIZE        BSP_UART_BUFFER_SIZE
#endif
#ifndef BSP_UART4_RX_BUFSIZE
#define BSP_UART4_RX_BUFSIZE        BSP_UART_BUFFER_SIZE
#endif
static unsigned char uart4_tx_buf[BSP_UART4_TX_BUFSIZE];
static unsigned char uart4_rx_buf[BSP_UART4_RX_BUFSIZE];
#endif

#if defined(USART5) && defined(BSP_USING_UART5)
#ifndef BSP_UART5_TX_BUFSIZE
#define BSP_UART5_TX_BUFSIZE        BSP_UART_BUFFER_SIZE
#endif
#ifndef BSP_UART5_RX_BUFSIZE
#define BSP_UART5_RX_BUFSIZE        BSP_UART_BUFFER_SIZE
#endif
static unsigned char uart5_tx_buf[BSP_UART5_TX_BUFSIZE];
static unsigned char uart5_rx_buf[BSP_UART5_RX_BUFSIZE];
#endif

#if defined(USART6) && defined(BSP_USING_UART6)
#ifndef BSP_UART6_TX_BUFSIZE
#define BSP_UART6_TX_BUFSIZE        BSP_UART_BUFFER_SIZE
#endif
#ifndef BSP_UART6_RX_BUFSIZE
#define BSP_UART6_RX_BUFSIZE        BSP_UART_BUFFER_SIZE
#endif
static unsigned char uart6_tx_buf[BSP_UART6_TX_BUFSIZE];
static unsigned char uart6_rx_buf[BSP_UART6_RX_BUFSIZE];
#endif

#if defined(USART7) && defined(BSP_USING_UART7)
#ifndef BSP_UART7_TX_BUFSIZE
#define BSP_UART7_TX_BUFSIZE        BSP_UART_BUFFER_SIZE
#endif
#ifndef BSP_UART7_RX_BUFSIZE
#define BSP_UART7_RX_BUFSIZE        BSP_UART_BUFFER_SIZE
#endif
static unsigned char uart7_tx_buf[BSP_UART7_TX_BUFSIZE];
static unsigned char uart7_rx_buf[BSP_UART7_RX_BUFSIZE];
#endif

#if defined(USART8) && defined(BSP_USING_UART8)
#ifndef BSP_UART8_TX_BUFSIZE
#define BSP_UART8_TX_BUFSIZE        BSP_UART_BUFFER_SIZE
#endif
#ifndef BSP_UART8_RX_BUFSIZE
#define BSP_UART8_RX_BUFSIZE        BSP_UART_BUFFER_SIZE
#endif
static unsigned char uart8_tx_buf[BSP_UART8_TX_BUFSIZE];
static unsigned char uart8_rx_buf[BSP_UART8_RX_BUFSIZE];
#endif

#if defined(LPUART1) && defined(BSP_USING_LPUART1)
#ifndef BSP_LPUART1_TX_BUFSIZE
#define BSP_LPUART1_TX_BUFSIZE      BSP_UART_BUFFER_SIZE
#endif
#ifndef BSP_LPUART1_RX_BUFSIZE
#define BSP_LPUART1_RX_BUFSIZE      BSP_UART_BUFFER_SIZE
#endif
static unsigned char lpuart1_tx_buf[BSP_LPUART1_TX_BUFSIZE];
static unsigned char lpuart1_rx_buf[BSP_LPUART1_RX_BUFSIZE];
#endif

#if defined(LPUART2) && defined(BSP_USING_LPUART2)
#ifndef BSP_LPUART2_TX_BUFSIZE
#define BSP_LPUART2_TX_BUFSIZE      BSP_UART_BUFFER_SIZE
#endif
#ifndef BSP_LPUART2_RX_BUFSIZE
#define BSP_LPUART2_RX_BUFSIZE      BSP_UART_BUFFER_SIZE
#endif
static unsigned char lpuart2_tx_buf[BSP_LPUART2_TX_BUFSIZE];
static unsigned char lpuart2_rx_buf[BSP_LPUART2_RX_BUFSIZE];
#endif

struct stm32_uart
{
    USART_TypeDef *uart;
    IRQn_Type irq;
    unsigned int lock;
    rb_t tx_rb;
    rb_t rx_rb;
};

enum
{
#if defined(USART1) && defined(BSP_USING_UART1)
    UART1_INDEX,
#endif
#if defined(USART2) && defined(BSP_USING_UART2)
    UART2_INDEX,
#endif
#if defined(USART3) && defined(BSP_USING_UART3)
    UART3_INDEX,
#endif
#if defined(USART4) && defined(BSP_USING_UART4)
    UART4_INDEX,
#endif
#if defined(USART5) && defined(BSP_USING_UART5)
    UART5_INDEX,
#endif
#if defined(USART6) && defined(BSP_USING_UART6)
    UART6_INDEX,
#endif
#if defined(USART7) && defined(BSP_USING_UART7)
    UART7_INDEX,
#endif
#if defined(USART8) && defined(BSP_USING_UART8)
    UART8_INDEX,
#endif
#if defined(LPUART1) && defined(BSP_USING_LPUART1)
    LPUART1_INDEX,
#endif
#if defined(LPUART2) && defined(BSP_USING_LPUART2)
    LPUART2_INDEX,
#endif
    UART_INDEX_MAX
};

static struct stm32_uart uart_list[] = 
{
#if defined(USART1) && defined(BSP_USING_UART1)
    {USART1, __UART1_IRQn, 0, {0, 0, sizeof(uart1_tx_buf), uart1_tx_buf}, {0, 0, sizeof(uart1_rx_buf), uart1_rx_buf}},
#endif
#if defined(USART2) && defined(BSP_USING_UART2)
    {USART2, __UART2_IRQn, 0, {0, 0, sizeof(uart2_tx_buf), uart2_tx_buf}, {0, 0, sizeof(uart2_rx_buf), uart2_rx_buf}},
#endif
#if defined(USART3) && defined(BSP_USING_UART3)
    {USART3, __UART3_IRQn, 0, {0, 0, sizeof(uart3_tx_buf), uart3_tx_buf}, {0, 0, sizeof(uart3_rx_buf), uart3_rx_buf}},
#endif
#if defined(USART4) && defined(BSP_USING_UART4)
    {USART4, __UART4_IRQn, 0, {0, 0, sizeof(uart4_tx_buf), uart4_tx_buf}, {0, 0, sizeof(uart4_rx_buf), uart4_rx_buf}},
#endif
#if defined(USART5) && defined(BSP_USING_UART5)
    {USART5, __UART5_IRQn, 0, {0, 0, sizeof(uart5_tx_buf), uart5_tx_buf}, {0, 0, sizeof(uart5_rx_buf), uart5_rx_buf}},
#endif
#if defined(USART6) && defined(BSP_USING_UART6)
    {USART6, __UART6_IRQn, 0, {0, 0, sizeof(uart6_tx_buf), uart6_tx_buf}, {0, 0, sizeof(uart6_rx_buf), uart6_rx_buf}},
#endif
#if defined(USART7) && defined(BSP_USING_UART7)
    {USART7, __UART7_IRQn, 0, {0, 0, sizeof(uart7_tx_buf), uart7_tx_buf}, {0, 0, sizeof(uart7_rx_buf), uart7_rx_buf}},
#endif
#if defined(USART8) && defined(BSP_USING_UART8)
    {USART8, __UART8_IRQn, 0, {0, 0, sizeof(uart8_tx_buf), uart8_tx_buf}, {0, 0, sizeof(uart8_rx_buf), uart8_rx_buf}},
#endif
#if defined(LPUART1) && defined(BSP_USING_LPUART1)
    {LPUART1, __LPUART1_IRQn, 0, {0, 0, sizeof(lpuart1_tx_buf), lpuart1_tx_buf}, {0, 0, sizeof(lpuart1_rx_buf), lpuart1_rx_buf}},
#endif
#if defined(LPUART2) && defined(BSP_USING_LPUART2)
    {LPUART2, __LPUART2_IRQn, 0, {0, 0, sizeof(lpuart2_tx_buf), lpuart2_tx_buf}, {0, 0, sizeof(lpuart2_rx_buf), lpuart2_rx_buf}},
#endif
};

static void uart_clk_enable(unsigned int port)
{
    switch (port)
    {
#if defined(USART1) && defined(BSP_USING_UART1)
    case UART1_INDEX:
        __UART1_CLK_ENABLE();
        break;
#endif
#if defined(USART2) && defined(BSP_USING_UART2)
    case UART2_INDEX:
        __UART2_CLK_ENABLE();
        break;
#endif
#if defined(USART3) && defined(BSP_USING_UART3)
    case UART3_INDEX:
        __UART3_CLK_ENABLE();
        break;
#endif
#if defined(USART4) && defined(BSP_USING_UART4)
    case UART4_INDEX:
        __UART4_CLK_ENABLE();
        break;
#endif
#if defined(USART5) && defined(BSP_USING_UART5)
    case UART5_INDEX:
        __UART5_CLK_ENABLE();
        break;
#endif
#if defined(USART6) && defined(BSP_USING_UART6)
    case UART6_INDEX:
        __UART6_CLK_ENABLE();
        break;
#endif
#if defined(USART7) && defined(BSP_USING_UART7)
    case UART7_INDEX:
        __UART7_CLK_ENABLE();
        break;
#endif
#if defined(USART8) && defined(BSP_USING_UART8)
    case UART8_INDEX:
        __UART8_CLK_ENABLE();
        break;
#endif
#if defined(LPUART1) && defined(BSP_USING_LPUART1)
    case LPUART1_INDEX:
        __LPUART1_CLK_ENABLE();
        break;
#endif
#if defined(LPUART2) && defined(BSP_USING_LPUART2)
    case LPUART2_INDEX:
        __LPUART2_CLK_ENABLE();
        break;
#endif
    default:
        break;
    }
}

void serial_handle_init(serial_handle_t *serial_handle, unsigned int serial_id)
{
    LOG_ASSERT(serial_id < UART_INDEX_MAX);
    
    uart_clk_enable(serial_id);
    serial_handle->user_data = &uart_list[serial_id];
}

void serial_open(serial_handle_t *serial_handle, unsigned int baudrate)
{
    struct stm32_uart *uart = serial_handle->user_data;
    
    // LPUART使用HSI时钟
    #if defined(LPUART1)
    if (uart->uart == LPUART1)
        uart->uart->BRR = (HSI_VALUE * 256 + baudrate / 2) / baudrate;
    else
    #endif
    #if defined(LPUART2)
    if (uart->uart == LPUART2)
        uart->uart->BRR = (HSI_VALUE * 256 + baudrate / 2) / baudrate;
    else
    #endif
        uart->uart->BRR = (SystemCoreClock + baudrate / 2) / baudrate;

    NVIC_SetPriority(uart->irq, 3);
    NVIC_EnableIRQ(uart->irq);
    uart->uart->CR1 = (1 << 5) | USART_CR1_TE | USART_CR1_RE | USART_CR1_UE;
}

void serial_close(serial_handle_t *serial_handle)
{
    struct stm32_uart *uart = serial_handle->user_data;

    uart->uart->CR1 = 0;
    NVIC_DisableIRQ(uart->irq);
}

void serial_write(serial_handle_t *serial_handle, const void *data, unsigned int size)
{
    struct stm32_uart *uart;

    if (size == 0)
        return;

    uart = serial_handle->user_data;

    while(uart->lock)
        mc_delay(0);
    uart->lock = 1;

    while (1)
    {
        unsigned int written_size = rb_write(&uart->tx_rb, data, size);
        uart->uart->CR1 |= (1 << 7);
        size -= written_size;

        if (size == 0)
            break;
        // 没有写完说明缓存满了，等待发送
        data = (void*)((unsigned)data + written_size);
        mc_delay(2);
    }
    uart->lock = 0;
}

unsigned int serial_read(serial_handle_t *serial_handle, void *data, unsigned int size)
{
    struct stm32_uart *uart = serial_handle->user_data;
    return rb_read(&uart->rx_rb, data, size);
}

static void uart_irq_handler(struct stm32_uart *uart)
{
    unsigned int sr;
    unsigned char data;

    sr = uart->uart->ISR;

    if ((uart->uart->CR1 & (1 << 7)) && (sr & (1 << 7)))
    {
        if (rb_read(&uart->tx_rb, &data, 1))
            uart->uart->TDR = data;
        else
            uart->uart->CR1 &= ~(1 << 7);
    }

    if (sr & (1 << 5))
    {
        data = uart->uart->RDR;
        rb_write(&uart->rx_rb, &data, 1);
        uart->uart->ICR = 0xf;    //写0xffffffff会导致发送两次相同的数据
    }
}

#if defined(USART1) && defined(BSP_USING_UART1)
void USART1_IRQHandler(void)
{
    uart_irq_handler(&uart_list[UART1_INDEX]);
}
#endif

#if defined(USART2) && defined(BSP_USING_UART2)
void USART2_IRQHandler(void)
{
    uart_irq_handler(&uart_list[UART2_INDEX]);
}
#endif

#if defined(USART3) && defined(BSP_USING_UART3)
void USART3_IRQHandler(void)
{
    uart_irq_handler(&uart_list[UART3_INDEX]);
}
#endif

#if defined(USART4) && defined(BSP_USING_UART4)
void UART4_IRQHandler(void)
{
    uart_irq_handler(&uart_list[UART4_INDEX]);
}
#endif

#if defined(USART5) && defined(BSP_USING_UART5)
void UART5_IRQHandler(void)
{
    uart_irq_handler(&uart_list[UART5_INDEX]);
}
#endif

#if defined(USART6) && defined(BSP_USING_UART6)
void USART6_IRQHandler(void)
{
    uart_irq_handler(&uart_list[UART6_INDEX]);
}
#endif

#if defined(USART7) && defined(BSP_USING_UART7)
void UART7_IRQHandler(void)
{
    uart_irq_handler(&uart_list[UART7_INDEX]);
}
#endif

#if defined(USART8) && defined(BSP_USING_UART8)
void UART8_IRQHandler(void)
{
    uart_irq_handler(&uart_list[UART8_INDEX]);
}
#endif

#if defined(LPUART1) && defined(BSP_USING_LPUART1)
void LPUART1_IRQHandler(void)
{
    uart_irq_handler(&uart_list[LPUART1_INDEX]);
}
#endif

#if defined(LPUART2) && defined(BSP_USING_LPUART2)
void LPUART2_IRQHandler(void)
{
    uart_irq_handler(&uart_list[LPUART2_INDEX]);
}
#endif

#if defined(STM32G0)
#if defined(BSP_USING_UART2)
#if defined(STM32G0B1xx) || defined(STM32G0C1xx)
void USART2_LPUART2_IRQHandler(void)
{
#if defined(BSP_USING_UART2)
    USART2_IRQHandler();
#endif
#if defined(BSP_USING_LPUART2)
    LPUART2_IRQHandler();
#endif
}
#endif /* defined(STM32G0B1xx) || defined(STM32G0C1xx) */
#endif /* defined(BSP_USING_UART2) */
#if defined(BSP_USING_UART3) || defined(BSP_USING_UART4) || defined(BSP_USING_UART5) || defined(BSP_USING_UART6) \
    || defined(BSP_USING_LPUART1)
#if defined(STM32G070xx)
void USART3_4_IRQHandler(void)
#elif defined(STM32G071xx) || defined(STM32G081xx)
void USART3_4_LPUART1_IRQHandler(void)
#elif defined(STM32G0B0xx)
void USART3_4_5_6_IRQHandler(void)
#elif defined(STM32G0B1xx) || defined(STM32G0C1xx)
void USART3_4_5_6_LPUART1_IRQHandler(void)
#endif /* defined(STM32G070xx) */
{
#if defined(BSP_USING_UART3)
    USART3_IRQHandler();
#endif
#if defined(BSP_USING_UART4)
    UART4_IRQHandler();
#endif
#if defined(BSP_USING_UART5)
    UART5_IRQHandler();
#endif
#if defined(BSP_USING_UART6)
    USART6_IRQHandler();
#endif
#if defined(BSP_USING_LPUART1)
    LPUART1_IRQHandler();
#endif
}
#endif /* defined(BSP_USING_UART3) || defined(BSP_USING_UART4) || defined(BSP_USING_UART5) || defined(BSP_USING_UART6) */
#endif /* defined(SOC_SERIES_STM32G0) */

#if defined(STM32F0)
#if defined(BSP_USING_UART3) || defined(BSP_USING_UART4) || defined(BSP_USING_UART5) \
 || defined(BSP_USING_UART6) || defined(BSP_USING_UART7) || defined(BSP_USING_UART8)
void USART3_4_IRQHandler(void)
{
#if defined(BSP_USING_UART3)
    USART3_IRQHandler();
#endif
#if defined(BSP_USING_UART4)
    UART4_IRQHandler();
#endif
#if defined(BSP_USING_UART5)
    UART5_IRQHandler();
#endif
#if defined(BSP_USING_UART6)
    USART6_IRQHandler();
#endif
#if defined(BSP_USING_UART7)
    UART7_IRQHandler();
#endif
#if defined(BSP_USING_UART8)
    UART8_IRQHandler();
#endif
}
#endif
#endif
