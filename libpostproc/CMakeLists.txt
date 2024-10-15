file(GLOB SRC_FILES *.c *.h)
add_library(libpostproc STATIC ${SRC_FILES})
