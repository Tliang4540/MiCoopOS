/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __TINYDB_H__
#define __TINYDB_H__

typedef struct
{
    void *user_data;
} tinydb_t;

#define TINYDB_GRAIN_SIZE    (8)
#define TINYDB_DATA_MAX_SIZE (256)

#if (TINYDB_GRAIN_SIZE < 8)
typedef unsigned int db_grain_t;
typedef unsigned short db_half_grain_t;
#else
typedef unsigned long long db_grain_t;
typedef unsigned int db_half_grain_t;
#endif

typedef struct
{
    db_grain_t is_full;
}db_sec_status_t;

typedef struct
{
    db_half_grain_t id;              //节点ID, 用于查找数据
    db_half_grain_t size;            //节点数据大小
    db_grain_t written;              //数据已写入
    db_grain_t deleting;             //数据正在删除
    db_grain_t deleted;              //数据已被删除
    unsigned char data[];            //节点数据
} db_node_t;

/**
 * @brief 查找回调函数，用于查找数据的匹配方法
 * @param user_data 用户数据
 * @param node 数据节点
 * @return 1:匹配 0:不匹配
 */
typedef unsigned int (*find_callback_t)(const void *user_data, const db_node_t *);

void tinydb_init(tinydb_t *db, void *user_data);
unsigned int tinydb_find_node_by_callback(tinydb_t *db, db_node_t *db_node, const void *user_data, find_callback_t callback);
unsigned int tinydb_write(tinydb_t *db, unsigned int id, const void *data, unsigned int size);
unsigned int tinydb_read(tinydb_t *db, unsigned int id, void *data);
void tinydb_delete(tinydb_t *db, unsigned int id);
void tinydb_format(tinydb_t *db);

#endif
