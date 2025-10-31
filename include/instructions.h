#ifndef CHIP8_INSTR_H
#define CHIP8_INSTR_H

#include <stdint.h>

struct chip8;

void 
op_0ZZZ(struct chip8 *p, uint16_t opcode);

void 
op_1nnn(struct chip8 *p, uint16_t opcode);

void 
op_2nnn(struct chip8 *p, uint16_t opcode);

void 
op_3xkk(struct chip8 *p, uint16_t opcode);

void 
op_4xkk(struct chip8 *p, uint16_t opcode);

void 
op_5xy0(struct chip8 *p, uint16_t opcode);

void 
op_6xkk(struct chip8 *p, uint16_t opcode);

void 
op_7xkk(struct chip8 *p, uint16_t opcode);

void 
op_8ZZZ(struct chip8 *p, uint16_t opcode);

void 
op_8xy0(struct chip8 *p, uint16_t opcode);

void 
op_8xy1(struct chip8 *p, uint16_t opcode);

void 
op_8xy2(struct chip8 *p, uint16_t opcode);

void 
op_8xy3(struct chip8 *p, uint16_t opcode);

void 
op_8xy4(struct chip8 *p, uint16_t opcode);

void 
op_8xy5(struct chip8 *p, uint16_t opcode);

void 
op_8xy6(struct chip8 *p, uint16_t opcode);

void 
op_8xy7(struct chip8 *p, uint16_t opcode);

void 
op_8xyE(struct chip8 *p, uint16_t opcode);

void
op_9xy0(struct chip8 *p, uint16_t opcode);

void
op_Annn(struct chip8 *p, uint16_t opcode);

void
op_Bnnn(struct chip8 *p, uint16_t opcode);

void
op_Cxkk(struct chip8 *p, uint16_t opcode);

void
op_Dxyn(struct chip8 *p, uint16_t opcode);

void
op_EZZZ(struct chip8 *p, uint16_t opcode);

void
op_FZZZ(struct chip8 *p, uint16_t opcode);

void
op_Fx07(struct chip8 *p, uint16_t opcode);

void
op_Fx0A(struct chip8 *p, uint16_t opcode);

void
op_Fx15(struct chip8 *p, uint16_t opcode);

void
op_Fx18(struct chip8 *p, uint16_t opcode);

void
op_Fx1E(struct chip8 *p, uint16_t opcode);

void
op_Fx29(struct chip8 *p, uint16_t opcode);

void
op_Fx33(struct chip8 *p, uint16_t opcode);

void
op_Fx55(struct chip8 *p, uint16_t opcode);

void
op_Fx65(struct chip8 *p, uint16_t opcode);

uint16_t
fetch_opcode(struct chip8 *p);

void 
(*decode_opcode(uint16_t opcode))(struct chip8 *, uint16_t);

#endif /* CHIP8_INSTR_H */
