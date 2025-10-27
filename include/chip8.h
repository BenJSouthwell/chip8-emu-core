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
    CHIP8_CLOCK_RATE_720Hz = 12,
    CHIP8_CLOCK_RATE_780Hz = 13,
    CHIP8_CLOCK_RATE_840Hz = 14,
    CHIP8_CLOCK_RATE_900Hz = 15
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
Arguments: 
    - enum chip8_clock: clock the rate the application will call  execute_cycle_chip8() at 
Returns a pointer to the chip8 emulator state.
*/
struct chip8 *
initialise_chip8(enum chip8_clock clock);

/* 
Get a pointer to the output state. This is the only publically available struct
and is all you need to interface and control the emulator with.
Arguments:
    - struct chip8 *p: a pointer to the chip8 state
Returns a pointer to the chip8_io struct
*/
struct chip8_io *
get_io_chip8(struct chip8 *p);

/*
Load ROM data into the chip8 RAM.
Arguments:
    - struct chip8 *p: a pointer to the chip8 state
    - uint8_t * data: a pointer to the ROM data in host RAM
    - uint16_t num_bytes: the size of the ROM data in bytes
Returns 0 on success 1 on failruie
*/
int 
load_rom_chip8(struct chip8 * p, uint8_t * data, uint16_t num_bytes);

/* 
Run a single fetch, decode, execute cyle.
You must call this at the clock rate the chip8 is configured for.
Arguments:
    - struct chip8 *p: a pointer to the chip8 state
*/
void
execute_cycle_chip8(struct chip8 *p);

/*
Use this to change the clock rate of the chip8 after initialisation
Arguments:
    - struct chip8 *p: a pointer to the chip8 state
    - enum chip8_clock: clock the rate the application will call  execute_cycle_chip8() at 
Returns 0 on success 1 on failruie
*/
int 
change_clock_rate_chip8(struct chip8 *p, enum chip8_clock clock);

void 
free_chip8(struct chip8 *p);

#endif /* CHIP8_H */
