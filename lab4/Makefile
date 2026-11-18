CC = clang
CFLAGS = -Wall -Wextra -std=c11 -Iinclude
LIBFLAGS = -dynamiclib -fPIC

HEADERS = $(wildcard include/*.h)

all: lib1.dylib lib2.dylib prog1 prog2

# Динамические библиотеки
lib1.dylib: src/lib1.c $(HEADERS)
	$(CC) $(CFLAGS) $(LIBFLAGS) -install_name @rpath/$@ $< -o $@

lib2.dylib: src/lib2.c $(HEADERS)
	$(CC) $(CFLAGS) $(LIBFLAGS) -install_name @rpath/$@ $< -o $@

# main1
prog1: src/main1.c src/common.c lib1.dylib $(HEADERS)
	$(CC) $(CFLAGS) src/main1.c src/common.c -L. -l1 -Wl,-rpath,@executable_path -o $@

# main2
prog2: src/main2.c src/common.c $(HEADERS)
	$(CC) $(CFLAGS) src/main2.c src/common.c -o $@

clean:
	rm -f lib1.dylib lib2.dylib prog1 prog2

.PHONY: all clean