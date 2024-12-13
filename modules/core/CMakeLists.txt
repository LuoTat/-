set(SOURCES
    alloc.cxx
    buffer_area.cxx
    convert_scale.dispatch.cxx
    convert.dispatch.cxx
    copy.cxx
    lapack.cxx
    matrix_decomp.cxx
    matrix_iterator.cxx
    matrix_transform.cxx
    matrix.cxx
    minmax.cxx
    norm.cxx
    parallel.cxx
    softfloat.cxx
    split.dispatch.cxx
    system.cxx
    umatrix.cxx
)

add_library(openHL_core STATIC ${SOURCES})
