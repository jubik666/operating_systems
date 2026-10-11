CC = gcc
CFLAGS = -Wall -Wextra -std=c11 -O2 -pthread
LDFLAGS = -pthread

TARGET = lab2

OBJS = main.o matrix.o seq_gauss.o par_gauss.o

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS) -lm

main.o: main.c matrix.h seq_gauss.h par_gauss.h
	$(CC) $(CFLAGS) -c main.c -o main.o

matrix.o: matrix.c matrix.h
	$(CC) $(CFLAGS) -c matrix.c -o matrix.o

seq_gauss.o: seq_gauss.c seq_gauss.h matrix.h
	$(CC) $(CFLAGS) -c seq_gauss.c -o seq_gauss.o

par_gauss.o: par_gauss.c par_gauss.h matrix.h
	$(CC) $(CFLAGS) -c par_gauss.c -o par_gauss.o

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
