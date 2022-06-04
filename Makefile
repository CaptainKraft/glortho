default:
	clang -g -o build/glortho.out -std=c11 -I/usr/include/SDL2 -Isrc -Wall -D_REENTRANT src/*.c -L/usr/lib -lm -pthread -lSDL2 -lGL

run:
	cd build; ./glortho.out
