#ifndef CHIP8_H
#define CHIP8_H

#include <stdint.h>

/*
This is the public API. Use this to integrate Chip8 into an app.
*/

#define CHIP8_SCREEN_WIDTH (64)
#define CHIP8_SCREEN_HEIGHT (32)
#define MAX_ROM_SIZE (3584) /* 4096 - 512 (0x200) */

enum chip8_clock
{
    CHIP8_CLOCK_RATE_300Hz = 5,
    CHIP8_CLOCK_RATE_360Hz = 6,
    CHIP8_CLOCK_RATE_420Hz = 7,
    CHIP8_CLOCK_RATE_480Hz = 8,
    CHIP8_CLOCK_RATE_540Hz = 9,
    CHIP8_CLOCK_RATE_600Hz = 10,
    CHIP8_CLOCK_RATE_660Hz = 11,
    CHIP8_CLOCK_RATE_720Hz = 12
};

struct chip8_io
{
    /* inputs */
    uint8_t     keypad_state[16];
    /* outputs */
    uint8_t     fbuff[CHIP8_SCREEN_WIDTH * CHIP8_SCREEN_HEIGHT];
    char        update_display;
    char        buzzer_active;            
};

/*
Initialise a chip8 emulator.
*/
struct chip8 *
initialise_chip8(enum chip8_clock clock);

/* 
Get a pointer to the output state
*/
struct chip8_io *
get_io_chip8(struct chip8 *p);

/*
Load ROM data into the chip8 RAM.
*/
int 
load_rom_chip8(struct chip8 * p, uint8_t * data, uint16_t num_bytes);

/* 
Run a single fetch, decode, execute cyle
*/
void
execute_cycle_chip8(struct chip8 *p);

void 
free_chip8(struct chip8 *p);

#endif /* CHIP8_H */
