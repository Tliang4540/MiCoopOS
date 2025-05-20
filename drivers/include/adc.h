/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __ADC_H__
#define __ADC_H__

#include <mctypes.h>

struct adc_device;
struct adc_device_ops
{
    int (*open)(struct adc_device *dev);
    int (*close)(struct adc_device *dev);
    int (*read)(struct adc_device *dev);
    int (*set_channel)(struct adc_device *dev, unsigned int channel);
};

typedef struct adc_device
{
    struct mc_object object;
    const struct adc_device_ops *ops;
    void *user_data;
}adc_device_t;

static inline int adc_open(adc_device_t *dev)
{
    return dev->ops->open(dev);
}

static inline int adc_close(adc_device_t *dev)
{
    return dev->ops->close(dev);
}

static inline int adc_read(adc_device_t *dev)
{
    return dev->ops->read(dev);
}

static inline int adc_set_channel(adc_device_t *dev, unsigned int channel)
{
    return dev->ops->set_channel(dev, channel);
}

void adc_device_init(adc_device_t *dev, unsigned int adc_id);

#endif
