/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __I2C_H__
#define __I2C_H__

#include <mctypes.h>

#define I2C_FLAG_WR          0
#define I2C_FLAG_RD          1
#define I2C_FLAG_NO_START    2
#define I2C_FLAG_NO_STOP     4

struct i2c_bus;
typedef struct i2c_device
{
    struct i2c_bus *bus;
    unsigned int addr;
}i2c_device_t;

typedef struct i2c_bus
{
    int (*transfer)(struct i2c_device *dev, unsigned int flag, void *data, size_t size);
    void *user_data;
}i2c_bus_t;

void i2c_bus_init(i2c_bus_t *bus, unsigned int i2c_id, unsigned int freq);

static inline int i2c_device_transfer(struct i2c_device *dev, unsigned int flag, void *data, size_t size)
{
    return dev->bus->transfer(dev, flag, data, size);
}

static inline void i2c_device_init(i2c_device_t *dev, i2c_bus_t *bus, unsigned int dev_addr)
{
    dev->bus = bus;
    dev->addr = dev_addr;
}

#endif
