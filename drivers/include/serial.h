/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __SERIAL_H__
#define __SERIAL_H__

typedef void* serial_handle_t;

serial_handle_t serial_init(unsigned int serial_id);
void serial_open(serial_handle_t serial, unsigned int baudrate);
void serial_close(serial_handle_t serial);
void serial_write(serial_handle_t serial, const void *data, unsigned int size);
unsigned int serial_read(serial_handle_t serial, void *data, unsigned int size);

#endif
