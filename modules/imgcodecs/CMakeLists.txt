set(SOURCES
    bitstrm.cxx
    grfmt_base.cxx
    grfmt_bmp.cxx
    loadsave.cxx
    utils.cxx
)

add_library(openHL_imgcodecs STATIC ${SOURCES})

# target_link_libraries(openHL_imgcodecs PRIVATE openHL_core)