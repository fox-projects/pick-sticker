#!/usr/bin/make -f

CC ::= clang
CFLAGS = $(LDFLAGS) $(LDLIBS) -g -Wpedantic -Werror -Wno-unused-command-line-argument -std=gnu2x
LDFLAGS ::= -L./third_party/raylib/lib
LDLIBS ::= -lraylib -lm

run: main.o
	$(CC) $(CFLAGS) -o ./out/main ./main.c && LD_LIBRARY_PATH="$$PWD/third_party/raylib/lib" ./out/main
