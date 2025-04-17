/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#include <mcos.h>
#include <string.h>
#include <log.h>
#include <mcconfig.h>

typedef struct mc_tcb
{
    mc_slist_t list_node;
    volatile mc_tick_t tick;
    volatile mc_ubase_t state;
    void *stack;
#if defined(MC_TASK_GET_FREE_STACK_ENABLE)
    void *stack_end;
#endif
}mc_tcb_t;

void *_mc_cur_sp;    // 当前使用的任务栈
static mc_tcb_t *mc_cur_task; // 当前任务
static mc_tcb_t *mc_task_head; // 任务链表头
static mc_tcb_t *mc_task_tail; // 任务链表尾
static volatile mc_ubase_t mc_sys_tick = 0;        //系统时间

void mc_tick_handler(void)
{
    mc_sys_tick++;
}

void _mc_yield(void);
void _mc_start(void);
void *_mc_stack_init(void (*func)(void *), void *param, void *stack, size_t stack_size);

/**
 * @brief 更新就绪任务栈指针
 */
void mc_update_sp(void)
{
    mc_cur_task->stack = _mc_cur_sp;

    while(1)
    {
        mc_cur_task = (mc_tcb_t*)mc_cur_task->list_node.next;
        if(mc_cur_task->state == 0)
            continue;
        
        if ((mc_sys_tick - mc_cur_task->tick) < (unsigned int)(MC_TICK_MAX))
            break;
    }

    _mc_cur_sp = mc_cur_task->stack;
}

void mc_task_init(void (*task)(void *), void *arg, void *stack, size_t stack_size)
{
    mc_tcb_t *new_task = stack;
    stack = (void*)((uint8_t*)stack + sizeof(mc_tcb_t));
    stack_size -= sizeof(mc_tcb_t);

    // 初始化任务栈
    #if defined(MC_TASK_GET_FREE_STACK_ENABLE)
    new_task->stack_end = stack;
    memset(stack, MC_STACK_FILL_VAL, stack_size);
    #endif
    new_task->stack = _mc_stack_init(task, arg, stack, stack_size);
    new_task->tick = mc_sys_tick;
    new_task->state = 1;

    // 将任务添加到链表
    if (mc_task_head == NULL)
    {
        mc_task_head = new_task;
        mc_task_tail = new_task;
        mc_cur_task = new_task;
    }
    else
    {
        mc_task_tail->list_node.next = (void*)new_task;
        mc_task_tail = new_task;
    }

    mc_task_tail->list_node.next = (void*)mc_task_head;
}

void mc_start(void)
{
    LOG_ASSERT(mc_cur_task != 0);
    _mc_cur_sp = mc_cur_task->stack;
    _mc_start();
}

void mc_delay(mc_ubase_t ticks)
{
    LOG_ASSERT(ticks < MC_TICK_MAX);
    mc_cur_task->tick = mc_sys_tick + ticks;
    _mc_yield();
}

mc_ubase_t mc_get_tick(void)
{
    return mc_sys_tick;
}

#if defined(MC_TASK_GET_FREE_STACK_ENABLE)
mc_ubase_t mc_task_get_free_stack(void)
{
    uint8_t *stack_end = mc_cur_task->stack_end;
    size_t free_size = 0;
    
    while (*stack_end++ == MC_STACK_FILL_VAL)
        free_size++;

    return free_size;
}
#endif

void mc_msg_send(mc_msg_t *msg, mc_ubase_t data)
{
    msg->data = data;
    if (msg->task)
    {
        ((mc_tcb_t*)msg->task)->state = 1;
        ((mc_tcb_t*)msg->task)->tick = mc_sys_tick;
    }
}

mc_bool_t mc_msg_recv(mc_msg_t *msg, mc_ubase_t *data, mc_ubase_t timeout)
{
    mc_bool_t ret = MC_TRUE;
    
    //先设置等待参数
    msg->task = mc_cur_task;
    if (timeout < MC_TICK_MAX)
    {
        mc_cur_task->tick = mc_sys_tick + timeout;
    }
    else
    {
        mc_cur_task->state = 0;
    }

    // 如果没有消息切换任务
    if (msg->data == MC_MSG_INVALID_VAL)
    {
        _mc_yield();
        if (msg->data == MC_MSG_INVALID_VAL)
        {
            ret = MC_FALSE;
        }
        else if (data)
        {
            *data = msg->data;   
        }
    }
    else if (data)
    {
        *data = msg->data;
        mc_cur_task->state = 1;
    }

    msg->data = MC_MSG_INVALID_VAL;
    msg->task = NULL;
    return ret;
}
