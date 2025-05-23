#include "flash_test.h"
#include <flash.h>
#include <mctypes.h>
#include <microdb.h>
#include <log.h>
#include <mcos.h>

static unsigned int flash_test_stack[128];
static void flash_test_task(void *arg)
{
    mdb_t mdb;
    uint64_t tmp = 0;
    uint64_t count = 0;
    mdb_init(&mdb, arg);

    if (0 == mdb_read(&mdb, 0, &tmp))
    {
        mdb_format(&mdb);
        LOG_I("flash test:format");
    }
    else
    {
        tmp++;
    }
    mdb_write(&mdb, 0, &tmp, sizeof(tmp));
    LOG_I("flash test:%d", (unsigned int)tmp);

    mdb_read(&mdb, 1, &count);

    while(1)
    {
        count++;
        LOG_I("flash test:write %d", (unsigned int)count);
        for (unsigned int i = 1; i < 200; i++)
        {
            mdb_write(&mdb, i, &count, sizeof(count));
            mc_delay(50);
        }
        for (unsigned int i = 1; i < 200; i++)
        {
            mdb_read(&mdb, i, &tmp);
            if (count != tmp)
                LOG_E("flash test:id %d error, val:%d", i, (unsigned int)tmp);
        }
        LOG_I("flash test:ok, stack:%d", mc_task_get_free_stack());
        mc_delay(3600000);
    }
}

void flash_test(void)
{
    static flash_device_t flash;
    onchip_flash_device_init(&flash, 0x08000000 + 50 * 1024, 14 * 1024);

    mc_task_init(flash_test_task, &flash, flash_test_stack, sizeof(flash_test_stack));
}
