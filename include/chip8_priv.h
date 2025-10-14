#ifndef CHIP8_PRIV_H
#define CHIP8_PRIV_H

/*
The API required to integrate the CHIP8 emulator into an app
*/

#include <stdint.h>

#include "chip8.h"
#include "prng.h"

#define CHIP8_MEM_SIZE_BYTES (4096)
#define PROGRAM_START_ADDRESS (0x200)
#define FONT_START_ADDRESS (0x0000)

struct chip8
{
    /* chip 8 */
    uint8_t     mem[CHIP8_MEM_SIZE_BYTES];  /* RAM */
    uint16_t    pc;                         /* program counter */
    uint8_t     V[16];                      /* General purpose registers */
    uint16_t    I;                          /* the address register (note we only use the lower 12 bits) */
    uint8_t     delay_timer;
    uint8_t     sound_timer;
    uint16_t    stack[16];                  /* the stack */
    uint8_t     sp;                         /* stack pointer (note we only use the lower 4 bits) */
    /* emulator state */ 
    uint8_t    tick;                        /* keep track of ticks for clock division */
    uint8_t    timer_clock_div;             /* clock divider to run the timers at*/
    struct lfsr_prng * prng;                /* random number generator */
    uint8_t            rnd;                 /* random number updates each cycle*/
    char waiting_for_key;                   /* execution of the program is halted */
    uint8_t            key_x;               /**/
    /* externally accessible IO (frambuffer, buzzer, keypad etc) */
    struct chip8_io chip8_io;
};

#endif /* CHIP8_PRIV_H */
