#ifndef _ENGINE_H
#define _ENGINE_H

#include "emulator.h"
#include "sound.h"

#include <SDL2/SDL.h>

#include <algorithm>
#include <vector>
#include <string>

#include <unistd.h>
#include <fcntl.h>
#include <dirent.h> 
#include <stdio.h> 

struct menu_screen_data 
{
	int offset = 0;
	std::vector <std::string> content;
	std::vector <std::string> path;
	bool valid = false;
};

extern uint8_t	memory[];
extern uint8_t  emulator_screen[][EMULATOR_SCREEN_WIDTH];
extern uint8_t	menu_screen[][MENU_SCREEN_WIDTH];
extern int  	emulate_instruction(void);
extern void     write_data(menu_screen_data&, uint8_t s[][MENU_SCREEN_WIDTH]);
extern void		write_line(int, int, const char *, int, int, uint8_t s[][MENU_SCREEN_WIDTH]);
extern void		reset_emulator();

extern wait_event wtx;

class Engine
{

public:

	Engine();
	void update();
	void handle_menu_view();
	int  handle_game_view();
	void load_game_names();
	void make_noise(int, double);

	std::string concate_path(const std::string& a, const std::string&  b)
	{
		return a + "/" + b;
	}

	std::string increment_string(const std::string&, int, bool);
	
	int load_game(const char *filename, void *, size_t);

	~Engine();

	bool         status;
	bool         view_game;
	bool		 game_halt;
	const Uint8  *keyboard;
	SDL_Window 	 *window;
	SDL_Event    event;
	Beeper       beep;
	SDL_Renderer *renderer;
	menu_screen_data data;
	
};

#endif