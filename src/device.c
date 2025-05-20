/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#include <device.h>
#include <string.h>

static mc_object_t *device_list_head;
static mc_object_t *device_list_tail;

mc_object_t *device_find(const char *name)
{
    mc_object_t *dev = device_list_head;

    while(dev != NULL)
    {
        if (strcmp(dev->name, name) == 0)
        {
            return dev;
        }
        dev = (mc_object_t*)dev->list_node.next;
    }

    return NULL;
}

void device_add(mc_object_t *dev, const char *name)
{
    dev->name = name;

    if (device_list_head == NULL)
    {
        device_list_head = dev;
        device_list_tail = dev;
    }
    else
    {
        device_list_tail->list_node.next = (mc_slist_t*)dev;
        device_list_tail = dev;
    }
    device_list_tail->list_node.next = NULL;
}
