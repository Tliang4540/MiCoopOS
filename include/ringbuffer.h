/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __RINGBUFFER_H__
#define __RINGBUFFER_H__

#include <stddef.h>

struct ringbuffer
{
    size_t w;
    size_t r;
    size_t size;
    void *buff;
};
typedef struct ringbuffer rb_t;

/**
 * @brief 环形缓冲区初始化
 * @param rb 需要初始化的环形缓冲区
 * @param buff 环形缓冲区指向的一块内存
 * @param buffsize 内存大小
*/
void rb_init(rb_t *rb, void *buff, size_t buffsize);

/**
 * @brief 从环形缓冲区读取数据
 * @param rb 需要读取的环形缓冲区
 * @param des 读出数据存放位置
 * @param size 读取多少字节
 * @retval 实际读取到多少字节
*/
size_t rb_read(rb_t *rb, void *des, size_t size);

/**
 * @brief 写入数据到环形缓冲区
 * @param rb 需要写入的环形缓冲区
 * @param src 写入的数据位置
 * @param size 写入多少字节
 * @retval 实际写入了多少字节
*/
size_t rb_write(rb_t *rb, const void *src, size_t size);

/**
 * @brief 获取环形缓冲区有多少数据
 * @param rb 需要获取的环形缓冲区
 * @retval 已经存放数据大小
*/
size_t rb_get_full(rb_t *rb);

#endif
