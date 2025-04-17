add_library(archlib)

string(TOLOWER ${CHIP} CHIP_LPWER)
if(${CHIP} MATCHES "STM32G0")
    target_compile_definitions(archlib PUBLIC USE_FULL_LL_DRIVER USER_VECT_TAB_ADDRESS VECT_TAB_OFFSET=${VECT_TAB_OFFSET} ${CHIP} STM32G0)
    aux_source_directory(arch/arm/stm32/stm32g0xx           SYSTEM_SRC)
    aux_source_directory(arch/arm/stm32/stm32g0xx/lib/Src   LIB_LL_SRC)

    set(ARCH_SRC ${SYSTEM_SRC} ${LIB_LL_SRC} arch/arm/stm32/stm32g0xx/startup/startup_${CHIP_LPWER}.s arch/arm/cortex-m0/context.s arch/arm/cortex-m0/cpuport.c)
    set(ARCH_INC arch/arm/include arch/arm/stm32/include arch/arm/stm32/stm32g0xx/include arch/arm/stm32/stm32g0xx/lib/Inc)
elseif(${CHIP} MATCHES "STM32L0")
    target_compile_definitions(archlib PUBLIC USE_FULL_LL_DRIVER USER_VECT_TAB_ADDRESS VECT_TAB_OFFSET=${VECT_TAB_OFFSET} ${CHIP} STM32L0)
    aux_source_directory(arch/arm/stm32/stm32l0xx           SYSTEM_SRC)
    aux_source_directory(arch/arm/stm32/stm32l0xx/lib/Src   LIB_LL_SRC)

    set(ARCH_SRC ${SYSTEM_SRC} ${LIB_LL_SRC} arch/arm/stm32/stm32l0xx/startup/startup_${CHIP_LPWER}.s arch/arm/cortex-m0/context.s arch/arm/cortex-m0/cpuport.c)
    set(ARCH_INC arch/arm/include arch/arm/stm32/include arch/arm/stm32/stm32l0xx/include arch/arm/stm32/stm32l0xx/lib/Inc)
else()
    message(FATAL_ERROR "ARCH ${ARCH} Undefined.")
endif()

target_sources(archlib PRIVATE ${ARCH_SRC})

target_include_directories(archlib PUBLIC ${ARCH_INC})
