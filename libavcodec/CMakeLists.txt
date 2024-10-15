file(GLOB SRC_FILES *.c *.h)
add_library(libavcodec STATIC ${SRC_FILES})
