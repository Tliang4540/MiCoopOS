/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#include <flash.h>
#include <stm32.h>
#include <log.h>
#include <string.h>
#include <mcconfig.h>

#if defined(STM32G0) && defined(BSP_USING_ONCHIP_FLASH)

#define FLASH_SECTOR_SIZE 2048

#define FLASH_KEY1 0x45670123U
#define FLASH_KEY2 0xCDEF89ABU

static int stm32g0_flash_open(flash_device_t *dev)
{
    MC_UNUSED(dev);
    return 0;
}

static int stm32g0_flash_close(flash_device_t *dev)
{
    MC_UNUSED(dev);
    return 0;
}

static int stm32g0_flash_read(flash_device_t *dev, unsigned int offset, void *buf, unsigned int size)
{
    LOG_ASSERT(offset + size <= dev->size);

    memcpy(buf, (const void*)(offset + dev->base_addr), size);

    return size;
}

static int stm32g0_flash_write(flash_device_t *dev, unsigned int offset, const void *buf, unsigned int size)
{
    LOG_ASSERT(offset + size <= dev->size);
    LOG_ASSERT(offset % 8 == 0);
    LOG_ASSERT(size % 8 == 0);

    unsigned int *p = (unsigned int *)buf;
    unsigned int ret = size;

    offset = offset + dev->base_addr;

    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
    FLASH->CR |= FLASH_CR_PG;

    while (size)
    {
        *(unsigned int *)offset = *p++;
        __ISB();
        *(unsigned int *)(offset + 4) = *p++;
        offset += 8;
        size -= 8;

        #if defined(FLASH_DBANK_SUPPORT)
        while (FLASH->SR & (FLASH_SR_BSY1 | FLASH_SR_BSY2));
        #else
        while (FLASH->SR & FLASH_SR_BSY1);
        #endif
        if (FLASH->SR & 0x83FA)
        {
            LOG_E("onchip_flash_write:%x", FLASH->SR);
            ret = -1;
            break;
        }
    }
    FLASH->SR = 0x83FA;
    FLASH->CR &= ~FLASH_CR_PG;
    FLASH->CR |= FLASH_CR_LOCK;

    return ret;
}

static int stm32g0_flash_erase(flash_device_t *dev, unsigned int offset, unsigned int size)
{
    LOG_ASSERT(offset + size <= dev->size);
    LOG_ASSERT(offset % FLASH_SECTOR_SIZE == 0);
    LOG_ASSERT(size % FLASH_SECTOR_SIZE == 0);

    unsigned int tmp;
    unsigned int ret = size;
    offset = (dev->base_addr + offset) / FLASH_SECTOR_SIZE;
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;

    while(size)
    {
        tmp = FLASH->CR & ~FLASH_CR_PNB;
        FLASH->CR = tmp | (offset << 3) | FLASH_CR_PER | FLASH_CR_STRT;
        #if defined(FLASH_DBANK_SUPPORT)
        while (FLASH->SR & (FLASH_SR_BSY1 | FLASH_SR_BSY2));
        #else
        while (FLASH->SR & FLASH_SR_BSY1);
        #endif
        if (FLASH->SR & 0x83FA)
        {
            LOG_E("onchip_flash_erase:%x", FLASH->SR);
            ret = -1;
            break;
        }
        offset++;
        size -= FLASH_SECTOR_SIZE;
    }
    FLASH->SR = 0x83FA;
    FLASH->CR &= ~FLASH_CR_PER;
    FLASH->CR |= FLASH_CR_LOCK;

    return ret;
}

static const struct flash_device_ops onchip_flash_ops = {
    .open = stm32g0_flash_open,
    .close = stm32g0_flash_close,
    .write = stm32g0_flash_write,
    .read = stm32g0_flash_read,
    .erase = stm32g0_flash_erase,
};

void onchip_flash_device_init(flash_device_t *dev, unsigned int base_addr, unsigned int size)
{
    LOG_ASSERT(base_addr % FLASH_SECTOR_SIZE == 0);
    LOG_ASSERT(size % FLASH_SECTOR_SIZE == 0);
    dev->base_addr = base_addr;
    dev->size = size;
    dev->sec_size = FLASH_SECTOR_SIZE;
    dev->write_grain = 8;
    dev->ops = &onchip_flash_ops;
}

#endif
