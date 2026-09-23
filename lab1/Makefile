CC     = gcc
CFLAGS = -Wall -Wextra -std=c11

all: parent child

parent: parent.o common.o
	$(CC) $(CFLAGS) -o parent parent.o common.o

child: child.o common.o
	$(CC) $(CFLAGS) -o child child.o common.o

parent.o: parent.c common.h
	$(CC) $(CFLAGS) -c parent.c

child.o: child.c common.h
	$(CC) $(CFLAGS) -c child.c

common.o: common.c common.h
	$(CC) $(CFLAGS) -c common.c

clean:
	rm -f parent child *.o

.PHONY: all clean