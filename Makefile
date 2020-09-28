build:
	gcc chip8.c -o chip8 `sdl2-config --libs --cflags` -ggdb3 -O0 -lm

debug:
	gcc chip8.c -o chip8 -g `sdl2-config --libs --cflags` -ggdb3 -O0 -lm

clean:
	rm chip8
