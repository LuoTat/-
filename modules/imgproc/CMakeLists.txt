set(SOURCES
    box_filter.dispatch.cxx
    color_rgb.dispatch.cxx
    color.cxx
    edge_detection.cxx
    filter.dispatch.cxx
    histogram.cxx
    imgwarp.cxx
    median_blur.dispatch.cxx
    region.cxx
    resize.cxx
    tables.cxx
    thresh.cxx
)

add_library(openHL_imgproc STATIC ${SOURCES})

target_link_libraries(openHL_imgproc PRIVATE openHL_core)