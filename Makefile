CC = gcc
CFLAGS = -Wall

TARGET = test.exe

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

.PHONY: all done clean rebuild

# make
all: rebuild

# 一度削除してから作り直す
rebuild: clean $(TARGET)

# exe作成
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

# c -> o
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# ヘッダファイル変更時も再コンパイル
$(OBJS): $(wildcard *.h)

# make done
done: $(TARGET)
	.\$(TARGET)

# make clean
clean:
	-del /Q *.o 2>nul
	-del /Q $(TARGET) 2>nul