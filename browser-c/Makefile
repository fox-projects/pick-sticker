#!/usr/bin/make -f

CC ::= gcc
CFLAGS = $(LDFLAGS) $(LDLIBS) -g -Wpedantic -std=gnu2x -I./third_party/raylib/include
LDFLAGS ::= -L./third_party/raylib/lib
LDLIBS ::= -lraylib -lm

.PHONY: run compile
default: compile run

run:
	$(CC) $(CFLAGS) -o ./out/main ./main.c && LD_LIBRARY_PATH="$$PWD/third_party/raylib/lib" ./out/main

compile:
	mkdir -p out
	$(CC) $(CFLAGS) -o ./out/main ./main.c
