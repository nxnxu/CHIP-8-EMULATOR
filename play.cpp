#include "emulator.h"
#include "engine.h"
#include "view.h"

#include <vector>
#include <string>
#include <stdint.h>
#include <time.h>
#include <stdlib.h>
#include <string.h>

extern int 		emulator_init(void);

extern uint8_t  emulator_screen[][EMULATOR_SCREEN_WIDTH];
extern uint8_t	delay_registor;
extern uint8_t	sound_registor;
uint8_t			menu_screen[MENU_SCREEN_HEIGHT][MENU_SCREEN_WIDTH];

const uint8_t  *keyboard;

int main(int argc, char **argv)
{
	srand(time(NULL));
	Engine engine;
	
	GameView <EMULATOR_SCREEN_WIDTH, EMULATOR_SCREEN_HEIGHT> gfx(GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT, 10,  10,  engine.renderer);
	GameView <MENU_SCREEN_WIDTH, MENU_SCREEN_HEIGHT>         fnt(GAME_SCREEN_WIDTH, GAME_SCREEN_HEIGHT, PPE, PPE, engine.renderer);
	keyboard = engine.keyboard;

	emulator_init();
	int FPS = 1000.0 / 60.0;

	engine.make_noise(1000, 350);
	while (engine.status) {
		int fs = SDL_GetTicks();
		
		engine.update();		

		if(engine.view_game){
			gfx.load_pixel(emulator_screen);
			gfx.update_texture();
			SDL_RenderCopyEx(engine.renderer, gfx.texture, NULL, 0, 0, 0, SDL_FLIP_NONE);
		} else{
			fnt.load_pixel(menu_screen);
			fnt.update_texture();
			SDL_RenderCopyEx(engine.renderer, fnt.texture, NULL, 0, 0, 0, SDL_FLIP_NONE);
		}

		SDL_RenderPresent(engine.renderer);

		int fe = SDL_GetTicks() - fs;
		if (fe < FPS)
            SDL_Delay(FPS - fe);

        if (delay_registor)
        	delay_registor -= 1;

        if (sound_registor) {
			engine.make_noise(500, 350);
        	sound_registor -= 1;
        }

	} SDL_Quit();

}

