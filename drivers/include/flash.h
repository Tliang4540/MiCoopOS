/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#ifndef __FLASH_H__
#define __FLASH_H__

typedef struct flash_dev
{
    unsigned int base_addr;
    unsigned int size;
    unsigned int sec_size;
    unsigned int write_grain;
    void *user_data;
    int (*read)(struct flash_dev *dev, unsigned int offset, void *buf, unsigned int size);
    int (*write)(struct flash_dev *dev, unsigned int offset, const void *buf, unsigned int size);
    int (*erase)(struct flash_dev *dev, unsigned int offset, unsigned int size);
    void (*enter_sleep)(struct flash_dev *dev);
    void (*exit_sleep)(struct flash_dev *dev);
}flash_dev_t;

static inline int flash_read(struct flash_dev *dev, unsigned int offset, void *buf, unsigned int size)
{
    return dev->read(dev, offset, buf, size);
}

static inline int flash_write(struct flash_dev *dev, unsigned int offset, const void *buf, unsigned int size)
{
    return dev->write(dev, offset, buf, size);
}

static inline int flash_erase(struct flash_dev *dev, unsigned int offset, unsigned int size)
{
    return dev->erase(dev, offset, size);
}

static inline void flash_enter_sleep(struct flash_dev *dev)
{
    dev->enter_sleep(dev);
}

static inline void flash_exit_sleep(struct flash_dev *dev)
{
    dev->exit_sleep(dev);
}

static inline unsigned int flash_get_sec_size(struct flash_dev *dev)
{
    return dev->sec_size;
}

static inline unsigned int flash_get_size(struct flash_dev *dev)
{
    return dev->size;
}

static inline unsigned int flash_get_write_grain(struct flash_dev *dev)
{
    return dev->write_grain;
}

int spiflash_init(flash_dev_t *dev, void *user_data, unsigned int base_addr, unsigned int size);
int onchip_flash_init(flash_dev_t *dev, void *user_data, unsigned int base_addr, unsigned int size);

#endif
