cmake_minimum_required(VERSION 3.10)

# 项目名称
project(demoBin)

# 设置C编译器
set(CMAKE_C_COMPILER gcc)

# 包含目录
include_directories(../../include)

# 源文件
set(SRC main.cpp)

# 目标文件
add_executable(${PROJECT_NAME} ${SRC})

# 链接库
target_link_libraries(${PROJECT_NAME}
    -L../../lib
    -lavutil
    -lavformat
    -lavcodec
    -lswscale
    -lswresample
    -lavfilter
    -lavdevice
    -lpostproc
)
