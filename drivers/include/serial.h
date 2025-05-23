/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __SERIAL_H__
#define __SERIAL_H__

#include <device.h>

static inline int serial_open(device_t *dev)
{
    return dev->ops->open(dev);
}

static inline int serial_close(device_t *dev)
{
    return dev->ops->close(dev);
}

static inline int serial_write(device_t *dev, const void *buf, size_t size)
{
    return dev->ops->write(dev, buf, size);
}

static inline int serial_read(device_t *dev, void *buf, size_t size)
{
    return dev->ops->read(dev, buf, size);
}

void serial_device_init(device_t *dev, unsigned int serial_id, unsigned int baudrate);

#endif
