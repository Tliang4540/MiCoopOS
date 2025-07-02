/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __MCOS_H__
#define __MCOS_H__

#include "mctypes.h"

#define MC_TICK_MAX         ((mc_tick_t)((mc_tick_t)-1 / 2))
#define MC_MSG_INVALID_VAL  ((mc_ubase_t)-1)
#define MC_STACK_FILL_VAL   (0xA5)

struct mc_msg
{
    void *task;
    volatile mc_ubase_t data;
};
typedef struct mc_msg mc_msg_t;

void mc_task_init(void (*task)(void *), void *arg, void *stack, size_t stack_size);
void mc_task_delete(void *task);
void mc_delay(mc_ubase_t ticks);
void mc_tick_handler(void);
void mc_start(void);

mc_ubase_t mc_get_tick(void);
mc_ubase_t mc_task_get_free_stack(void);

mc_bool_t mc_msg_recv(mc_msg_t *msg, mc_ubase_t *data, mc_ubase_t timeout);
void mc_msg_send(mc_msg_t *msg, mc_ubase_t data);

static inline void mc_msg_init(mc_msg_t *msg)
{
    msg->task = NULL;
    msg->data = MC_MSG_INVALID_VAL;
}

#endif
