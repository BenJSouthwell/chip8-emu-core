#ifndef ROMS_H
#define ROMS_H

/*
A simple header-only library that reads ROMs from disk
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MAX_ROM_SIZE (3584) /* 4096 - 512 (0x200) */

struct rom {
    uint8_t * data;
    uint16_t num_bytes;
};

static inline struct rom *
read_rom(const char * path)
{	
    FILE    *infile;
    long    numbytes;
    struct rom * r;

    infile = fopen(path, "rb");
 
    if(infile == NULL)
    {
        fprintf(stderr, "could not open: %s\n", path);
        return NULL;
    }

    fseek(infile, 0L, SEEK_END);
    numbytes = ftell(infile);

    if (numbytes > MAX_ROM_SIZE)
    {
        fprintf(stderr, "%s is %ld bytes, maximum size is %d\n", path, numbytes, MAX_ROM_SIZE);
        fclose(infile);
        return NULL;
    }

    fseek(infile, 0L, SEEK_SET);

    r = malloc(sizeof(struct rom));
    if (r == NULL) {
        fclose(infile);
        return NULL;
    }
    r->data = malloc(numbytes);
    if (r->data == NULL) {
        free(r);
        fclose(infile);
        return NULL;
    }
    r->num_bytes = numbytes;

    fread(r->data, sizeof(uint8_t), numbytes, infile);
    fclose(infile);

    return r;
}

static inline void 
free_rom(struct rom * r)
{
    if (r == NULL)
        return;

    if (r->data != NULL)
        free(r->data);
    
    free(r);
}

#undef MAX_ROM_SIZE

#endif // ROMS_H
