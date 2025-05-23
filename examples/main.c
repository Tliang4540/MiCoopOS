/*
 * SPDX-License-Identifier: MIT
 *
 * Copyright (c) 2025 ljyan
 */
#include <pin.h>
#include <clk.h>
#include <serial.h>
#include <systick.h>
#include <log.h>
#include <mcos.h>

#include "led_dev_test/led_dev_test.h"
#include "flash_test/flash_test.h"

static uint32_t hello_task_stack[64];
static void hello_task(void *arg)
{
    MC_UNUSED(arg);
    while (1)
    {
        LOG_I("%s", arg);
        LOG_DUMP(arg, 16);
        LOG_I("free stack: %d", mc_task_get_free_stack());
        mc_delay(1000);
    }
}

int main(void)
{
    static device_t log_dev;
    clk_init();
    clk_lsc_init(CLK_LSC_EXTERNAL);

    pin_mode(2, PIN_MODE_FUNCTION_PP);  // UART TX
    pin_mode(3, PIN_MODE_FUNCTION_PP);  // UART RX
    pin_pull(3, PIN_PULL_UP);           // Pull up RX pin
    pin_function(2, 1);                 // UART TX function
    pin_function(3, 1);                 // UART RX function
    serial_device_init(&log_dev, 0, 500000);
    log_init(&log_dev);                 // Initialize log

    // 在os未启动前不能输出超过seiral缓存的数据，否则触发任务切换导致异常
    LOG_I("MiCoopOS test startup.");
    
    led_dev_test();
    mc_task_init(hello_task, "Hello MiCoopOS.", hello_task_stack, sizeof(hello_task_stack));
    // flash_test();

    systick_init(1000, mc_tick_handler);
    mc_start();
}
