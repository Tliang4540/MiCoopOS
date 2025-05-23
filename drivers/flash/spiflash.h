#ifndef __SPIFLAHSH_H__
#define __SPIFLASH_H__ 

#include <flash.h>
#include <spi.h>

void spiflash_device_init(flash_device_t *dev, spi_device_t *spi, unsigned int base_addr, unsigned int size);

#endif
