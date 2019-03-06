all: build test

build:
	gcc main.c -o main -I include -L lib -l SDL2-2.0.0

test:
	./main
