set(SOURCES
    alloc.cxx
    convert_scale.dispatch.cxx
    convert.dispatch.cxx
    copy.cxx
    matrix_iterator.cxx
    matrix.cxx
    parallel.cxx
    split.dispatch.cxx
    system.cxx
    umatrix.cxx
)

add_library(openHL_core STATIC ${SOURCES})
