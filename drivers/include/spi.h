/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __SPI_H__
#define __SPI_H__

#include <mctypes.h>

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

#define SPI_FLAG_CS_TAKE     (1 << 0)
#define SPI_FLAG_CS_RELEASE  (1 << 1)

struct spi_bus;
typedef struct spi_device
{
    struct spi_bus *bus;
    unsigned int cs_pin;
}spi_device_t;

typedef struct spi_bus
{
    int (*transfer)(struct spi_device *dev, unsigned int flags, const void *send_buf, void *recv_buf, size_t size);
    void *user_data;
}spi_bus_t;

void spi_bus_init(spi_bus_t *bus, unsigned int spi_id, unsigned int freq, unsigned int mode);

static inline void spi_device_init(spi_device_t *dev, spi_bus_t *bus, unsigned int cs_pin)
{
    dev->bus = bus;
    dev->cs_pin = cs_pin;
}

static inline int spi_transfer(spi_device_t *dev, unsigned int flags, const void *send_buf, void *recv_buf, size_t size)
{
    return dev->bus->transfer(dev, flags, send_buf, recv_buf, size);
}

static inline int spi_send(spi_device_t *dev, const void *data, size_t size)
{
    return dev->bus->transfer(dev, SPI_FLAG_CS_TAKE | SPI_FLAG_CS_RELEASE, data, 0, size);
}

static inline int spi_recv(spi_device_t *dev, void *data, size_t size)
{
    return dev->bus->transfer(dev, SPI_FLAG_CS_TAKE | SPI_FLAG_CS_RELEASE, 0, data, size);
}

static inline int spi_send_then_send(spi_device_t *dev, const void *data1, size_t size1, const void *data2, size_t size2)
{
    int ret1 = dev->bus->transfer(dev, SPI_FLAG_CS_TAKE, data1, 0, size1);
    int ret2 = dev->bus->transfer(dev, SPI_FLAG_CS_RELEASE, data2, 0, size2);
    return ret1 | ret2;
}

static inline int spi_send_then_recv(spi_device_t *dev, const void *send_data, size_t send_size, void *recv_data, size_t recv_size)
{
    int ret1 = dev->bus->transfer(dev, SPI_FLAG_CS_TAKE, send_data, 0, send_size);
    int ret2 = dev->bus->transfer(dev, SPI_FLAG_CS_RELEASE, 0, recv_data, recv_size);
    return ret1 | ret2;
}

#endif
