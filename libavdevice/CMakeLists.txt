file(GLOB SRC_FILES *.c *.h)
add_library(libavdevice STATIC ${SRC_FILES})
