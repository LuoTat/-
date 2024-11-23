set(SOURCES
    convert_scale.dispatch.c
    convert.dispatch.c
    mat.c
)

add_library(openHL_core STATIC ${SOURCES})

target_link_libraries(openHL_core PRIVATE m)
