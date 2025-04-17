/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __PIN_H__
#define __PIN_H__

typedef enum
{
    PIN_MODE_NONE = 0,
    PIN_MODE_INPUT,
    PIN_MODE_OUTPUT_PP,
    PIN_MODE_OUTPUT_OD,
    PIN_MODE_FUNCTION_PP,
    PIN_MODE_FUNCTION_OD,
}pin_mode_t;

typedef enum
{
    PIN_IRQ_MODE_DISABLE = 0,
    PIN_IRQ_MODE_RISING,
    PIN_IRQ_MODE_FALLING,
    PIN_IRQ_MODE_RISING_FALLING,
    PIN_IRQ_MODE_LOW_LEVEL,
    PIN_IRQ_MODE_HIGH_LEVEL,
}pin_irq_mode_t;

typedef enum
{
    PIN_PULL_NO = 0,
    PIN_PULL_UP,
    PIN_PULL_DOWN,
}pin_pull_t;

typedef enum
{
    PIN_SPEED_LOW = 0,
    PIN_SPEED_MEDIUM,
    PIN_SPEED_HIGH,
    PIN_SPEED_VERY_HIGH
}pin_speed_t;

void pin_mode(unsigned int pin, pin_mode_t mode);
void pin_pull(unsigned int pin, pin_pull_t pull);
void pin_write(unsigned int pin, unsigned int value);
int pin_read(unsigned int pin);
void pin_attach_irq(unsigned int pin, void (*callback)(void), pin_irq_mode_t mode);
void pin_function(unsigned int pin, unsigned int func);
void pin_speed(unsigned int pin, unsigned int speed);

#endif
