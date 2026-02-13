#ifndef _EMULATOR_H
#define _EMULATOR_H

#include <stdint.h>

#define MAX_MEMORY	 				0x1fa0
#define G_OFFSET					0x200
#define MAX_REGISTOR				0x10
#define MAX_STACK					0x10
#define MAX_INSTRUCTION				0x24
#define KEYBOARD_MAX				0x10
#define EMULATOR_SCREEN_WIDTH    	0x40
#define EMULATOR_SCREEN_HEIGHT   	0x20
#define GAME_SCREEN_WIDTH			0x280
#define GAME_SCREEN_HEIGHT			0x140
#define PPE							0xA
#define MAX_FONT_WIDTH				0x5
#define ROW_CHAR_SPACE				0x2
#define COL_CHAR_SPACE				0x2
#define WORD_SPACE					0x2

#define MENU_SCREEN_WIDTH 	(GAME_SCREEN_WIDTH  / PPE)
#define MENU_SCREEN_HEIGHT  (GAME_SCREEN_HEIGHT / PPE)

struct wait_event {

	bool wait;
	uint8_t *ptr;

	bool screen_update = false;
	uint32_t x;
	uint32_t y;
	uint32_t n;

};

#define GET_LLN(VAL) 	( (VAL & 0xf)   >> 0 )
#define GET_LHN(VAL) 	( (VAL & 0xf0)  >> 4 )
#define GET_HLN(VAL)	( (VAL & 0xf00) >> 8 )
#define GET_HHN(VAL) 	( (VAL & 0xf00) >> 8 )

#endif