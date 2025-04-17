/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __I2C_H__
#define __I2C_H__

typedef struct i2c_handle
{
    unsigned short dev_addr;
    unsigned short reserved;
    void *user_data;
}i2c_handle_t;

void i2c_init(i2c_handle_t *dev, unsigned int i2c_id, unsigned int dev_addr, unsigned int freq);
int i2c_write(i2c_handle_t *dev, const void *data, unsigned int size);
int i2c_read(i2c_handle_t *dev, const void *addr, unsigned int addr_size, void *data, unsigned int size);

#endif
