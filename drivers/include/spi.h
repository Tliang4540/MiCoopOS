/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __SPI_H__
#define __SPI_H__

#define SPI_CPHA        (1 << 0)
#define SPI_CPOL        (1 << 1)

#define SPI_MSB         (0 << 2)
#define SPI_LSB         (1 << 2)

#define SPI_MASTER      (0 << 3)
#define SPI_SLAVE       (1 << 3)

#define SPI_4WIRE       (0 << 4)
#define SPI_3WIRE       (1 << 4)

#define SPI_DATA_8BIT   (0 << 5)
#define SPI_DATA_16BIT  (1 << 5)

#define SPI_MODE0       (0 | 0)
#define SPI_MODE1       (0 | SPI_CPHA)
#define SPI_MODE2       (SPI_CPOL | 0)
#define SPI_MODE3       (SPI_CPOL | SPI_CPHA)

typedef struct spi_handle
{
    unsigned int cs_pin;
    void *user_data;
}spi_handle_t;

void spi_handle_init(spi_handle_t *spi_handle, unsigned int spi_id, unsigned int cs_pin);
void spi_open(spi_handle_t *spi_handle, unsigned int freq, unsigned int mode);

/**
 * @brief 控制spi传输一次数据
 * @param [inout] data 将data数据发出，同时接收数据到data
 * @param [in] size 传输数据的大小
 * @note 此接口会同时进行发送和接收，并将接收的数据覆盖发送的数据
 */
void spi_transfer(spi_handle_t *spi_handle, void *data, unsigned int size);
void spi_send(spi_handle_t *spi_handle, const void *data, unsigned int size);
void spi_send_then_send(spi_handle_t *spi_handle, const void *send_data1, unsigned int send_size1, const void *send_data2, unsigned int send_size2);
void spi_send_then_recv(spi_handle_t *spi_handle, const void *send_data, unsigned int send_size, void *recv_data, unsigned int recv_size);

#endif
