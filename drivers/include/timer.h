/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __TIMER_H__
#define __TIMER_H__

#include <mctypes.h>

struct timer_device;
struct timer_device_ops
{
    int (*start)(struct timer_device *dev, unsigned int interval);
    int (*stop)(struct timer_device *dev);
    unsigned int (*read)(struct timer_device *dev);
    int (*set_callback)(struct timer_device *dev, void (*hdr)(void));
};

typedef struct timer_device
{
    struct mc_object parent;
    const struct timer_device_ops *ops;
    void *user_data;
}timer_device_t;

static inline int timer_start(timer_device_t *dev, unsigned int interval)
{
    return dev->ops->start(dev, interval);
}

static inline int timer_stop(timer_device_t *dev)
{
    return dev->ops->stop(dev);
}

static inline unsigned int timer_read(timer_device_t *dev)
{
    return dev->ops->read(dev);
}

void timer_device_init(timer_device_t *dev, unsigned int timerid, unsigned int freq);

#endif
