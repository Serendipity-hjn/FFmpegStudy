file(GLOB SRC_FILES *.c *.h)
add_library(libavutil STATIC ${SRC_FILES})
