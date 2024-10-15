file(GLOB SRC_FILES *.c *.h)
add_library(libavformat STATIC ${SRC_FILES})
