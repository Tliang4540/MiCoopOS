/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __DEVICE_H__
#define __DEVICE_H__

#include "mctypes.h"

struct device;
struct device_ops
{
    int (*open)(struct device *dev);
    int (*close)(struct device *dev);
    int (*write)(struct device *dev, const void *buf, size_t size);
    int (*read)(struct device *dev, void *buf, size_t size);
    int (*ioctrl)(struct device *dev, int cmd, void *args);
};
typedef struct device_ops device_ops_t;

struct device
{
    mc_slist_t list_node;
    const char *name;
    const struct device_ops *ops;
    void *user_data;
};
typedef struct device device_t;

static inline int device_open(device_t *dev)
{
    return dev->ops->open(dev);
}

static inline int device_close(device_t *dev)
{
    return dev->ops->close(dev);
}

static inline int device_write(device_t *dev, const void *buf, size_t size)
{
    return dev->ops->write(dev, buf, size);
}

static inline int device_read(device_t *dev, void *buf, size_t size)
{
    return dev->ops->read(dev, buf, size);
}

static inline int device_ioctrl(device_t *dev, int cmd, void *args)
{
    return dev->ops->ioctrl(dev, cmd, args);
}

void device_add(device_t *dev, const char *name);
device_t *device_find(const char *name);

#endif
