#include "engine.h"

#include <stdio.h>
uint16_t  chip_keymap[] = {

	SDL_SCANCODE_0, SDL_SCANCODE_1,
	SDL_SCANCODE_2, SDL_SCANCODE_3,
	SDL_SCANCODE_4, SDL_SCANCODE_5,
	SDL_SCANCODE_6, SDL_SCANCODE_7,
	SDL_SCANCODE_8, SDL_SCANCODE_9,
	SDL_SCANCODE_A, SDL_SCANCODE_B,
	SDL_SCANCODE_C, SDL_SCANCODE_D,
	SDL_SCANCODE_E, SDL_SCANCODE_F,

};

Engine::Engine()
: status{false}, view_game{false}, game_halt{true}
{
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	window = SDL_CreateWindow("Chip 8 Emulator", 0, 0, GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT, 0);

	if (!window)
		return;

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

	if (!renderer)
		return;

	load_game_names();
	keyboard = SDL_GetKeyboardState(0);
	status = true;
}

Engine::~Engine()
{
	if (renderer)
		SDL_DestroyRenderer(renderer);
	if (window)
		SDL_DestroyWindow(window);
}

void 
Engine::update()
{
	while( SDL_PollEvent( &event ) ){

		if( event.type == SDL_QUIT )
			status = false;

		if (event.type == SDL_KEYDOWN) {

			switch (event.key.keysym.sym) {

				case SDLK_LEFT:
					view_game = false;
					break;

			}
		}
	}

	if (view_game && !game_halt) {

		if ( handle_game_view() == -1) {
			game_halt = true;
			write_line(10, 10, "ERROR", 0, 1, emulator_screen);
		}

	} else {
		handle_menu_view();
	}
}

int 
Engine::handle_game_view()
{
	if (wtx.wait) {

		for (int i=0; i<16; ++i) {

			if (keyboard[chip_keymap[i]]) {
				wtx.wait = false;
				wtx.ptr[0] = i;
				break;
			}
		}
	}
	return emulate_instruction();
}

void 
Engine::handle_menu_view()
{	
	int prev = 0;
	if (event.type == SDL_KEYDOWN) {

		switch (event.key.keysym.sym) {

			case SDLK_UP:
				prev = data.offset;
				data.offset = std::max(0, data.offset - 1);

				if (data.offset == prev)
					beep.beep(400, 200);

				SDL_Delay(100);
				break;
			
			case SDLK_DOWN:
				
				prev = data.offset;
				data.offset = std::min( static_cast<int>(data.content.size()) - 1, data.offset + 1);

				if (data.offset == prev)
					beep.beep(400, 200);

				SDL_Delay(100);
				break;
			
			case SDLK_RETURN:
				if (data.valid && load_game(data.path[data.offset].c_str(), memory + G_OFFSET, MAX_MEMORY - G_OFFSET) != -1) {
					reset_emulator();
					game_halt = false;
					view_game = true;
				} 

				break;
		}			
	}
	write_data(data, menu_screen);
}

void
Engine::load_game_names()
{
	DIR *d;
	struct dirent *dir;
	d = opendir("./roms");

	data.valid = false;

	if (d) {

		while ((dir = readdir(d)) != NULL) {
			std::string file_name = dir->d_name;

			if( file_name.size() >= 4 && file_name.substr(file_name.size() - 4, 4) == ".ch8"){
				data.valid = true;
				if (data.content.size())
					data.content.push_back(increment_string(data.content.back(), data.content.back().size() - 1, true));
				else
					data.content.push_back("00");
				data.path.push_back( concate_path("./roms", file_name.c_str()) );
			}
		}
		closedir(d);
	}

	if (data.valid == false) {
		data.content.push_back("EMP");
	}

	std::sort(data.path.begin(), data.path.end());
}

std::string 
Engine::increment_string(const std::string& arg, int pos, bool inc)
{	

	if (pos < 0) return "";

	if (inc == false) {
		return arg.substr(0, pos+1);
	}
	
	if (arg[pos] != '9' ) {
		return increment_string(arg, pos-1, false) + std::string{ static_cast<char>(arg[pos] + 1) };
	}

	if(pos == 0)
		return std::string{"10"};

	return increment_string(arg, pos-1, true) + std::string{"0"};

}

int
Engine::load_game(const char *filename, void *buffer, size_t nbytes)
{
	int fd = open(filename, O_RDONLY);
	int cnt = 0, res = 0;

	if (fd == -1)
		goto ERR_A;

	cnt = read(fd, buffer, nbytes);
	res = close(fd);

	if (cnt != -1 && res != -1)
		return cnt;

	ERR_A:
		return -1;
}

void 
Engine::make_noise(int duration, double Hz)
{
 	beep.beep(Hz, duration);
}
