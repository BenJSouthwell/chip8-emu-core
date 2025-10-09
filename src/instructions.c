#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "instructions.h"
#include "chip8_priv.h"

/*
Instruction descriptions are taken from Cowgod's
Chip-8 Technical Reference v1.0 by Thomas P. Greene available at 
https://devernay.free.fr/hacks/chip8/C8TECH10.HTM 
*/

/* Tables of function pointers to speed up instruction lookups */

static void (*op_8ZZZ_table[16])(struct chip8 *, uint16_t) = {
    op_8xy0, op_8xy1, op_8xy2, op_8xy3, 
    op_8xy4, op_8xy5, op_8xy6, op_8xy7,
    NULL, NULL, NULL, NULL,
    NULL, NULL, op_8xyE, NULL
};

static void (*opcode4_table[16])(struct chip8 *, uint16_t) = {
    op_0ZZZ, op_1nnn, op_2nnn, op_3xkk, 
    op_4xkk, op_5xy0, op_6xkk, op_7xkk,
    op_8ZZZ, op_9xy0, op_Annn, op_Bnnn,
    op_Cxkk, op_Dxyn, 0, 0
};

void 
op_0ZZZ(struct chip8 *p, uint16_t opcode) 
{
    /* 0nnn - SYS addr.    Jump to a machine code routine at nnn. */
    /* 00E0 - CLS          Clear the display. */
    /* 00EE - RET          Return from a subroutine. */
    uint16_t op8;
    /* Take the lower 8 bits of the opcode */
    op8 = opcode & 0x00FF;
    /* only 3 options, not going to bother with another table */
    if (op8 == 0x00E0)
    {
        /* Clear the display */
        memset(p->chip8_io.fbuff, 0, CHIP8_SCREEN_HEIGHT * CHIP8_SCREEN_WIDTH * sizeof(uint8_t));
        p->chip8_io.update_display = 1;
    }
    else if (op8 == 0x00EE)
    {
        /* Return from a subroutine */
        /* first, move the program counter up one on the stack */
        p->pc = p->stack[p->sp];
        /* then decrement the stack pointer */
        p->sp--;
    }
    else
    {
        /* SYS address, don't do anyting */
    }
}

void 
op_1nnn(struct chip8 *p, uint16_t opcode)
{
    /* 1nnn - JP addr */
    /* Jump to location nnn. */
    /* The interpreter sets the program counter to nnn. */
    uint16_t nnn;

    nnn = opcode & 0x0FFF;
    p->pc = nnn;
}

void 
op_2nnn(struct chip8 *p, uint16_t opcode)
{
    /* 2nnn - CALL addr */
    /* Call subroutine at nnn. */
    /* The interpreter increments the stack pointer, then puts the */
    /* current PC on the top of the stack. The PC is then set to nnn. */
    uint16_t nnn;

    nnn = opcode & 0x0FFF;
    p->sp ++;
    p->stack[p->sp] = p->pc;
    p->pc = nnn;
}

void 
op_3xkk(struct chip8 *p, uint16_t opcode)
{
    /* 3xkk - SE Vx, byte */
    /* Skip next instruction if Vx = kk. */
    /* The interpreter compares register Vx to kk, and if */
    /* they are equal, increments the program counter by 2. */
    uint8_t x, kk;

    x = (opcode & 0x0F00) >> 8;
    kk = opcode & 0x00FF;
    if(p->V[x] == kk)
    {
        p->pc += 2;
    }
}

void 
op_4xkk(struct chip8 *p, uint16_t opcode)
{
    /* 4xkk - SNE Vx, byte */
    /* Skip next instruction if Vx != kk. */
    /* The interpreter compares register Vx to kk, and if they */
    /* are not equal, increments the program counter by 2. */
    uint8_t x, kk;

    x = (opcode & 0x0F00) >> 8;
    kk = opcode & 0x00FF;
    if(p->V[x] != kk)
    {
        p->pc += 2;
    }
}

void 
op_5xy0(struct chip8 *p, uint16_t opcode)
{
    /* 5xy0 - SE Vx, Vy */
    /* Skip next instruction if Vx = Vy. */
    /* The interpreter compares register Vx to register Vy, */
    /* and if they are equal, increments the program counter by 2. */
    uint8_t x, y;
    
    x = (opcode & 0x0F00) >> 8;
    y = (opcode & 0x00F0) >> 4;
    if(p->V[x] == p->V[y])
    {
        p->pc += 2;
    }
}

void 
op_6xkk(struct chip8 *p, uint16_t opcode)
{
    /* 6xkk - LD Vx, byte */
    /* Set Vx = kk. */
    /* The interpreter puts the value kk into register Vx. */
    uint8_t x, kk;

    x = (opcode & 0x0F00) >> 8;
    kk = opcode & 0x00FF;
    p->V[x] = kk;
}

void 
op_7xkk(struct chip8 *p, uint16_t opcode)
{
    /* 7xkk - ADD Vx, byte */
    /* Set Vx = Vx + kk. */
    /* Adds the value kk to the value of register Vx, */
    /* then stores the result in Vx. */
    uint8_t x, kk;

    x = (opcode & 0x0F00) >> 8;
    kk = opcode & 0x00FF;

    p->V[x] += kk;
}

void 
op_8ZZZ(struct chip8 *p, uint16_t opcode)
{
    /* there are quite a few possible instructions */
    /* so we use another table. */
    uint8_t subcode;

    subcode = opcode & 0x000F;
    op_8ZZZ_table[subcode](p, opcode);
}

void
op_8xy0(struct chip8 *p, uint16_t opcode)
{
    /* 8xy0 - LD Vx, Vy */
    /* Set Vx = Vy. */
    /* Stores the value of register Vy in register Vx. */

    uint8_t x, y;

    x = (opcode & 0x0F00) >> 8;
    y = (opcode & 0x00F0) >> 4;
    p->V[x] = p->V[y];
}

void
op_8xy1(struct chip8 *p, uint16_t opcode)
{
    /* 8xy1 - OR Vx, Vy */
    /* Set Vx = Vx OR Vy. */
    /* Performs a bitwise OR on the values of Vx and Vy, */
    /* then stores the result in Vx. A bitwise OR compares the */
    /* corrseponding bits from two values, and if either bit is 1, */
    /* then the same bit in the result is also 1. Otherwise, it is 0. */

    uint8_t x, y;

    x = (opcode & 0x0F00) >> 8;
    y = (opcode & 0x00F0) >> 4;
    p->V[x] = p->V[x] | p->V[y];
 }

void
op_8xy2(struct chip8 *p, uint16_t opcode)
{
    /* 8xy2 - AND Vx, Vy */
    /* Set Vx = Vx AND Vy. */
    /* Performs a bitwise AND on the values of Vx and Vy, */
    /* then stores the result in Vx. A bitwise AND compares the */
    /* corrseponding bits from two values, and if both bits are 1, */
    /* then the same bit in the result is also 1. Otherwise, it is 0. */

    uint8_t x, y;

    x = (opcode & 0x0F00) >> 8;
    y = (opcode & 0x00F0) >> 4;
    p->V[x] = p->V[x] & p->V[y];
}

void
op_8xy3(struct chip8 *p, uint16_t opcode)
{
    /* 8xy3 - XOR Vx, Vy */
    /* Set Vx = Vx XOR Vy. */
    /* Performs a bitwise exclusive OR on the values of Vx and Vy, */
    /* then stores the result in Vx. An exclusive OR compares the */
    /* corrseponding bits from two values, and if the bits are not */
    /* both the same, then the corresponding bit in the result is */
    /* set to 1. Otherwise, it is 0. */

    uint8_t x, y;

    x = (opcode & 0x0F00) >> 8;
    y = (opcode & 0x00F0) >> 4;
    p->V[x] = p->V[x] ^ p->V[y];
}

void
op_8xy4(struct chip8 *p, uint16_t opcode)
{
    /* 8xy4 - ADD Vx, Vy */
    /* Set Vx = Vx + Vy, set VF = carry. */
    /* The values of Vx and Vy are added together. If the result is */
    /* greater than 8 bits (i.e., > 255,) VF is set to 1, otherwise 0. */
    /* Only the lowest 8 bits of the result are kept, and stored in Vx. */

    uint8_t x, y;

    x = (opcode & 0x0F00) >> 8;
    y = (opcode & 0x00F0) >> 4;

    /* set the overflow bit */
    p->V[0xF] = p->V[x] > (255 - p->V[y]) ? 1 : 0;

    p->V[x] = p->V[x] + p->V[y];
}

void
op_8xy5(struct chip8 *p, uint16_t opcode)
{
    /* 8xy5 - SUB Vx, Vy */
    /* Set Vx = Vx - Vy, set VF = NOT borrow. */
    /* If Vx > Vy, then VF is set to 1, otherwise 0. Then Vy is */
    /* subtracted from Vx, and the results stored in Vx. */

    uint8_t x, y;

    x = (opcode & 0x0F00) >> 8;
    y = (opcode & 0x00F0) >> 4;

    p->V[0xF] = p->V[x] > p->V[y] ? 1 : 0;

    p->V[x] = p->V[x] - p->V[y];
}

void
op_8xy6(struct chip8 *p, uint16_t opcode)
{
    /* 8xy6 - SHR Vx {, Vy} */
    /* Set Vx = Vx SHR 1. */
    /* If the least-significant bit of Vx is 1, then VF is set */
    /* to 1, otherwise 0. Then Vx is divided by 2. */

    uint8_t x;

    x = (opcode & 0x0F00) >> 8;

    p->V[0xF] = p->V[x] & 0x01;
    p->V[x] = p->V[x] >> 1;
}

void
op_8xy7(struct chip8 *p, uint16_t opcode)
{
    /* 8xy7 - SUBN Vx, Vy */
    /* Set Vx = Vy - Vx, set VF = NOT borrow. */
    /* If Vy > Vx, then VF is set to 1, otherwise 0. */
    /* Then Vx is subtracted from Vy, and the results stored in Vx. */

    uint8_t x, y;

    x = (opcode & 0x0F00) >> 8;
    y = (opcode & 0x00F0) >> 4;

    p->V[0xF] = p->V[y] > p->V[x] ? 1 : 0;
    p->V[x] = p->V[y] - p->V[x];
}

void
op_8xyE(struct chip8 *p, uint16_t opcode)
{
    /* 8xyE - SHL Vx {, Vy} */
    /* Set Vx = Vx SHL 1. */
    /* If the most-significant bit of Vx is 1, then VF is set to 1, */
    /* otherwise to 0. Then Vx is multiplied by 2. */

    uint8_t x;

    x = (opcode & 0x0F00) >> 8;

    p->V[0xF] = (p->V[x] & 8000) >> 15;
    p->V[x] = p->V[x] << 1;
}

void
op_9xy0(struct chip8 *p, uint16_t opcode)
{
    /* 9xy0 - SNE Vx, Vy */
    /* Skip next instruction if Vx != Vy. */
    /* The values of Vx and Vy are compared, and if they are not equal, */
    /* the program counter is increased by 2. */
    uint8_t x, y;

    x = (opcode & 0x0F00) >> 8;
    y = (opcode & 0x00F0) >> 4;

    if (p->V[x] != p->V[y])
    {
        p->pc += 2;
    }
}

void
op_Annn(struct chip8 *p, uint16_t opcode)
{
    /* Annn - LD I, addr */
    /* Set I = nnn. */
    /* The value of register I is set to nnn. */

    uint16_t nnn;
    
    nnn = (opcode & 0x0FFF);
    p->I = nnn;
}

void
op_Bnnn(struct chip8 *p, uint16_t opcode)
{
    /* Bnnn - JP V0, addr */
    /* Jump to location nnn + V0. */
    /* The program counter is set to nnn plus the value of V0. */
    uint16_t nnn;
    
    nnn = (opcode & 0x0FFF);
    p->pc = nnn + p->V[0];
}

void
op_Cxkk(struct chip8 *p, uint16_t opcode)
{
    /* Cxkk - RND Vx, byte */
    /* Set Vx = random byte AND kk. */
    /* The interpreter generates a random number from 0 to 255, */
    /* which is then ANDed with the value kk. The results are */
    /* stored in Vx. See instruction 8xy2 for more information on AND. */

    uint8_t x, kk, random;

    kk = (opcode & 0x00FF);
    x = (opcode & 0x0F00) >> 8;
    random = 42; /* TODO */
    p->V[x] = kk & random;
}

void
op_Dxyn(struct chip8 *p, uint16_t opcode)
{
    /* Dxyn - DRW Vx, Vy, nibble */
    /* Display n-byte sprite starting at memory location I at */
    /* (Vx, Vy), set VF = collision. */
    /* The interpreter reads n bytes from memory, starting at the */
    /* address stored in I. These bytes are then displayed as sprites */
    /* on screen at coordinates (Vx, Vy). Sprites are XORed onto the */
    /* existing screen. If this causes any pixels to be erased, */
    /* VF is set to 1, otherwise it is set to 0. If the sprite is positioned */
    /* so part of it is outside the coordinates of the display, it wraps */
    /* around to the opposite side of the screen. See instruction 8xy3 for */
    /* more information on XOR, and section 2.4, Display, for more information */
    /* on the Chip-8 screen and sprites. */

    uint8_t x, y, n, i, b, row, col, mask, sprite_chunk, sprite_bit, sprite_bit1;
    uint16_t sprite_address;

    x = (opcode & 0x0F00) >> 8;
    y = (opcode & 0x00F0) >> 4;
    n = (opcode & 0x000F);
    sprite_address = p->I;
    p->V[0xF] = 0;
    /* Each sprite is 8 pixels wide */
    /* for each byte (row) in the sprite */
    for(i=0; i<n; i++)
    {
        row = (p->V[y] + i) % CHIP8_SCREEN_HEIGHT;
        for(b=0; b<8; b++)
        {
            col = (p->V[x] + b) % CHIP8_SCREEN_WIDTH;
            sprite_chunk = p->mem[sprite_address + i];
            mask = 0x80 >> b;
            sprite_bit = (sprite_chunk & mask);
            sprite_bit1 = sprite_bit > 0 ? 1 : 0;
            if (p->chip8_io.fbuff[col + row * CHIP8_SCREEN_WIDTH] && sprite_bit1)
            {
                p->V[0xF] = 1;
            }
            p->chip8_io.fbuff[col + row * CHIP8_SCREEN_WIDTH] ^= sprite_bit1;
        }
    }

    p->chip8_io.update_display = 1;
}

/*
Ex9E - SKP Vx
Skip next instruction if key with the value of Vx is pressed.

Checks the keyboard, and if the key corresponding to the value of Vx is currently in the down position, PC is increased by 2.


ExA1 - SKNP Vx
Skip next instruction if key with the value of Vx is not pressed.

Checks the keyboard, and if the key corresponding to the value of Vx is currently in the up position, PC is increased by 2.


Fx07 - LD Vx, DT
Set Vx = delay timer value.

The value of DT is placed into Vx.


Fx0A - LD Vx, K
Wait for a key press, store the value of the key in Vx.

All execution stops until a key is pressed, then the value of that key is stored in Vx.


Fx15 - LD DT, Vx
Set delay timer = Vx.

DT is set equal to the value of Vx.


Fx18 - LD ST, Vx
Set sound timer = Vx.

ST is set equal to the value of Vx.


Fx1E - ADD I, Vx
Set I = I + Vx.

The values of I and Vx are added, and the results are stored in I.


Fx29 - LD F, Vx
Set I = location of sprite for digit Vx.

The value of I is set to the location for the hexadecimal sprite corresponding to the value of Vx. See section 2.4, Display, for more information on the Chip-8 hexadecimal font.


Fx33 - LD B, Vx
Store BCD representation of Vx in memory locations I, I+1, and I+2.

The interpreter takes the decimal value of Vx, and places the hundreds digit in memory at location in I, the tens digit at location I+1, and the ones digit at location I+2.


Fx55 - LD [I], Vx
Store registers V0 through Vx in memory starting at location I.

The interpreter copies the values of registers V0 through Vx into memory, starting at the address in I.


Fx65 - LD Vx, [I]
Read registers V0 through Vx from memory starting at location I.

The interpreter reads values from memory starting at location I into registers V0 through Vx.
*/

uint16_t
fetch_opcode(struct chip8 *p)
{
    uint16_t opcode;

    opcode = p->mem[p->pc] << 8 | p->mem[p->pc + 1];
    p->pc += 2;

    return opcode;
}

void 
(*decode_opcode(uint16_t opcode))(struct chip8 *, uint16_t) 
{
    /* Take the first 4 bits of the opcode */
    uint16_t op4 = (opcode & 0xF000) >> 12;
    /* Return the instruction function pointer */
    /* Note this may end up with additional redirects to other functions */
    /* that is, this may not be returning an insstruction pointer */
    return opcode4_table[op4];
}
