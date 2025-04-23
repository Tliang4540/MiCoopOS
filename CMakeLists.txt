cmake_minimum_required(VERSION 3.20)

if (NOT DEFINED CHIP)
    set(MICOOPOS_PATH .)
    include(cmake/config_template.cmake)
    project(micoopos VERSION 1.0.0 LANGUAGES C ASM)
endif()

CONFIGURE_FILE(include/mcconfig.h.in ${CMAKE_CURRENT_BINARY_DIR}/mcconfig.h)

include(cmake/arch.cmake)
include(cmake/drivers.cmake)
include(cmake/components.cmake)

add_library(micoopos
    src/mcos.c
    src/device.c
    src/ringbuffer.c
    src/log.c
    src/mclib.c
)

target_link_libraries(micoopos PRIVATE archlib driverslib)

if(TARGET componentslib)
    target_link_libraries(micoopos PUBLIC componentslib)
endif()

target_include_directories(micoopos PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(micoopos PUBLIC include drivers/include)
target_compile_definitions(micoopos PUBLIC LOG_LEVEL=${LOG_LEVEL} LOG_COLOR_ENABLE)
