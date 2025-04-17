/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __SERIAL_H__
#define __SERIAL_H__

typedef void* serial_handle_t;

serial_handle_t serial_open(unsigned int serial_id, unsigned int baudrate);
void serial_close(serial_handle_t serial);
void serial_write(serial_handle_t serial, const void *data, unsigned int size);
unsigned int serial_read(serial_handle_t serial, void *data, unsigned int size);

#endif
