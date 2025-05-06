/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __SYSTICK_H__
#define __SYSTICK_H__

/**
 * @brief systick init
 * @param period systick period(us)
 * @param callback systick callback
 */
void systick_init(unsigned int period, void (*callback)(void));

#endif
