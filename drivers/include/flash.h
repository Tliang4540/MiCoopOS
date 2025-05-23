/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __FLASH_H__
#define __FLASH_H__

#include <mctypes.h>

struct flash_device;
struct flash_device_ops
{
    int (*open)(struct flash_device *dev);
    int (*close)(struct flash_device *dev);
    int (*write)(struct flash_device *dev, unsigned int offset, const void *buf, size_t size);
    int (*read)(struct flash_device *dev, unsigned int offset, void *buf, size_t size);
    int (*erase)(struct flash_device *dev, unsigned int offset, size_t size);
};

typedef struct flash_device
{
    struct mc_object parent;
    unsigned int base_addr;
    unsigned int size;
    unsigned int sec_size;
    unsigned int write_grain;
    const struct flash_device_ops *ops;
    void *user_data;
}flash_device_t;

static inline int flash_open(struct flash_device *dev)
{
    return dev->ops->open(dev);
}

static inline int flash_close(struct flash_device *dev)
{
    return dev->ops->close(dev);
}

static inline int flash_read(struct flash_device *dev, unsigned int offset, void *buf, unsigned int size)
{
    return dev->ops->read(dev, offset, buf, size);
}

static inline int flash_write(struct flash_device *dev, unsigned int offset, const void *buf, unsigned int size)
{
    return dev->ops->write(dev, offset, buf, size);
}

static inline int flash_erase(struct flash_device *dev, unsigned int offset, unsigned int size)
{
    return dev->ops->erase(dev, offset, size);
}

static inline unsigned int flash_get_sec_size(struct flash_device *dev)
{
    return dev->sec_size;
}

static inline unsigned int flash_get_size(struct flash_device *dev)
{
    return dev->size;
}

static inline unsigned int flash_get_write_grain(struct flash_device *dev)
{
    return dev->write_grain;
}

void onchip_flash_device_init(flash_device_t *dev, unsigned int base_addr, unsigned int size);

#endif
