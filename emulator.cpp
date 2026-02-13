#include "emulator.h"

#include <algorithm>
#include <stdint.h>

uint8_t	memory[MAX_MEMORY] = {

	0xF0, 0x90, 0x90, 0x90, 0xF0, 0x20, 0x60, 0x20, 0x20, 0x70, \
	0xF0, 0x10, 0xF0, 0x80, 0xF0, 0xF0, 0x10, 0xF0, 0x10, 0xF0, \
	0x90, 0x90, 0xF0, 0x10, 0x10, 0xF0, 0x80, 0xF0, 0x10, 0xF0, \
	0xF0, 0x80, 0xF0, 0x90, 0xF0, 0xF0, 0x10, 0x20, 0x40, 0x40, \
	0xF0, 0x90, 0xF0, 0x90, 0xF0, 0xF0, 0x90, 0xF0, 0x10, 0xF0, \
	0xF0, 0x90, 0xF0, 0x90, 0x90, 0xE0, 0x90, 0xE0, 0x90, 0xE0, \
	0xF0, 0x80, 0x80, 0x80, 0xF0, 0xE0, 0x90, 0x90, 0x90, 0xE0, \
	0xF0, 0x80, 0xF0, 0x80, 0xF0, 0xF0, 0x80, 0xF0, 0x80, 0x80, \
	0xf8, 0x80, 0xb8, 0xa8, 0xe8, 0x90, 0x90, 0xf0, 0x90, 0x90, \
	0xe0, 0x40, 0x40, 0x40, 0xe0, 0xe0, 0x20, 0x20, 0xa0, 0xe0, \
	0xa0, 0xc0, 0x80, 0xc0, 0xa0, 0x80, 0x80, 0x80, 0x80, 0xf0, \
	0xf8, 0xa8, 0xa8, 0x88, 0x88, 0x88, 0xc8, 0xa8, 0x98, 0x88, \
	0xf0, 0x90, 0x90, 0x90, 0xf0, 0xf0, 0x90, 0xf0, 0x80, 0x80, \
	0xf0, 0x90, 0x90, 0xb0, 0xf0, 0xf0, 0x90, 0xf0, 0xa0, 0x90, \
	0xe0, 0x80, 0xe0, 0x20, 0xe0, 0xe0, 0x40, 0x40, 0x40, 0x40, \
	0xa0, 0xa0, 0xa0, 0xa0, 0xe0, 0x88, 0x00, 0x50, 0x00, 0x20, \
	0x88, 0x88, 0xa8, 0xa8, 0xf8, 0x88, 0x50, 0x20, 0x50, 0x88, \
	0x90, 0x90, 0xf0, 0x10, 0xf0, 0xf8, 0x10, 0x20, 0x40, 0xf8, \
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, \

};

uint8_t emulator_screen[EMULATOR_SCREEN_HEIGHT][EMULATOR_SCREEN_WIDTH];
uint8_t	delay_registor;
uint8_t	sound_registor;

extern const uint8_t   *keyboard;

static uint8_t	reg[MAX_REGISTOR];
static uint8_t	SP;

static uint16_t	stack[MAX_STACK];
static uint16_t	I_registor;
extern uint16_t chip_keymap[];
static uint16_t	PC;
wait_event 		wtx;

using  fp = int(*)(uint16_t);
static fp ins_mapper[MAX_INSTRUCTION];


static inline uint16_t
get_next_instruction()
{
	uint16_t instruction = memory[PC++];
	return (instruction << 8) | memory[PC++];
}

int
emulate_instruction()
{
	uint16_t instruction = get_next_instruction();
	uint8_t msn = (instruction & 0xf000) >> 12;

	if ( msn >= MAX_INSTRUCTION) {
		return 1;
	}

	return !ins_mapper[msn](instruction)? instruction: -1; 
}

static int
emulate_ins0(uint16_t instruction)
{
	switch (instruction & 0xff) {
	
		case 0xE0: 
			memset(emulator_screen, 0, sizeof emulator_screen);
			break;
		
		case 0xEE:
 			PC = stack[SP--];
 			break;
 		
 		default:
 			return 1;
	}

	return 0;
}

static int
emulate_ins1(uint16_t instruction)
{
	PC = instruction & 0xfff;
	return 0;
}

static int
emulate_ins2(uint16_t instruction)
{
	stack[++SP] = PC;
	PC = instruction & 0xfff;
	return 0;
}

static int
emulate_ins3(uint16_t instruction)
{
	if (reg[GET_HLN(instruction)] == (instruction & 0xff)) {
		PC += 2;
	}

	return 0;
}

static int 
emulate_ins4(uint16_t instruction)
{
	if (reg[GET_HLN(instruction)] != (instruction & 0xff)) {
		PC += 2;
	}

	return 0;
}

static int 
emulate_ins5(uint16_t instruction)
{
	if (reg[GET_HLN(instruction)] == reg[GET_LHN(instruction)]){
		PC += 2;
	}

	return 0;
}

static int 
emulate_ins6(uint16_t instruction)
{
	reg[GET_HLN(instruction)] = instruction & 0xff;
	return 0;
}

static int 
emulate_ins7(uint16_t instruction)
{
	reg[GET_HLN(instruction)] += instruction & 0xff;
	return 0;
}

static int
emulate_ins8(uint16_t instruction)
{

	uint8_t x = GET_HLN(instruction);
	uint8_t y = GET_LHN(instruction);

	uint16_t ret;

	switch (instruction & 0xf) {

		case 0x0:
			reg[x] = reg[y];
			break;
		
		case 0x1:
			reg[x] |= reg[y];
			break;
		
		case 0x2:
			reg[x] &= reg[y];
		
		case 0x3:
			reg[x] ^= reg[y];
			break;
		
		case 0x4:
			reg[0xf] = 0;
			ret = (uint16_t) reg[x] + reg[y];
			if (ret > 255) {
				reg[0xf] = 0x1;
			}
			reg[x] = (uint8_t) ret;
			break;
		
		case 0x5:
			reg[0xf] = (reg[x] > reg[y]) ? 0x1: 0x0;
			reg[x] = std::max(reg[x], reg[y]) - std::min(reg[x], reg[y]);
			break;
		
		case 0x6:
			reg[0xf] = reg[x] & 0x1;
			reg[x] >>= 1;
			break;

		case 0x7:
			reg[0xf] = (reg[y] > reg[x])? 0x1: 0x0;
			reg[x] = std::max(reg[x], reg[y]) - std::min(reg[x], reg[y]);
			break;
		
		case 0xE:
			reg[0xf] = (reg[x] & 0x80)? 0x1: 0x0;
			reg[x] <<= 1;
		
		default:
			return 1;
	}
	return 0;
}

static int 
emulate_ins9(uint16_t instruction)
{
	if (reg[GET_HLN(instruction)] != reg[GET_LHN(instruction)]) {
		PC += 2;
	}

	return 0;
}

static int 
emulate_insA(uint16_t instruction)
{
	I_registor = instruction & 0xfff;
	return 0;
}

static int
emulate_insB(uint16_t instruction)
{
	PC = reg[0x0] + (instruction & 0xfff);
	if (PC >= MAX_MEMORY) {
		return 1;
	}

	return 0;
}

static int 
emulate_insC(uint16_t instruction)
{	
	uint8_t random_num = rand() % 256;
	reg[GET_HLN(instruction)] = random_num & (instruction & 0xff);
	return 0;
}

static int 
emulate_insD(uint16_t instruction)
{	
	int x = (reg[GET_HLN(instruction)]) & 0x3F;
	int y = (reg[GET_LHN(instruction)]) & 0x1F;
	int n = GET_LLN(instruction);

	wtx.screen_update = true;
	wtx.x = x;
	wtx.y = y;
	wtx.n = n;

	reg[0xf] = 0x0;
	for (int row=0; row < n; ++row) {

		for (int bit = 7, col = 0; bit >= 0; --bit, ++col) {

			if (x + col >= EMULATOR_SCREEN_WIDTH) 
				break;

			int gg = (memory[I_registor + row] & (1 << bit)) ?1 :0;

			if (emulator_screen[y + row][x + col] == gg && gg)
				reg[0xf] = 1;

			emulator_screen[y + row][x + col] ^= gg;
		}
	}

	return 0;
}

static int 
emulate_insE(uint16_t instruction)
{		

	uint8_t which = reg[GET_HLN(instruction)];
	bool ret = keyboard[chip_keymap[which]];

	switch (GET_LLN(instruction)) {
			
		case 0x1:
			PC += (!ret)? 2: 0;
			break;
	
		case 0xE:
			PC += ret? 2: 0;
			break;

		default:
			return 1;
	}

	return 0;
}

static int 
emulate_insF(uint16_t instruction)
{	
	uint8_t ret;
	uint8_t x = GET_HLN(instruction);

	switch (GET_LLN(instruction)) {

		case 0x7:
			reg[x] = delay_registor;
			break;

		case 0xA:
			wtx.wait = true;
			wtx.ptr  = reg + GET_HLN(instruction);
			break;

		case 0x5:
			switch (GET_LHN(instruction)) {

				case 0x1:
					delay_registor = reg[x];
					break;

				case 0x5:
					for(int i=0; i<=x; ++i)
						memory[I_registor++] = reg[i];
					break;

				case 0x6:
					for(int i=0; i<=x; ++i)
						reg[i] = memory[I_registor++];
					break;

				default:
					return 1;
				return 0;
			}

		case 0x8:
			sound_registor = reg[x];
			break;
		
		case 0xE:
			reg[0xf] = ( (uint32_t) I_registor + reg[x] > 0xfff)? 0x1: 0x0;
			I_registor += reg[x];
			break;
		
		case 0x9:
			I_registor = GET_LLN(reg[GET_HLN(instruction)]) * 5;
			break;
		
		case 0x3:
			ret = reg[x];
			for(int i=2; i>=0; --i) {
				memory[I_registor + i] = ret % 10;
				ret /= 10;
			}
			break;
		default:
			return 1;
	}
	return 0;

}

void 
reset_emulator()
{
	memset(emulator_screen, 0, sizeof emulator_screen);
	PC = G_OFFSET;
	SP = 0;
	wtx.wait = false;
}

void 
emulator_init(void)
{
	reset_emulator();
	ins_mapper[0]  = emulate_ins0;
	ins_mapper[1]  = emulate_ins1;
	ins_mapper[2]  = emulate_ins2;
	ins_mapper[3]  = emulate_ins3;
	ins_mapper[4]  = emulate_ins4;
	ins_mapper[5]  = emulate_ins5;
	ins_mapper[6]  = emulate_ins6;
	ins_mapper[7]  = emulate_ins7;
	ins_mapper[8]  = emulate_ins8;
	ins_mapper[9]  = emulate_ins9;
	ins_mapper[10] = emulate_insA;
	ins_mapper[11] = emulate_insB;
	ins_mapper[12] = emulate_insC;
	ins_mapper[13] = emulate_insD;
	ins_mapper[14] = emulate_insE;
	ins_mapper[15] = emulate_insF;
}
