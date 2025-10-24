#include <stdint.h>
#include <stdlib.h>

#include "prng.h"

/*
A simple 32 bit linear feedback shift register pseudo random number generator
that outputs the least significant 8 bits only.
*/

struct 
lfsr_prng
{
    uint32_t buff;
    uint32_t polynomial;
};

struct
lfsr_prng *
initialise_lfsr_prng(uint32_t seed, uint32_t polynomial)
{
    struct lfsr_prng * p;
    p = calloc(1, sizeof(struct lfsr_prng));
    if (p == NULL)
    {
        return NULL;
    }
    if (seed == 0)
    {
        p->buff = 0x8FF00F00;
    }
    else
    {
        p->buff = seed;
    }
    if (polynomial == 0)
    {
        p->polynomial = 0x80200003;
    }
    else
    {
        p->polynomial = polynomial;
    }
    return p;
}

uint8_t
lfsr_prng_process(struct lfsr_prng * p)
{
    uint32_t lsb;
    if (p == NULL)
    {
        return 0;
    }
    lsb = p->buff & 0x0001;
    p->buff = p->buff >> 1;
    if (lsb)
    {
        p->buff ^= p->polynomial;
    }
    return (uint8_t) (p->buff & 0x000000FF);
}

void 
free_lfsr_prng(struct lfsr_prng *p)
{
    if (p != NULL)
    {
        free(p);
    }
}
