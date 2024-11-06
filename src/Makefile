# 源文件
SRC = captureVideo.c

# 目标文件
TARGET_LINUX = demoBin_linux
TARGET_WINDOWS = demoBin_windows.exe

# 编译器和链接器
CC = gcc
WIN_CC = x86_64-w64-mingw32-gcc

# 使用pkg-config获取编译选项
CFLAGS = -I../include

# 暂时不需要linux可执行文件
# # Linux的链接选项
# LDFLAGS_LINUX = -L../lib \
# 	-lavutil \
# 	-lavformat \
# 	-lavcodec \
# 	-lswscale \
# 	-lswresample \
# 	-lavfilter \
# 	-lavdevice \
# 	-lpostproc

# Windows的链接选项
LDFLAGS_WINDOWS = -L../lib_windows \
	-lavutil \
	-lavformat \
	-lavcodec \
	-lswscale \
	-lswresample \
	-lavfilter \
	-lavdevice \
	-lpostproc

# 默认目标
all: $(TARGET_LINUX) $(TARGET_WINDOWS)

# 编译Linux版本
$(TARGET_LINUX): $(SRC)
	$(CC) $(CFLAGS) $(SRC) $(LDFLAGS_LINUX) -o $(TARGET_LINUX)
	@echo "Linux编译成功，目标文件: $(TARGET_LINUX)"

# 编译Windows版本
$(TARGET_WINDOWS): $(SRC)
	$(WIN_CC) $(CFLAGS) $(SRC) $(LDFLAGS_WINDOWS) -o $(TARGET_WINDOWS)
	@echo "Windows编译成功，目标文件: $(TARGET_WINDOWS)"

# 清理目标
clean:
	rm -f $(TARGET_LINUX) $(TARGET_WINDOWS)

# 检查编译是否成功
.PHONY: all clean
