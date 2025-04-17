/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __WDT_H__
#define __WDT_H__

void wdt_init(unsigned int period);
void wdt_reset(void);

#endif
