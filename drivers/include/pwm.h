/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __PWM_H__
#define __PWM_H__

typedef void *pwm_handle_t;

pwm_handle_t pwm_open(unsigned int pwmid);
void pwm_set(pwm_handle_t pwm, unsigned int channel, unsigned int period, unsigned int pulse);
void pwm_enable(pwm_handle_t pwm, unsigned int channel);
void pwm_disable(pwm_handle_t pwm, unsigned int channel);
void pwm_close(pwm_handle_t pwm);

#endif
