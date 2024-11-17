# 定义编译器
CC = gcc
# 编译选项
CFLAGS = -g
# 目标可执行文件
TARGET = main
# 源文件
SRCS = main.c BmpDecoder.c BmpEncoder.c loadsave.c mat.c color.c color_rgb.c split.c histogram.c norm.c minmax.c
# 生成的对象文件
OBJS = $(SRCS:.c=.o)

# 默认目标
all: $(TARGET)

# 编译目标
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS)

# 编译各个源文件为对象文件
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清理生成的文件
clean:
	rm -f $(OBJS) $(TARGET)
