#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "chip8.h"
#include "chip8_priv.h"
#include "fonts.h"
#include "instructions.h"
 
struct chip8 *
initialise_chip8(enum chip8_clock clock)
{
    struct chip8 * p;
    p = calloc(1, sizeof(struct chip8));
    if (p == NULL)
    {
        return NULL;
    }
    /* initialise the program counter to the start address */
    p->pc = PROGRAM_START_ADDRESS;
    p->chip8_io.buzzer_active = 0;
    p->chip8_io.update_display = 0;
    /* the value of the clock enum is the timer clock divider */
    p->timer_clock_div = clock;
    /* copy font into memory */
    memcpy(&p->mem[FONT_START_ADDRESS], fontset, FONTSET_SIZE*sizeof(uint8_t));
    /* initialise the random number generator */
    p->prng = initialise_lfsr_prng(0, 0);
    return p;
}

struct chip8_io *
get_io_chip8(struct chip8 *p)
{
    if (p == NULL)
    {
        return NULL;
    }
    return &p->chip8_io;
}

int 
load_rom_chip8(struct chip8 * p, uint8_t * data, uint16_t num_bytes)
{	
    if(p == NULL || data == NULL || num_bytes == 0)
    {
        return 1;
    }
    if (num_bytes > MAX_ROM_SIZE)
    {
        fprintf(stderr, "ROM is %d bytes, maximum size is %d bytes\n", num_bytes, MAX_ROM_SIZE);
        return 1;
    }
    
    /* zero the old ROM data, if any */
    memset(&p->mem[PROGRAM_START_ADDRESS], 0, MAX_ROM_SIZE);

    /* now copy in the ROM data */
    memcpy(&p->mem[PROGRAM_START_ADDRESS], data, num_bytes);
    
    return 0;
}

static
void
update_timers(struct chip8 *p)
{
    if(p == NULL)
    {
        return;
    }
    p->tick += 1;
    if(p->tick % p->timer_clock_div != 0)
    {
        return;
    }
    p->tick = 0;
    if(p->sound_timer > 0)
    {
        p->sound_timer --;
        p->chip8_io.buzzer_active = 1;
    }
    else
    {
        p->chip8_io.buzzer_active = 0;
    }
    if(p->delay_timer > 0)
    {
        p->delay_timer --;
    }
}

void
execute_cycle_chip8(struct chip8 *p)
{
    uint8_t n;
    uint16_t opcode;
    void (*fn)(struct chip8 *, uint16_t);

    if(p==NULL)
    {
        return;
    }

    p->chip8_io.update_display = 0;

    if(p->waiting_for_key == 1)
    {
        /* check to see if there is a key press */
        for (n=0; n<16; n++)
        {
            if(p->chip8_io.keypad_state[n] == 1)
            {
                p->V[p->key_x] = n;
                p->waiting_for_key = 0;
                break;
            }
        }
        /* no key press so we do not continue*/
        if(p->waiting_for_key == 1)
        {
            update_timers(p);
            return;
        }
    }

    /* update internal random number generator */
    p->rnd = lfsr_prng_process(p->prng);

    /* Grab the next opcode in the ROM. This is a 16bit chunk of data */
    opcode = fetch_opcode(p);

    /* Decode the opcode. This takes the opcode and gets the instruction function pointer */
    fn = decode_opcode(opcode);

    /* Now, execute the instruction as we have the opcode (which still contains the variable part)
       decoded instruction */
    fn(p, opcode);
    
    update_timers(p);

    return;
}

void 
free_chip8(struct chip8 * p)
{
    free_lfsr_prng(p->prng);
    free(p);
    return;
}
