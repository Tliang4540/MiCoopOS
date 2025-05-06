/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __I2C_H__
#define __I2C_H__

#include <mctypes.h>

typedef struct i2c_handle
{
    unsigned short dev_addr;
    unsigned short reserved;
    void *user_data;
}i2c_handle_t;

void i2c_handle_init(i2c_handle_t *i2c_handle, unsigned int i2c_id, unsigned int dev_addr);
void i2c_open(i2c_handle_t *i2c_handle, unsigned int freq);
mc_err_t i2c_write(i2c_handle_t *i2c_handle, const void *data, unsigned int size);
mc_err_t i2c_read(i2c_handle_t *i2c_handle, const void *addr, unsigned int addr_size, void *data, unsigned int size);

#endif
