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

static inline int i2c_transfer(struct i2c_device *dev, unsigned int flag, void *data, size_t size)
{
    return dev->bus->transfer(dev, flag, data, size);
}

static inline void i2c_device_init(i2c_device_t *dev, i2c_bus_t *bus, unsigned int dev_addr)
{
    dev->bus = bus;
    dev->addr = dev_addr;
}

static inline int i2c_master_write_reg8(i2c_device_t *dev, uint8_t reg, void *data, size_t size)
{
    int ret1 = dev->bus->transfer(dev, I2C_FLAG_WR | I2C_FLAG_NO_STOP, &reg, 1);
    int ret2 = dev->bus->transfer(dev, I2C_FLAG_NO_START, data, size);
    return ret1 | ret2;
}

static inline int i2c_master_read_reg8(i2c_device_t *dev, uint8_t reg, void *data, size_t size)
{
    int ret1 = dev->bus->transfer(dev, I2C_FLAG_WR | I2C_FLAG_NO_STOP, &reg, 1);
    int ret2 = dev->bus->transfer(dev, I2C_FLAG_RD, data, size);
    return ret1 | ret2;
}

static inline int i2c_master_write_reg16(i2c_device_t *dev, uint16_t reg, void *data, size_t size)
{
    uint8_t reg_addr[2] = {reg >> 8, reg};
    int ret1 = dev->bus->transfer(dev, I2C_FLAG_WR | I2C_FLAG_NO_STOP, reg_addr, 2);
    int ret2 = dev->bus->transfer(dev, I2C_FLAG_NO_START, data, size);
    return ret1 | ret2;
}

static inline int i2c_master_read_reg16(i2c_device_t *dev, uint8_t reg, void *data, size_t size)
{
    uint8_t reg_addr[2] = {reg >> 8, reg};
    int ret1 = dev->bus->transfer(dev, I2C_FLAG_WR | I2C_FLAG_NO_STOP, reg_addr, 2);
    int ret2 = dev->bus->transfer(dev, I2C_FLAG_RD, data, size);
    return ret1 | ret2;
}

#endif
