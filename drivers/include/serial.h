/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __SERIAL_H__
#define __SERIAL_H__

typedef struct 
{
    void *user_data;
}serial_handle_t;

void serial_handle_init(serial_handle_t *serial_handle, unsigned int serial_id);
void serial_open(serial_handle_t *serial_handle, unsigned int baudrate);
void serial_close(serial_handle_t *serial_handle);
void serial_write(serial_handle_t *serial_handle, const void *data, unsigned int size);
unsigned int serial_read(serial_handle_t *serial_handle, void *data, unsigned int size);

#endif
