#ifndef _VIEW_H
#define _VIEW_H

#include "emulator.h"

#include <SDL2/SDL.h>
#include <vector>
#include <stdint.h>

#define COLOR_RGB0	(((uint32_t) SDL_ALPHA_OPAQUE << 24) | 0x9bbc0f)
#define COLOR_RGB1	(((uint32_t) SDL_ALPHA_OPAQUE << 24) | 0x8bac0f)
#define COLOR_RGB2	(((uint32_t) SDL_ALPHA_OPAQUE << 24) | 0x306230)
#define COLOR_RGB3	(((uint32_t) SDL_ALPHA_OPAQUE << 24) | 0xf380f)

template <uint32_t UX, uint32_t UY>
class GameView
{
public:
	SDL_Texture *texture;

	GameView(uint32_t w, uint32_t h, uint32_t px, uint32_t py, SDL_Renderer *renderer)
	: texture_w{w}, texture_h{h}, ppx{px}, ppy{py}, pixels(w*h, COLOR_RGB1) 
	{
		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, w, h);
	}

	void load_pixel(uint8_t screen[][UX])
	{	
		for (int y = 0; y < UY; ++y) {
			for (int x = 0; x < UX; ++x) {

				int real_x = x * ppx;
				int real_y = y * ppy;

				for (int a = real_x; a < real_x + ppx; ++ a) {
					for(int b = real_y; b < real_y + ppy; ++b) {
						uint32_t color = ((uint32_t) SDL_ALPHA_OPAQUE << 24) | colors[screen[y][x]];
						pixels[texture_w * b + a] = color;
					}
				}
			}
		}
	}

	void update_texture()
	{
		SDL_UpdateTexture(texture, NULL, &pixels[0], texture_w * 4);
	}

	~GameView()
	{
		SDL_DestroyTexture(texture);
	}
	
private:
	uint32_t	texture_w;
	uint32_t	texture_h;
	uint32_t	ppx;
	uint32_t    ppy;
	std::vector <uint32_t> pixels;
	uint32_t colors[4] = {COLOR_RGB1, COLOR_RGB3, COLOR_RGB0, COLOR_RGB2};

};

#endif 