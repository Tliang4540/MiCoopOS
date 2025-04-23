/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#include "microdb.h"
#include <stddef.h>
#include <flash.h>
#include <string.h>
#include <log.h>

#define DB_LOG_ENABLE 1
#if DB_LOG_ENABLE
#define DB_LOG(fmt, ...)    LOG_OUT("[DB]:"fmt"\n", ##__VA_ARGS__)
#else
#define DB_LOG(fmt, ...)
#endif

#define DB_EMPTY_DATA       (-1)
#define DB_FILL_DATA        (0xA5A5A5A5A5A5A5A5)

static unsigned int find_by_id(const void *arg, const mdb_node_t *node)
{
    if((node->written != (mdb_grain_t)DB_EMPTY_DATA) && //有正确写入的数据
       (node->id == *(mdb_half_grain_t*)arg))     //id相同
    {
        return 1;
    }
    return 0;
}

static unsigned int find_by_id_nodetect(const void *arg, const mdb_node_t *node)
{
    if(node->id == *(mdb_half_grain_t*)arg)    //fd相同
    {
        return 1;
    }
    return 0;
}

/**
 * @brief  通过回调查找数据，并将节点数据和地址返回
 * @param [in] db 数据库
 * @param [out] db_node 找到的数据节点数据
 * @param [in] user_data 用户数据
 * @param [in] callback 查找回调函数
 * @return 数据节点地址
 */
unsigned int mdb_find_node_by_callback(mdb_t *db, mdb_node_t *node, const void *arg, mdb_find_callback_t callback)
{
    for (unsigned int i = 0; i < flash_get_size(db->user_data); i += flash_get_sec_size(db->user_data))
    {
        for (unsigned int n = sizeof(mdb_sec_status_t); n < (flash_get_sec_size(db->user_data) - sizeof(mdb_node_t)); n += sizeof(mdb_node_t))
        {
            flash_read(db->user_data, i + n, node, sizeof(mdb_node_t));
            if (node->id == (mdb_half_grain_t)DB_EMPTY_DATA)
                break;

            if ((node->deleted == (mdb_grain_t)DB_EMPTY_DATA) &&  //数据未被删除
                (node->deleting == (mdb_grain_t)DB_EMPTY_DATA) && //数据未准备删除
                callback(arg, node))
            {
                return i + n;       //返回找到的地址
            }

            if (node->size <= MDB_DATA_MAX_SIZE)
            {
                n += node->size; // 跳过数据长度
            }
            else
            {
                DB_LOG("find error, size:%d\n", node->size);
            }
        }
    }

    return (unsigned int)DB_EMPTY_DATA;
}

/**
 * @brief 获取一段可用空间的起始地址
 * @param [in] db 数据库
 * @param [in] size 需要的大小
 * @param [in] skip 不检索的扇区
 * @retval DB_EMPTY_DATA: 获取失败
 *         other: 起始地址
 */
static unsigned int get_empty_addr(mdb_t *db, unsigned int size, unsigned int skip)
{
    unsigned int empty_addr0 = (unsigned int)DB_EMPTY_DATA; // 空扇区地址和可用地址
    unsigned int empty_addr1 = (unsigned int)DB_EMPTY_DATA; // 空扇区地址和可用地址
    mdb_half_grain_t buf[2];
    // 找到可用空间
    for (unsigned int i = 0; i < flash_get_size(db->user_data); i += flash_get_sec_size(db->user_data))
    {
        flash_read(db->user_data, i, buf, sizeof(mdb_half_grain_t));
        if ((buf[0] != (mdb_half_grain_t)DB_EMPTY_DATA) || (i == skip)) // 如果扇区已满则跳过
        {
            continue;
        }
        // 扇区空间是否足够
        for (unsigned int n = sizeof(mdb_sec_status_t); n < (flash_get_sec_size(db->user_data) - sizeof(mdb_node_t)); n += sizeof(mdb_node_t))
        {
            flash_read(db->user_data, i + n, buf, sizeof(mdb_grain_t));
            if (buf[1] == (mdb_half_grain_t)DB_EMPTY_DATA) // 跳过空扇区，先使用已使用扇区
            {
                if (n == sizeof(mdb_sec_status_t))
                {
                    if (empty_addr0 == (unsigned int)DB_EMPTY_DATA)
                        empty_addr0 = i + sizeof(mdb_sec_status_t);
                    else if (empty_addr1 == (unsigned int)DB_EMPTY_DATA) // 要确保至少留有一块空扇区
                        empty_addr1 = i + sizeof(mdb_sec_status_t);
                }
                else if ((flash_get_sec_size(db->user_data) - n) >= (size + sizeof(mdb_node_t)))
                {
                    return i + n;
                }
                break;
            }

            if (buf[1] <= MDB_DATA_MAX_SIZE)
            {
                n += buf[1]; // 跳过数据长度
            }
            else
            {
                DB_LOG("empty error, size:%d\n", buf[1]);
            }
        }
    }

    return empty_addr1;
}

/**
 * @brief 设置节点写入完成状态，同时设置旧数据删除状态
 * @param [in] db 数据库
 * @param [in] new_addr 新数据地址
 * @param [in] old_addr 旧数据地址
 */
static void set_node_written_status(mdb_t *db, unsigned int new_addr, unsigned int old_addr)
{
    mdb_grain_t status = DB_FILL_DATA;
    if (old_addr == (unsigned int)DB_EMPTY_DATA)
    {
        flash_write(db->user_data, new_addr + offsetof(mdb_node_t, written), &status, sizeof(mdb_grain_t));
        return;
    }
    flash_write(db->user_data, old_addr + offsetof(mdb_node_t, deleting), &status, sizeof(mdb_grain_t));
    flash_write(db->user_data, new_addr + offsetof(mdb_node_t, written), &status, sizeof(mdb_grain_t));
    flash_write(db->user_data, old_addr + offsetof(mdb_node_t, deleted), &status, sizeof(mdb_grain_t));
}

/**
 * @brief 设置节点写入中状态, 同时更新扇区已满状态
 * @param [in] db 数据库
 * @param [in] addr 节点地址
 * @param [in] status 状态
 */
static void set_note_writing_status(mdb_t *db, unsigned int addr, mdb_node_t *node)
{
    unsigned int sec_size = flash_get_sec_size(db->user_data);
    unsigned int sec_mask = sec_size - 1;

    // 如果剩余空间无法容纳一个节点设置扇区已满
    if ((sec_mask & addr) + node->size + sizeof(mdb_node_t) + sizeof(mdb_node_t) + sizeof(mdb_grain_t) > sec_size)
    {
        unsigned int sec_addr = addr & ~sec_mask;
        mdb_grain_t status = DB_FILL_DATA;
        flash_write(db->user_data, sec_addr, &status, sizeof(mdb_grain_t));
        DB_LOG("sector [%x] full.", sec_addr);
    }
    flash_write(db->user_data, addr + offsetof(mdb_node_t, id), &node->id, sizeof(mdb_grain_t));
}

/**
 * @brief 垃圾回收，找到删除数据最多的扇区将数据移出后擦除
*/
static unsigned int mdb_defrag(mdb_t *db)
{
    mdb_node_t node;
    unsigned int full_min = (unsigned int)DB_EMPTY_DATA;
    unsigned int empty_addr = (unsigned int)DB_EMPTY_DATA;   //空扇区地址
    unsigned int erase_addr = (unsigned int)DB_EMPTY_DATA;   //擦除的地址
    
    unsigned int del_max = 0;   //删除最多的数量
    unsigned int del_addr = (unsigned int)DB_EMPTY_DATA;  //删除最多的地址

    // 1. 查找垃圾数据最多的扇区和空扇区
    for (unsigned int i = 0; i < flash_get_size(db->user_data); i += flash_get_sec_size(db->user_data))
    {
        unsigned int del_sum = 0;       //记录删除的数量
        for (unsigned int n = sizeof(mdb_sec_status_t); n < (flash_get_sec_size(db->user_data) - sizeof(mdb_node_t)); n += sizeof(mdb_node_t))
        {
            flash_read(db->user_data, i + n, &node, sizeof(mdb_node_t));
            if (node.id == (mdb_half_grain_t)DB_EMPTY_DATA)
            {
                // 记录空间最多的扇区
                if (n < full_min)
                {
                    full_min = n;
                    empty_addr = i;
                }
                break;
            }

            if (node.size <= MDB_DATA_MAX_SIZE)
            {
                // 已删除和未写入完成的数据都认为是被删除的数据
                if ((node.deleted != (mdb_grain_t)DB_EMPTY_DATA) || 
                    (node.written == (mdb_grain_t)DB_EMPTY_DATA))
                    del_sum += (node.size + sizeof(mdb_node_t));
                n += node.size; // 跳过数据长度
            }
        }
        // 如果扇区的数据已经全部删除，擦除扇区
        if (del_sum >= (flash_get_sec_size(db->user_data) - sizeof(mdb_node_t)))
        {
            DB_LOG("erase:%x", i);
            flash_erase(db->user_data, i, flash_get_sec_size(db->user_data));
            erase_addr = i;
        }
        else if (del_sum > del_max)  //获得垃圾数据最多的扇区
        {
            del_max = del_sum;
            del_addr = i;
        }
    }
    // 不需要移动数据整理空间
    if(del_addr == (unsigned int)DB_EMPTY_DATA)
    {
        // 没有擦除过扇区，数据库已满
        if (erase_addr == (unsigned int)DB_EMPTY_DATA)
        {
            DB_LOG("db full");
            return (unsigned int)DB_EMPTY_DATA;
        }
        // 有擦除过扇区，将之前保留的扇区拿来使用
        DB_LOG("empty addr:%x", empty_addr);
        return (unsigned int)empty_addr;
    }

    empty_addr += sizeof(mdb_sec_status_t);
    for (unsigned int i = sizeof(mdb_sec_status_t); i < (flash_get_sec_size(db->user_data) - sizeof(mdb_node_t)); i += sizeof(mdb_node_t))
    {
        flash_read(db->user_data, del_addr + i, &node, sizeof(mdb_node_t));
        if (node.id == (mdb_half_grain_t)DB_EMPTY_DATA)
        {
            break;
        }

        if ((node.deleted == (mdb_grain_t)DB_EMPTY_DATA) && 
            (node.written != (mdb_grain_t)DB_EMPTY_DATA))
        {
            // 获取一个空地址
            unsigned int new_addr = get_empty_addr(db, node.size, del_addr);
            unsigned int moved_size = 0;
            unsigned int remain_size = node.size;
            if ((new_addr == (unsigned int)DB_EMPTY_DATA) || (new_addr == empty_addr))
            {
                new_addr = empty_addr;
                empty_addr = empty_addr + sizeof(mdb_node_t) + node.size;
            }
            set_note_writing_status(db, new_addr, &node);
            while (remain_size)
            {
                unsigned int data_buf[16];
                unsigned int size = remain_size < sizeof(data_buf) ? remain_size : sizeof(data_buf);
                flash_read(db->user_data, del_addr + i + moved_size + sizeof(mdb_node_t), data_buf, size);
                flash_write(db->user_data, new_addr + sizeof(mdb_node_t) + moved_size, data_buf, size);
                remain_size -= size;
                moved_size += size;
            }
            set_node_written_status(db, new_addr, del_addr + i);
            DB_LOG("mov data %x to %x, size:%d", del_addr + i, new_addr, node.size);
        }
        if (node.size <= MDB_DATA_MAX_SIZE)
        {
            i += node.size; // 跳过数据长度
        }
    }

    // 数据移动完后擦除扇区
    DB_LOG("erase %x, empty_addr:%x", del_addr, empty_addr);
    flash_erase(db->user_data, del_addr, flash_get_sec_size(db->user_data));
    return empty_addr;
}

/**
 * @brief 向数据库写入数据，覆盖原有数据
 * @param [in] db 数据库
 * @param [in] id 数据ID
 * @param [in] data 数据
 * @param [in] size 数据大小
 * @retval 0: 写入失败 1: 写入成功
 */
unsigned int mdb_write(mdb_t *db, unsigned int id, const void *data, unsigned int size)
{
    mdb_node_t db_node;
    unsigned int old_addr;
    unsigned int empty_addr;

    LOG_ASSERT(db != NULL);
    LOG_ASSERT(data != NULL);
    LOG_ASSERT(size <= MDB_DATA_MAX_SIZE);
    LOG_ASSERT(size > 0);
    LOG_ASSERT(id < (mdb_half_grain_t)DB_EMPTY_DATA);

    // 1. 查找空扇区地址
    empty_addr = get_empty_addr(db, size, DB_EMPTY_DATA);
    if (empty_addr == (unsigned int)DB_EMPTY_DATA)
    {
        unsigned int sec_mask = flash_get_sec_size(db->user_data) - 1;
        empty_addr = mdb_defrag(db);
        if ((empty_addr == (unsigned int)DB_EMPTY_DATA) || 
            (((empty_addr & sec_mask) + size + sizeof(mdb_node_t)) > flash_get_sec_size(db->user_data)))
        {
            DB_LOG("no space.");
            return 0;
        }
    }

    // 2. 查找数据是否存在
    old_addr = mdb_find_node_by_callback(db, &db_node, &id, find_by_id);

    // 3. 设置节点写入中状态
    db_node.id = id;
    db_node.size = size;
    set_note_writing_status(db, empty_addr, &db_node);

    // 4. 写入数据
    flash_write(db->user_data, empty_addr + sizeof(mdb_node_t), data, size);

    // 5. 设置节点写入完成状态
    set_node_written_status(db, empty_addr, old_addr);

    DB_LOG("write id:%x, addr:%x", id, empty_addr);
    return size;
}

/**
 * @brief 根据id读取数据库内容
 * @param [in] db 要读取的数据库
 * @param [in] id 要读取的id
 * @param [out] data 读取的数据
 * @retval 读取的大小
 */
unsigned int mdb_read(mdb_t *db, unsigned int id, void *data)
{
    mdb_node_t db_node;
    unsigned int addr = mdb_find_node_by_callback(db, &db_node, &id, find_by_id);
    DB_LOG("read id:%x, addr:%x", id, addr);
    if (addr == (unsigned int)DB_EMPTY_DATA)
        return 0;
    flash_read(db->user_data, addr + sizeof(mdb_node_t), data, db_node.size);
    return db_node.size;
}

void mdb_delete(mdb_t *db, unsigned int id)
{
    mdb_node_t db_node;
    unsigned int addr = mdb_find_node_by_callback(db, &db_node, &id, find_by_id);
    DB_LOG("del id:%x, addr:%x", id, addr);
    if (addr == (unsigned int)DB_EMPTY_DATA)
        return;

    db_node.deleting = (mdb_grain_t)DB_FILL_DATA;
    db_node.deleted = (mdb_grain_t)DB_FILL_DATA;
    flash_write(db->user_data, addr + offsetof(mdb_node_t, deleting), &db_node.deleting, sizeof(mdb_grain_t) * 2);
}

void mdb_init(mdb_t *db, void *user_data)
{
    mdb_node_t node;
    unsigned int has_empty_sec = 0;
    db->user_data = user_data;

    // 检测数据完整性，有写入过程中断电的数据则进行数据恢复
    for (unsigned int i = 0; i < flash_get_size(db->user_data); i += flash_get_sec_size(db->user_data))
    {
        for (unsigned int n = sizeof(mdb_sec_status_t); n < (flash_get_sec_size(db->user_data) - sizeof(mdb_node_t)); n += sizeof(mdb_node_t))
        {
            // 读取一个节点信息
            flash_read(db->user_data, i + n, &node, sizeof(mdb_node_t));
            // 如果已经为空数据则退出
            if (node.id == (mdb_half_grain_t)DB_EMPTY_DATA)
            {
                // 如果扇区无数据标记空扇区
                if (n == sizeof(mdb_sec_status_t))
                {
                    has_empty_sec = 1;
                }
                break;
            }

            // 如果数据正在删除却没有删除完成，说明新数据已经写完
            if ((node.deleted == (mdb_grain_t)DB_EMPTY_DATA) && 
                (node.deleting != (mdb_grain_t)DB_EMPTY_DATA))
            {
                //查找新数据位置，不管是否写入完成
                unsigned int id = node.id;
                unsigned int old_data_size = node.size;
                unsigned int new_data_addr = mdb_find_node_by_callback(db, &node, &id, find_by_id_nodetect);
                node.deleted = DB_FILL_DATA;
                if((new_data_addr != (unsigned int)DB_EMPTY_DATA) && (node.written == (mdb_grain_t)DB_EMPTY_DATA))
                {
                    //找到新数据，设置新数据写入完成
                    flash_write(db->user_data, new_data_addr + offsetof(mdb_node_t, written), &node.deleted, sizeof(node.written));
                    DB_LOG("set new node[%x] to written.\n", new_data_addr);
                }
                //如果没有新数据，是删除时断电
                //删除数据
                flash_write(db->user_data, i + n + offsetof(mdb_node_t, deleted), &node.deleted, sizeof(node.deleted));
                DB_LOG("del old node[%x].\n", i + n);
                node.size = old_data_size;
            }

            if (node.size <= MDB_DATA_MAX_SIZE)
            {
                n += node.size; // 跳过数据长度
            }
        }
    }

    // 2. 检测是否存在空扇区，如果没有空扇区可用，说明数据整理时断电了，进行数据整理
    if(!has_empty_sec)
    {
        mdb_defrag(db);
    }
}

void mdb_format(mdb_t *db)
{
    flash_erase(db->user_data, 0, flash_get_size(db->user_data));
}
