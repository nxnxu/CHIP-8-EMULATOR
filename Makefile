CC=g++
CPPFLAGS=--std=c++11 -pedantic-errors -Werror
appname = chip_emulator
SDL_CFLAGS :=  $(shell sdl2-config --cflags)
SDL_LDFLAGS := $(shell sdl2-config --libs)
OBJ = emulator.o play.o engine.o sound.o datawrite.o

$(appname): $(OBJ)
	$(CC) -o $@ $^ $(CPPFLAGS) $(SDL_CFLAGS) $(SDL_LDFLAGS)
	$(RM) *.o