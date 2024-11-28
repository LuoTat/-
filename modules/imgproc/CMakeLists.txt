set(SOURCES
    color_rgb.dispatch.c
    color.c
    histogram.c
    blur.c
)

add_library(openHL_imgproc STATIC ${SOURCES})