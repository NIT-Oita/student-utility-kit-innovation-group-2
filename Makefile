CC = gcc
CFLAGS = -Wall

TARGET = file_program.exe

SRCS = $(wildcard *.c)
OBJS = $(SRCS:.c=.o)

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

#ヘッダファイルの依存関係
$(OBJS): $(wildcard *.h)

clean:
	del $(OBJS) $(TARGET)