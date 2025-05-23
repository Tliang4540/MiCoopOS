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
    src/mcutils.c
)

target_link_libraries(micoopos PRIVATE archlib)
target_link_libraries(micoopos INTERFACE driverslib)

if(TARGET componentslib)
    target_link_libraries(micoopos PUBLIC componentslib)
endif()

target_include_directories(micoopos PRIVATE ${CMAKE_CURRENT_BINARY_DIR})
target_include_directories(micoopos PUBLIC include drivers/include)
target_compile_definitions(micoopos PUBLIC LOG_LEVEL=${LOG_LEVEL})
if(LOG_ASSERT_ENABLE)
    target_compile_definitions(micoopos PUBLIC LOG_ASSERT_ENABLE)
endif()
if(LOG_COLOR_ENABLE)
    target_compile_definitions(micoopos PUBLIC LOG_COLOR_ENABLE)
endif()
