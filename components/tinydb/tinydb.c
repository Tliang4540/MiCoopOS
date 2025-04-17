/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#include "tinydb.h"
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

static unsigned int find_by_id(const void *user_data, const db_node_t *node)
{
    if((node->written != (db_grain_t)DB_EMPTY_DATA) && //有正确写入的数据
       (node->id == *(db_half_grain_t*)user_data))     //id相同
    {
        return 1;
    }
    return 0;
}

static unsigned int find_by_id_nodetect(const void *user_data, const db_node_t *node)
{
    if(node->id == *(db_half_grain_t*)user_data)    //fd相同
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
unsigned int tinydb_find_node_by_callback(tinydb_t *db, db_node_t *db_node, const void *user_data, find_callback_t callback)
{
    for (unsigned int i = 0; i < flash_get_size(db->user_data); i += flash_get_sec_size(db->user_data))
    {
        for (unsigned int n = sizeof(db_sec_status_t); n < (flash_get_sec_size(db->user_data) - sizeof(db_node_t)); n += sizeof(db_node_t))
        {
            flash_read(db->user_data, i + n, db_node, sizeof(db_node_t));
            if (db_node->id == (db_half_grain_t)DB_EMPTY_DATA)
                break;

            if ((db_node->deleted == (db_grain_t)DB_EMPTY_DATA) &&  //数据未被删除
                (db_node->deleting == (db_grain_t)DB_EMPTY_DATA) && //数据未准备删除
                callback(user_data, db_node))
            {
                return i + n;       //返回找到的地址
            }

            if (db_node->size <= TINYDB_DATA_MAX_SIZE)
            {
                n += db_node->size; // 跳过数据长度
            }
            else
            {
                DB_LOG("find error, size:%d\n", db_node->size);
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
static unsigned int get_empty_addr(tinydb_t *db, unsigned int size, unsigned int skip)
{
    unsigned int empty_addr0 = (unsigned int)DB_EMPTY_DATA; // 空扇区地址和可用地址
    unsigned int empty_addr1 = (unsigned int)DB_EMPTY_DATA; // 空扇区地址和可用地址
    db_half_grain_t buf[2];
    // 找到可用空间
    for (unsigned int i = 0; i < flash_get_size(db->user_data); i += flash_get_sec_size(db->user_data))
    {
        flash_read(db->user_data, i, buf, sizeof(db_half_grain_t));
        if ((buf[0] != (db_half_grain_t)DB_EMPTY_DATA) || (i == skip)) // 如果扇区已满则跳过
        {
            continue;
        }
        // 扇区空间是否足够
        for (unsigned int n = sizeof(db_sec_status_t); n < (flash_get_sec_size(db->user_data) - sizeof(db_node_t)); n += sizeof(db_node_t))
        {
            flash_read(db->user_data, i + n, buf, sizeof(db_grain_t));
            if (buf[1] == (db_half_grain_t)DB_EMPTY_DATA) // 跳过空扇区，先使用已使用扇区
            {
                if (n == sizeof(db_sec_status_t))
                {
                    if (empty_addr0 == (unsigned int)DB_EMPTY_DATA)
                        empty_addr0 = i + sizeof(db_sec_status_t);
                    else if (empty_addr1 == (unsigned int)DB_EMPTY_DATA) // 要确保至少留有一块空扇区
                        empty_addr1 = i + sizeof(db_sec_status_t);
                }
                else if ((flash_get_sec_size(db->user_data) - n) >= (size + sizeof(db_node_t)))
                {
                    return i + n;
                }
                break;
            }

            if (buf[1] <= TINYDB_DATA_MAX_SIZE)
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
static void set_node_written_status(tinydb_t *db, unsigned int new_addr, unsigned int old_addr)
{
    db_grain_t status = DB_FILL_DATA;
    if (old_addr == (unsigned int)DB_EMPTY_DATA)
    {
        flash_write(db->user_data, new_addr + offsetof(db_node_t, written), &status, sizeof(db_grain_t));
        return;
    }
    flash_write(db->user_data, old_addr + offsetof(db_node_t, deleting), &status, sizeof(db_grain_t));
    flash_write(db->user_data, new_addr + offsetof(db_node_t, written), &status, sizeof(db_grain_t));
    flash_write(db->user_data, old_addr + offsetof(db_node_t, deleted), &status, sizeof(db_grain_t));
}

/**
 * @brief 设置节点写入中状态, 同时更新扇区已满状态
 * @param [in] db 数据库
 * @param [in] addr 节点地址
 * @param [in] status 状态
 */
static void set_note_writing_status(tinydb_t *db, unsigned int addr, db_node_t *node)
{
    unsigned int sec_size = flash_get_sec_size(db->user_data);
    unsigned int sec_mask = sec_size - 1;

    // 如果剩余空间无法容纳一个节点设置扇区已满
    if ((sec_mask & addr) + node->size + sizeof(db_node_t) + sizeof(db_node_t) + sizeof(db_grain_t) > sec_size)
    {
        unsigned int sec_addr = addr & ~sec_mask;
        db_grain_t status = DB_FILL_DATA;
        flash_write(db->user_data, sec_addr, &status, sizeof(db_grain_t));
        DB_LOG("sector [%x] full.", sec_addr);
    }
    flash_write(db->user_data, addr + offsetof(db_node_t, id), &node->id, sizeof(db_grain_t));
}

/**
 * @brief 垃圾回收，找到删除数据最多的扇区将数据移出后擦除
*/
static unsigned int tinydb_defrag(tinydb_t *db)
{
    db_node_t db_node;
    unsigned int full_min = (unsigned int)DB_EMPTY_DATA;
    unsigned int empty_addr = (unsigned int)DB_EMPTY_DATA;   //空扇区地址
    
    unsigned int del_max = 0;   //删除最多的数量
    unsigned int del_addr = (unsigned int)DB_EMPTY_DATA;  //删除最多的地址

    // 1. 查找垃圾数据最多的扇区和空扇区
    for (unsigned int i = 0; i < flash_get_size(db->user_data); i += flash_get_sec_size(db->user_data))
    {
        unsigned int del_sum = 0;       //记录删除的数量
        for (unsigned int n = sizeof(db_sec_status_t); n < (flash_get_sec_size(db->user_data) - sizeof(db_node_t)); n += sizeof(db_node_t))
        {
            flash_read(db->user_data, i + n, &db_node, sizeof(db_node_t));
            if (db_node.id == (db_half_grain_t)DB_EMPTY_DATA)
            {
                // 记录空间最多的扇区
                if (n < full_min)
                {
                    full_min = n;
                    empty_addr = i;
                }
                break;
            }

            if (db_node.size <= TINYDB_DATA_MAX_SIZE)
            {
                //已删除和未写入完成的数据都认为是被删除的数据
                if ((db_node.deleted != (db_grain_t)DB_EMPTY_DATA) || 
                    (db_node.written == (db_grain_t)DB_EMPTY_DATA))
                    del_sum += db_node.size;
                n += db_node.size; // 跳过数据长度
            }
        }
        if (del_sum > del_max)  //获得垃圾数据最多的扇区
        {
            del_max = del_sum;
            del_addr = i;
        }
    }

    if(del_addr == (unsigned int)DB_EMPTY_DATA)  //都是有效的数据，没有可整理的空间
    {
        DB_LOG("no need defrag");
        return (unsigned int)DB_EMPTY_DATA;
    }

    empty_addr += sizeof(db_sec_status_t);
    for (unsigned int i = sizeof(db_sec_status_t); i < (flash_get_sec_size(db->user_data) - sizeof(db_node_t)); i += sizeof(db_node_t))
    {
        flash_read(db->user_data, del_addr + i, &db_node, sizeof(db_node_t));
        if (db_node.id == (db_half_grain_t)DB_EMPTY_DATA)
        {
            break;
        }

        if ((db_node.deleted == (db_grain_t)DB_EMPTY_DATA) && 
            (db_node.written != (db_grain_t)DB_EMPTY_DATA))
        {
            // 获取一个空地址
            unsigned int new_addr = get_empty_addr(db, db_node.size, del_addr);
            unsigned int moved_size = 0;
            unsigned int remain_size = db_node.size;
            if ((new_addr == (unsigned int)DB_EMPTY_DATA) || (new_addr == empty_addr))
            {
                new_addr = empty_addr;
                empty_addr = empty_addr + sizeof(db_node_t) + db_node.size;
            }
            set_note_writing_status(db, new_addr, &db_node);
            while (remain_size)
            {
                unsigned int data_buf[16];
                unsigned int size = remain_size < sizeof(data_buf) ? remain_size : sizeof(data_buf);
                flash_read(db->user_data, del_addr + i + moved_size + sizeof(db_node_t), data_buf, size);
                flash_write(db->user_data, new_addr + sizeof(db_node_t) + moved_size, data_buf, size);
                remain_size -= size;
                moved_size += size;
            }
            set_node_written_status(db, new_addr, del_addr + i);
            DB_LOG("mov data %x to %x, size:%d", del_addr + i, new_addr, db_node.size);
        }
        if (db_node.size <= TINYDB_DATA_MAX_SIZE)
        {
            i += db_node.size; // 跳过数据长度
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
unsigned int tinydb_write(tinydb_t *db, unsigned int id, const void *data, unsigned int size)
{
    db_node_t db_node;
    unsigned int old_addr;
    unsigned int empty_addr;

    LOG_ASSERT(db != NULL);
    LOG_ASSERT(data != NULL);
    LOG_ASSERT(size <= TINYDB_DATA_MAX_SIZE);
    LOG_ASSERT(size > 0);
    LOG_ASSERT(id < (db_half_grain_t)DB_EMPTY_DATA);

    // 1. 查找空扇区地址
    empty_addr = get_empty_addr(db, size, DB_EMPTY_DATA);
    if (empty_addr == (unsigned int)DB_EMPTY_DATA)
    {
        unsigned int sec_mask = flash_get_sec_size(db->user_data) - 1;
        empty_addr = tinydb_defrag(db);
        if ((empty_addr == (unsigned int)DB_EMPTY_DATA) || 
            (((empty_addr & sec_mask) + size + sizeof(db_node_t)) > flash_get_sec_size(db->user_data)))
        {
            DB_LOG("no space.");
            return 0;
        }
    }

    // 2. 查找数据是否存在
    old_addr = tinydb_find_node_by_callback(db, &db_node, &id, find_by_id);

    // 3. 设置节点写入中状态
    db_node.id = id;
    db_node.size = size;
    set_note_writing_status(db, empty_addr, &db_node);

    // 4. 写入数据
    flash_write(db->user_data, empty_addr + sizeof(db_node_t), data, size);

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
unsigned int tinydb_read(tinydb_t *db, unsigned int id, void *data)
{
    db_node_t db_node;
    unsigned int addr = tinydb_find_node_by_callback(db, &db_node, &id, find_by_id);
    DB_LOG("read id:%x, addr:%x", id, addr);
    if (addr == (unsigned int)DB_EMPTY_DATA)
        return 0;
    flash_read(db->user_data, addr + sizeof(db_node_t), data, db_node.size);
    return db_node.size;
}

void tinydb_delete(tinydb_t *db, unsigned int id)
{
    db_node_t db_node;
    unsigned int addr = tinydb_find_node_by_callback(db, &db_node, &id, find_by_id);
    DB_LOG("del id:%x, addr:%x", id, addr);
    if (addr == (unsigned int)DB_EMPTY_DATA)
        return;

    db_node.deleting = (db_grain_t)DB_FILL_DATA;
    db_node.deleted = (db_grain_t)DB_FILL_DATA;
    flash_write(db->user_data, addr + offsetof(db_node_t, deleting), &db_node.deleting, sizeof(db_grain_t) * 2);
}

void tinydb_init(tinydb_t *db, void *user_data)
{
    db_node_t db_node;
    unsigned int has_empty_sec = 0;
    db->user_data = user_data;

    // 检测数据完整性，有写入过程中断电的数据则进行数据恢复
    for (unsigned int i = 0; i < flash_get_size(db->user_data); i += flash_get_sec_size(db->user_data))
    {
        for (unsigned int n = sizeof(db_sec_status_t); n < (flash_get_sec_size(db->user_data) - sizeof(db_node_t)); n += sizeof(db_node_t))
        {
            // 读取一个节点信息
            flash_read(db->user_data, i + n, &db_node, sizeof(db_node_t));
            // 如果已经为空数据则退出
            if (db_node.id == (db_half_grain_t)DB_EMPTY_DATA)
            {
                // 如果扇区无数据标记空扇区
                if (n == sizeof(db_sec_status_t))
                {
                    has_empty_sec = 1;
                }
                break;
            }

            // 如果数据正在删除却没有删除完成，说明新数据已经写完
            if ((db_node.deleted == (db_grain_t)DB_EMPTY_DATA) && 
                (db_node.deleting != (db_grain_t)DB_EMPTY_DATA))
            {
                //查找新数据位置，不管是否写入完成
                unsigned int id = db_node.id;
                unsigned int old_data_size = db_node.size;
                unsigned int new_data_addr = tinydb_find_node_by_callback(db, &db_node, &id, find_by_id_nodetect);
                db_node.deleted = DB_FILL_DATA;
                if((new_data_addr != (unsigned int)DB_EMPTY_DATA) && (db_node.written == (db_grain_t)DB_EMPTY_DATA))
                {
                    //找到新数据，设置新数据写入完成
                    flash_write(db->user_data, new_data_addr + offsetof(db_node_t, written), &db_node.deleted, sizeof(db_node.written));
                    DB_LOG("set new node[%x] to written.\n", new_data_addr);
                }
                //如果没有新数据，是删除时断电
                //删除数据
                flash_write(db->user_data, i + n + offsetof(db_node_t, deleted), &db_node.deleted, sizeof(db_node.deleted));
                DB_LOG("del old node[%x].\n", i + n);
                db_node.size = old_data_size;
            }

            if (db_node.size <= TINYDB_DATA_MAX_SIZE)
            {
                n += db_node.size; // 跳过数据长度
            }
        }
    }

    // 2. 检测是否存在空扇区，如果没有空扇区可用，说明数据整理时断电了，进行数据整理
    if(!has_empty_sec)
    {
        tinydb_defrag(db);
    }
}

void tinydb_format(tinydb_t *db)
{
    flash_erase(db->user_data, 0, flash_get_size(db->user_data));
}
