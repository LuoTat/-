# 设置cmake的最低版本
cmake_minimum_required(VERSION 3.31)

# 设置工程名称和版本
project(openHL VERSION 1.0.0 LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_EXTENSIONS ON)
set(CMAKE_C_STANDARD_REQUIRED ON)

# 设置编译器的参数
SET(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -Wall -Wextra")
SET(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -Wall -Wextra")

# 设置编译器显示详细信息
set(CMAKE_VERBOSE_MAKEFILE False)

# 设置默认编译类型
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release)
    message(STATUS "未指定编译类型,设置默认编译类型:Release")
endif()

# 显示编译信息
message(STATUS "Build type:               ${CMAKE_BUILD_TYPE}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "C编译器参数:                ${CMAKE_C_FLAGS_DEBUG}")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "C编译器参数:                ${CMAKE_C_FLAGS_RELEASE}")
endif()

# 设置头文件的路径
include_directories(include)

# 生成modules
add_subdirectory(modules)

# 定义库列表
set(OPENHL_LIBRARIES openHL_core openHL_imgcodecs openHL_imgproc)

# 生成main
add_executable(main main.c)
target_link_libraries(main ${OPENHL_LIBRARIES})
