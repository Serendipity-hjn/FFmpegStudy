TARGET=demoBin
SRC=encodeAudioDemo.c
CC=gcc
CFLAGS=-I../include
LDFLAGS=-L../lib \
        -lavutil \
        -lavformat	\
        -lavcodec	\
        -lswscale	\
        -lswresample	\
        -lavfilter	\
        -lavdevice	\
        -lpostproc

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS) -o $(TARGET)

clean:
	rm -rf $(TARGET)
