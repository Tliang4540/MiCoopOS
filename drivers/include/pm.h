/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __PM_H__
#define __PM_H__

typedef enum 
{
    PM_MODE_RUN = 0,
    PM_MODE_SLEEP,
    PM_MODE_STANDBY
}pm_mode_t;

void pm_set_mode(pm_mode_t mode);
void pm_enter_sleep(void);

#endif
