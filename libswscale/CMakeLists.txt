file(GLOB SRC_FILES *.c *.h)
add_library(libswscale STATIC ${SRC_FILES})
