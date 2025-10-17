#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>

#include <ncurses.h> 
# include "locale.h"

#include "chip8.h"
#include "roms.h"

static void draw_basic_display(struct chip8_io *io, int start_x, int start_y);
static int get_chip8_key(int ch);

int 
main(int argc, char* argv[])
{
    struct chip8 * p;
    enum chip8_clock clock_rate;
    struct chip8_io * chip8_io;
    struct rom * r;
    int i, ch, key, k;
    useconds_t sleep_time;

    /* the value of this enum is clock / 60 (used as the clock_rate divider) */
    clock_rate = CHIP8_CLOCK_RATE_600Hz; 
    sleep_time = (int)floorf(1000000.0f / (clock_rate * 60.0f)); 

    initscr(); 
    cbreak();   
    noecho();  
    curs_set(0);
    nodelay(stdscr, TRUE);

    attron(A_BOLD);
    mvprintw(0, 0, "CHIP-8 Emulator - %.0f Hz CPU. Press ESC to quit. Press +/- to change clock rate.", clock_rate * 60.0f);
    attroff(A_BOLD);
    setlocale(LC_ALL, "en-US.UTF-8");

    if (argc != 2)
    {
        fprintf(stderr, "usage:\n\t%s <ROM_FILE>\n", argv[0]);
        exit(1);
    }
    
    p = initialise_chip8(clock_rate);
    chip8_io = get_io_chip8(p);
    r = read_rom(argv[1]);
    load_rom_chip8(p, r->data, r->num_bytes);

    for(;;)
    {
        /* get and handle any keyboard input */
        ch = getch();
        if (ch != ERR) {
            // map the key to the CHIP8 Keypad
            key = get_chip8_key(ch) ;
            if (key >= 0)
            {
                chip8_io->keypad_state[key] = 1;
            }
            else if (ch == 27)
            {
                /*escape key pressed */
                break; 
            }
            else if (ch == 43 || ch == 61)
            {
                /* '=' or '+' key pressed */
                if(clock_rate < CHIP8_CLOCK_RATE_900Hz)
                {
                    clock_rate += 1;
                    sleep_time = (int)floorf(1000000.0f / (clock_rate * 60.0f));
                    attron(A_BOLD);
                    mvprintw(0, 0, "CHIP-8 Emulator - %.0f Hz CPU. Press ESC to quit. Press +/- to change clock rate.", clock_rate * 60.0f);
                    attroff(A_BOLD);
                    refresh(); 
                }
            }
            else if (ch == 45 || ch == 95)
            {
                /* '-' or '_' key pressed */
                if(clock_rate > CHIP8_CLOCK_RATE_300Hz)
                {
                    clock_rate -= 1;
                    sleep_time = (int)floorf(1000000.0f / (clock_rate * 60.0f));
                    attron(A_BOLD);
                    mvprintw(0, 0, "CHIP-8 Emulator - %.0f Hz CPU. Press ESC to quit. Press +/- to change clock rate.", clock_rate * 60.0f);
                    attroff(A_BOLD);
                    refresh(); 
                }
            }
        }

        /* Fetch, decode and execute an instruction */
        execute_cycle_chip8(p);
        
        /* if the emulator has drawn to or cleared the display, we update it*/
        if(chip8_io->update_display)
        {
            draw_basic_display(chip8_io, 1, 2);
            memset(chip8_io->keypad_state, 0, 16*sizeof(uint8_t));
            refresh(); 
        }
             
        if(chip8_io->buzzer_active)
        {
            beep();
        }
        
        usleep(sleep_time); 
    }
 
    endwin(); 
    printf("Thanks for playing ...\n");
    fflush(stdout);
    return 0;
}

static
void
draw_basic_display(struct chip8_io *io, int start_x, int start_y) {
    int x, y, pixel;    
    for (y = 0; y < CHIP8_SCREEN_HEIGHT; y++) {
        for (x = 0; x < CHIP8_SCREEN_WIDTH; x++) {
            pixel = io->fbuff[y * CHIP8_SCREEN_WIDTH + x];
            mvaddch(start_y + y, start_x + x*2, pixel ? (char)219  : ' ');
            mvaddch(start_y + y, start_x + x*2 +1, pixel ? (char)219 : ' ');
        }
    }
}

static  
int
get_chip8_key(int ch) {
    switch(ch) {
        case '1': return 0x1;
        case '2': return 0x2;
        case '3': return 0x3;
        case '4': return 0xC;
        case 'q': case 'Q': return 0x4;
        case 'w': case 'W': return 0x5;
        case 'e': case 'E': return 0x6;
        case 'r': case 'R': return 0xD;
        case 'a': case 'A': return 0x7;
        case 's': case 'S': return 0x8;
        case 'd': case 'D': return 0x9;
        case 'f': case 'F': return 0xE;
        case 'z': case 'Z': return 0xA;
        case 'x': case 'X': return 0x0;
        case 'c': case 'C': return 0xB;
        case 'v': case 'V': return 0xF;
        default: return -1;
    }
}
 