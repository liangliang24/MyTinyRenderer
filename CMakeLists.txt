# 指定最低 CMake 版本
cmake_minimum_required(VERSION 3.10)

# 项目信息
project(MyTinyRenderer)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# 指定源文件
file(GLOB SOURCE_FILES
    src/*.cpp
    vendor/temp/*.cpp
)

message(STATUS "SOURCE_FILES: ${SOURCE_FILES}")

# 生成可执行文件
add_executable(MyTinyRenderer ${SOURCE_FILES})

# 可选：添加头文件路径
include_directories(vendor/temp vendor/glm)

add_subdirectory(vendor/glm)
