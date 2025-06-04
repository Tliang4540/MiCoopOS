/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __AUDIO_H__
#define __AUDIO_H__

#include <mctypes.h>

typedef void (*audio_callback_t)(void *buffer, size_t size);

struct audio_device;
struct audio_device_ops
{
    int (*open)(struct audio_device *dev, unsigned int sample_rate);
    int (*close)(struct audio_device *dev);
    int (*play)(struct audio_device *dev, audio_callback_t callback);
    int (*stop)(struct audio_device *dev);
};

typedef struct audio_device
{
    struct mc_object parent;
    const struct audio_device_ops *ops;
    unsigned int sample_bits;
    void *user_data;
}audio_device_t;

static inline int audio_open(audio_device_t *dev, unsigned int sample_rate)
{
    return dev->ops->open(dev, sample_rate);
}

static inline int audio_close(audio_device_t *dev)
{
    return dev->ops->close(dev);
}

static inline int audio_play(audio_device_t *dev, audio_callback_t callback)
{
    return dev->ops->play(dev, callback);
}

static inline int audio_stop(audio_device_t *dev)
{
    return dev->ops->stop(dev);
}

static inline int audio_get_sample_bits(audio_device_t *dev)
{
    return dev->sample_bits;
}

void audio_device_init(audio_device_t *dev, unsigned int dac_id, unsigned int channel, unsigned int en_pin);

#endif
