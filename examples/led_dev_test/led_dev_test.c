/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#include <device.h>
#include <pin.h>
#include <mcos.h>

static int led_open(struct device *dev)
{
    pin_mode((int)dev->user_data, PIN_MODE_OUTPUT_PP);
    return 0;
}

static int led_write(struct device *dev, const void *data, unsigned size)
{
    (void)(size);
    pin_write((int)dev->user_data, *(unsigned char*)data);

    return 0;
}

static const struct device_ops led_ops = {
    .open = led_open,
    .write = led_write,
};

static struct device led_dev = {
    .ops = &led_ops,
    .user_data = (void*)8
};

static uint32_t led_task_stack[64];
static void led_task(void *arg)
{
    device_t *led_dev;
    MC_UNUSED(arg);
    led_dev = (device_t*)device_find("led");
    device_open(led_dev);

    while (1)
    {
        device_write(led_dev, "\x01", 1);
        mc_delay(100);
        device_write(led_dev, "\x00", 1);
        mc_delay(400);
    }
}

void led_dev_test(void)
{
    device_add((mc_object_t*)&led_dev, "led");

    mc_task_init(led_task, NULL, led_task_stack, sizeof(led_task_stack));
}
