file(GLOB SRC_FILES *.c *.h)
add_library(libswresample STATIC ${SRC_FILES})
