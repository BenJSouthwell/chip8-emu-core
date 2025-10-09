#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "chip8.h"
#include "chip8_priv.h"
#include "instructions.h"
 
struct chip8 *
initialise_chip8()
{
    struct chip8 * p;
    p = calloc(1, sizeof(struct chip8));
    /* initialise the program counter to the start address */
    p->pc = PROGRAM_START_ADDRESS;
    p->chip8_io.buzzer_active = 0;
    p->chip8_io.update_display = 0;
    return p;
}

struct chip8_io *
get_io_chip8(struct chip8 *p)
{
    return &p->chip8_io;
}

int 
load_rom_chip8(struct chip8 * p, uint8_t * data, uint16_t num_bytes)
{	
    if (num_bytes > MAX_ROM_SIZE)
    {
        fprintf(stderr, "ROM is %d bytes, maximum size is %d bytes\n", num_bytes, MAX_ROM_SIZE);
        return 1;
    }
    
    /* nuke the chip8 memory ? */
    memset(p->mem, 0, CHIP8_MEM_SIZE_BYTES);

    /* now copy in the ROM data */
    memcpy(&p->mem[PROGRAM_START_ADDRESS], data, num_bytes);
    
    return 0;
}



void
execute_cycle_chip8(struct chip8 *p)
{
    uint16_t opcode;

    void (*fn)(struct chip8 *, uint16_t);

    p->chip8_io.update_display = 0;

    /* Grab the next opcode in the ROM. This is a 16bit chunk of data */
    opcode = fetch_opcode(p);

    /* Decode the opcode. This takes the opcode and gets the instruction function pointer */
    fn = decode_opcode(opcode);

    /* Now, execute the instruction as we have the opcode (which still contains the variable part)
       decoded instruction */
    fn(p, opcode);
}

void 
free_chip8(struct chip8 * p)
{
    free(p);
    return;
}
