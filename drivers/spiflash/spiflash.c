/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#include <mcos.h>
#include <log.h>
#include <spi.h>
#include <flash.h>
#include <mcconfig.h>

#if defined (BSP_USING_SPIFLASH)

#define SPIFLASH_PAGE_SIZE          (256)
#define SPIFLASH_SECTOR_SIZE        (4096)

#define CMD_PAGE_WRITE          0x02
#define CMD_READ                0x03
#define CMD_WRITE_DISABLE       0x04
#define CMD_READ_STATUS         0x05
#define CMD_WRITE_ENABLE        0x06
#define CMD_ERASE_4K            0x20
#define CMD_ERASE_32K           0x52
#define CMD_ERASE_64K           0xD8
#define CMD_ERASE_ALL           0x60

#define CMD_READ_ID             0x9F
#define CMD_DEEP_SLEEP          0xB9
#define CMD_WAKEUP              0xAB

static int spiflash_wait_busy(flash_dev_t *dev)
{
    unsigned char buf[4];
    unsigned int overtime = 100;

    while(overtime--)
    {
        buf[0] = CMD_READ_STATUS;
        spi_transfer(dev->user_data, buf, 2);
        if((buf[1] & 0x01) == 0)
            return 0;
        mc_delay(1);
    };
    
    return -1;
}

static int spiflash_read(flash_dev_t *dev, unsigned int offset, void *buf, unsigned int size)
{
    unsigned char cmd[4];

    LOG_ASSERT(offset + size <= dev->size);

    if(spiflash_wait_busy(dev))
        return -1;

    offset += dev->base_addr;
    cmd[0] = CMD_READ;
    cmd[1] = (unsigned char)(offset >> 16);
    cmd[2] = (unsigned char)(offset >> 8);
    cmd[3] = (unsigned char)(offset);
    
    spi_send_then_recv(dev->user_data, cmd, 4, buf, size);

    return size;
}

static int spiflash_write(flash_dev_t *dev, unsigned int offset, const void *buf, unsigned int size)
{
    unsigned int write_size;
    unsigned char cmd[4];

    LOG_ASSERT(offset + size <= dev->size);

    offset += dev->base_addr;
    while(size)
    {
        if(spiflash_wait_busy(dev))
            return -1;

        cmd[0] = CMD_WRITE_ENABLE;
        spi_send(dev->user_data, cmd, 1);

        cmd[0] = CMD_PAGE_WRITE;
        cmd[1] = (unsigned char)(offset >> 16);
        cmd[2] = (unsigned char)(offset >> 8);
        cmd[3] = (unsigned char)(offset);
        
        //获取当前页剩余空间
        write_size = SPIFLASH_PAGE_SIZE - (offset & (SPIFLASH_PAGE_SIZE - 1));  

        //如果当前页空间足够，写入输入的长度，否则写满整页
        if(write_size > size)
            write_size = size;

        spi_send_then_send(dev->user_data, cmd, 4, buf, write_size);

        buf = (void*)((unsigned int)buf + write_size);
        size -= write_size;
        offset += write_size;
    }
    return size;
}

static int spiflash_erase(flash_dev_t *dev, unsigned int offset, unsigned int size)
{
    unsigned char cmd[4];

    LOG_ASSERT(offset + size <= dev->size);
    LOG_ASSERT(offset % SPIFLASH_SECTOR_SIZE == 0);
    LOG_ASSERT(size % SPIFLASH_SECTOR_SIZE == 0);

    offset += dev->base_addr;
    while(size)
    {
        if(spiflash_wait_busy(dev))
            return -1;

        cmd[0] = CMD_WRITE_ENABLE;
        spi_send(dev->user_data, cmd, 1);

        cmd[0] = CMD_ERASE_4K;
        cmd[1] = (unsigned char)(offset >> 16);
        cmd[2] = (unsigned char)(offset >> 8);
        cmd[3] = (unsigned char)(offset);
        spi_send(dev->user_data, cmd, 4);
        offset += SPIFLASH_SECTOR_SIZE;
        size -= SPIFLASH_SECTOR_SIZE;
    }
    return size;
}

static void spiflash_enter_sleep(flash_dev_t *dev)
{
    unsigned char cmd[4];
    cmd[0] = CMD_DEEP_SLEEP;
    spi_send(dev->user_data, cmd, 1);
}

static void spiflash_exit_sleep(flash_dev_t *dev)
{
    unsigned char cmd[4];
    cmd[0] = CMD_WAKEUP;
    spi_send(dev->user_data, cmd, 1);
}

int spiflash_init(flash_dev_t *dev, void *user_data, unsigned int base_addr, unsigned int size)
{
    unsigned char tmp[4];

    LOG_ASSERT(base_addr % SPIFLASH_SECTOR_SIZE == 0);
    LOG_ASSERT(size % SPIFLASH_SECTOR_SIZE == 0);

    dev->base_addr = base_addr;
    dev->size = size;
    dev->sec_size = SPIFLASH_SECTOR_SIZE;
    dev->write_grain = 1;
    dev->user_data = user_data;
    dev->read = spiflash_read;
    dev->write = spiflash_write;
    dev->erase = spiflash_erase;
    dev->enter_sleep = spiflash_enter_sleep;
    dev->exit_sleep = spiflash_exit_sleep;

    tmp[0] = CMD_READ_ID;
    spi_transfer(dev->user_data, tmp, 4);

    LOG_I("spiflash id:%x %x %x", tmp[1], tmp[2], tmp[3]);
    if (tmp[1] == 0xff)
        return -1;

    return 0;
}

#endif
