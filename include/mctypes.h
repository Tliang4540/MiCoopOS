/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __MCTYPES_H__
#define __MCTYPES_H__

#include <stdint.h>
#include <stddef.h>

typedef int                 mc_bool_t;
typedef signed long         mc_base_t;
typedef unsigned long       mc_ubase_t;
typedef mc_ubase_t          mc_tick_t;
typedef mc_base_t           mc_err_t;

#define MC_TICK_MAX         ((mc_tick_t)((mc_tick_t)-1 / 2))
#define MC_MSG_INVALID_VAL  ((mc_ubase_t)-1)
#define MC_STACK_FILL_VAL   (0xA5)

#define MC_TRUE             (1)
#define MC_FALSE            (0)

#define MC_EOK              (0)
#define MC_ERROR            (-1)

#define MC_UNUSED(x)        ((void)x)

struct mc_list_node
{
    struct mc_list_node *next;
    struct mc_list_node *prev;
};
typedef struct mc_list_node mc_list_t;

struct mc_slist_node
{
    struct mc_slist_node *next;
};
typedef struct mc_slist_node mc_slist_t;

struct mc_object
{
    struct mc_slist_node list_node;
    const char *name;
};
typedef struct mc_object mc_object_t;

struct mc_tm
{
    unsigned int sec;
    unsigned int min;
    unsigned int hour;
    unsigned int wday;
    unsigned int mday;
    unsigned int mon;
    unsigned int year;
};
typedef struct mc_tm mc_tm_t;

#endif
