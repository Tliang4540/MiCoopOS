/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __MICRODB_H__
#define __MICRODB_H__

typedef struct
{
    void *user_data;
} mdb_t;

#ifndef MDB_GRAIN_SIZE
#define MDB_GRAIN_SIZE    (8)
#endif

#ifndef MDB_DATA_MAX_SIZE
#define MDB_DATA_MAX_SIZE (256)
#endif

#if (MDB_GRAIN_SIZE < 8)
typedef unsigned int mdb_grain_t;
typedef unsigned short mdb_half_grain_t;
#else
typedef unsigned long long mdb_grain_t;
typedef unsigned int mdb_half_grain_t;
#endif

typedef struct
{
    mdb_grain_t is_full;
}mdb_sec_status_t;

typedef struct
{
    mdb_half_grain_t id;              //节点ID, 用于查找数据
    mdb_half_grain_t size;            //节点数据大小
    mdb_grain_t written;              //数据已写入
    mdb_grain_t deleting;             //数据正在删除
    mdb_grain_t deleted;              //数据已被删除
    unsigned char data[];            //节点数据
} mdb_node_t;

/**
 * @brief 查找回调函数，用于查找数据的匹配方法
 * @param user_data 用户数据
 * @param node 数据节点
 * @return 1:匹配 0:不匹配
 */
typedef unsigned int (*mdb_find_callback_t)(const void *arg, const mdb_node_t *node);

void mdb_init(mdb_t *db, void *flash_dev);
unsigned int mdb_find_node_by_callback(mdb_t *db, mdb_node_t *node, const void *arg, mdb_find_callback_t callback);
unsigned int mdb_write(mdb_t *db, unsigned int id, const void *data, unsigned int size);
unsigned int mdb_read(mdb_t *db, unsigned int id, void *data);
void mdb_delete(mdb_t *db, unsigned int id);
void mdb_format(mdb_t *db);

#endif
