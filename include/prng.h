#ifndef CHIP8_PRNG_H
#define CHIP8_PRNG_H

#include <stdint.h>

/*
A simple 32 bit linear feedback shift register pseudo random number generator
that outputs the least significant 8 bits only.
*/

struct 
lfsr_prng *
initialise_lfsr_prng(uint32_t seed, uint32_t polynomial);

uint8_t
lfsr_prng_process(struct lfsr_prng *p);

void 
free_lfsr_prng(struct lfsr_prng *p);

#endif /* CHIP8_PRNG_H */
