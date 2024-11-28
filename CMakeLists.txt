# 设置cmake的最低版本
cmake_minimum_required(VERSION 3.31)

# 设置工程名称和版本
project(openHL VERSION 1.0.0 LANGUAGES CXX)

# 设置编译器的参数
set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED True)
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -ggdb -Wall -Wextra -Werror")
SET(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -Werror")

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
    message(STATUS "CXX编译器参数:                ${CMAKE_CXX_FLAGS_DEBUG}")
elseif(CMAKE_BUILD_TYPE STREQUAL "Release")
    message(STATUS "CXX编译器参数:                ${CMAKE_CXX_FLAGS_RELEASE}")
endif()

# 设置头文件的路径
include_directories(include)

# 生成modules
add_subdirectory(modules)

# 定义库列表
set(OPENHL_LIBRARIES openHL_core openHL_imgcodecs)

# 生成main
add_executable(main main.cxx)
target_link_libraries(main ${OPENHL_LIBRARIES})
