set(SOURCES
    grfmt_bmp.c
    loadsave.c
    utils.c
)

add_library(openHL_imgcodecs STATIC ${SOURCES})

target_link_libraries(openHL_imgcodecs PRIVATE openHL_core)