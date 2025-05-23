/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __PWM_H__
#define __PWM_H__

#include <mctypes.h>

struct pwm_device;
struct pwm_device_ops {
    int (*open)(struct pwm_device *dev);
    int (*close)(struct pwm_device *dev);
    int (*set_period)(struct pwm_device *dev, unsigned int period);
    int (*set_channel_pulse)(struct pwm_device *dev, unsigned int channel, unsigned int pulse);
    int (*enable_channel)(struct pwm_device *dev, unsigned int channel);
    int (*disable_channel)(struct pwm_device *dev, unsigned int channel);
};

typedef struct pwm_device {
    struct mc_object parent;
    const struct pwm_device_ops *ops;
    void *user_data;
} pwm_device_t;

static inline int pwm_open(pwm_device_t *dev)
{
    return dev->ops->open(dev);
}

static inline int pwm_close(pwm_device_t *dev)
{
    return dev->ops->close(dev);
}

static inline int pwm_set_period(pwm_device_t *dev, unsigned int period)
{
    return dev->ops->set_period(dev, period);
}

static inline int pwm_set_channel_pulse(pwm_device_t *dev, unsigned int channel, unsigned int pulse)
{
    return dev->ops->set_channel_pulse(dev, channel, pulse);
}

static inline int pwm_enable_channel(pwm_device_t *dev, unsigned int channel)
{
    return dev->ops->enable_channel(dev, channel);
}

static inline int pwm_disable_channel(pwm_device_t *dev, unsigned int channel)
{
    return dev->ops->disable_channel(dev, channel);
}

void pwm_device_init(pwm_device_t *dev, unsigned int pwm_id);

#endif
