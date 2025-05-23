add_library(driverslib)

if(${CHIP} MATCHES "STM32")
    aux_source_directory(drivers/stm32 DRIVERS_SRC)
    if(${CHIP} MATCHES "STM32G0")
        set(DRIVERS_SRC ${DRIVERS_SRC} drivers/stm32/flash/stm32g0_flash.c)
    endif()
endif()

if (BSP_USING_SPIFLASH)
    set(DRIVERS_SRC ${DRIVERS_SRC} drivers/flash/spiflash.c)
endif()

target_sources(driverslib PRIVATE ${DRIVERS_SRC})
target_include_directories(driverslib PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(driverslib PRIVATE archlib micoopos)
target_include_directories(driverslib PUBLIC drivers/include drivers)
