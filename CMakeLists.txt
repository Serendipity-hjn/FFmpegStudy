cmake_minimum_required(VERSION 3.10)
project(demoBin)

set(CMAKE_C_STANDARD 11)

set(SRC decodeVideoChange.c)

find_package(PkgConfig REQUIRED)
pkg_check_modules(FFMPEG REQUIRED
    libavutil
    libavformat
    libavcodec
    libswscale
    libswresample
    libavfilter
    libavdevice
    libpostproc
)

include_directories(include)
link_directories(lib)

add_executable(${PROJECT_NAME} ${SRC})

target_link_libraries(${PROJECT_NAME}
    ${FFMPEG_LIBRARIES}
)
