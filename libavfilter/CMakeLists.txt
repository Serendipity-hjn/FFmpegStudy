file(GLOB SRC_FILES *.c *.h)
add_library(libavfilter STATIC ${SRC_FILES})
