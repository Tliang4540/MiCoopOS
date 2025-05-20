/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __MCLIB_H__
#define __MCLIB_H__

#include "mctypes.h"

unsigned int mc_utoa(unsigned int val, char *str, unsigned int radix);

void mc_localtime(unsigned int time, struct mc_tm *t);
unsigned int mc_mktime(struct mc_tm *time);
unsigned char mc_bin2bcd(unsigned int val);
unsigned int mc_bcd2bin(unsigned char val);

#endif
