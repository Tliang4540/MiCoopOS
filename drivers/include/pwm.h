/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __PWM_H__
#define __PWM_H__

typedef struct
{
    void *user_data;
}pwm_handle_t;

void pwm_handle_init(pwm_handle_t *pwm_handle, unsigned int pwm_id);
void pwm_open(pwm_handle_t *pwm_handle);
void pwm_set(pwm_handle_t *pwm_handle, unsigned int channel, unsigned int period, unsigned int pulse);
void pwm_set_pulse(pwm_handle_t *pwm_handle, unsigned int channel, unsigned int pulse);
void pwm_enable(pwm_handle_t *pwm_handle, unsigned int channel);
void pwm_disable(pwm_handle_t *pwm_handle, unsigned int channel);
void pwm_close(pwm_handle_t *pwm_handle);

#endif
