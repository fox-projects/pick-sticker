#!/usr/bin/make -f

CC ::= gcc
CFLAGS = -g -Wpedantic -std=gnu2x -I./third_party/raylib/include
LDFLAGS ::= -L./third_party/raylib/lib -lraylib -lm

.PHONY: run compile
default: compile run

run:
	$(CC) $(CFLAGS) -o ./out/main ./main.c $(LDFLAGS) && LD_LIBRARY_PATH="$$PWD/third_party/raylib/lib" ./out/main

compile:
	mkdir -p out
	$(CC) $(CFLAGS) -o ./out/main ./main.c $(LDFLAGS)
