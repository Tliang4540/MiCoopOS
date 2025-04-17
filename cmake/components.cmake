add_library(componentslib
    components/graphic/graphic.c
    components/tinydb/tinydb.c
)

target_include_directories(componentslib PUBLIC
    components/graphic
    components/tinydb
)

target_link_libraries(componentslib PRIVATE driverslib micoopos)
