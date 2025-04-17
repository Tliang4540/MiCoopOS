## 简介

MiCoopOS(Micro Cooperative OS) 是一个简单的协作式OS，占用资源很低，同时集成了基础的外设驱动。

目前支持stm32g0和stm32l0系列

## 资源占用

只有一个任务使用-Os编译占用如下（包含了clk、systick、pin、serial、ringbuff、log等功能）：
| Region | Size |
| :---: | :---: |
|  RAM | 1768 Byte |
| FLASH | 2652 Byte |

主栈占用1K（可根据中断嵌套配置更小值），serial缓存256字节，log缓存128字节，任务栈256字节

## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.