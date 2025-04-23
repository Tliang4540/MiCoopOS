cmake_minimum_required(VERSION 3.20)

set(MICOOPOS_PATH ..)
set(LINK_LD_FILE ${CMAKE_CURRENT_SOURCE_DIR}/link.ld)

project(micoopos_test LANGUAGES C ASM)

include(stm32g0_config.cmake)
# include(stm32l0_config.cmake)

add_executable(${CMAKE_PROJECT_NAME}
    main.c 
    led_dev_test/led_dev_test.c
    flash_test/flash_test.c
)

add_subdirectory(${MICOOPOS_PATH} micoopos)

target_link_libraries(${CMAKE_PROJECT_NAME} PRIVATE micoopos)

# Create Flash Image
add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME}
    POST_BUILD
    # COMMAND ${CMAKE_OBJCOPY} ARGS -O ihex ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.hex
    COMMAND ${CMAKE_OBJCOPY} ARGS -O binary ${CMAKE_PROJECT_NAME}.elf ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.bin
)

add_custom_command(
    TARGET ${CMAKE_PROJECT_NAME}
    POST_BUILD
    COMMAND ${CMAKE_OBJDUMP} ARGS --source --all-headers --demangle --line-numbers --wide ${CMAKE_PROJECT_NAME}.elf > ${CMAKE_BINARY_DIR}/${CMAKE_PROJECT_NAME}.lst
	#--all-headers --demangle --disassemble -M xw 
)