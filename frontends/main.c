#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "chip8.h"
#include "roms.h"

/* a janky (TODO unjank it) renderer */
static void render_to_terminal(struct chip8_io * chip8_io);

int 
main(int argc, char* argv[])
{
    struct chip8 * p;
    struct chip8_io * chip8_io;
    struct rom * r;
    int i;
    
    if (argc != 2)
    {
        fprintf(stderr, "usage:\n\t%s <ROM_FILE>\n", argv[0]);
        exit(1);
    }
    
    printf("loading rom file: %s\n", argv[1]);


    p = initialise_chip8(CHIP8_CLOCK_RATE_600Hz);
    chip8_io = get_io_chip8(p);
    r = read_rom(argv[1]);
    load_rom_chip8(p, r->data, r->num_bytes);

    for(i=0; i<2000; i ++)
    {
        // Non-blocking keyboard input
        fd_set set;
        struct timeval timeout = {0, 0};
        FD_ZERO(&set);
        FD_SET(STDIN_FILENO, &set);
        int rv = select(STDIN_FILENO + 1, &set, NULL, NULL, &timeout);
        if (rv > 0) {
            char ch;
            read(STDIN_FILENO, &ch, 1);
            // Process input character 'ch' as needed
        }

        // Fetch, decode and execute an instruction
        execute_cycle_chip8(p);

        // draw to screen 
        if(chip8_io->update_display)
        {
            render_to_terminal(chip8_io);
        }
        
        // update inputs

        // throttle the loop and wait a bit
        usleep(1667); // ~600 FPS
    }
 
    return 0;
}


void 
render_to_terminal(struct chip8_io * chip8_out)
{
    int r,c;
    /* Move cursor to top-left and clear screen */
    printf("\033[H\033[J");
    for( c =0; c < CHIP8_SCREEN_WIDTH; c++)
        printf("-");
    printf("\n");
    for( r=0; r < CHIP8_SCREEN_HEIGHT; r++)
    {
        for( c =0; c < CHIP8_SCREEN_WIDTH; c++)
        {
            if (chip8_out->fbuff[c + r * CHIP8_SCREEN_WIDTH])
                printf("*");
            else
                printf(" ");
        }
        printf("\n");
    }
    fflush(stdout); // Ensure output is shown immediately
}
 