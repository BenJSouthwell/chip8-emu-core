#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>

#include <SDL.h>

#include "chip8.h"
#include "roms.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 512
#define PIXEL_SIZE 16

static void draw_display(SDL_Renderer *renderer, struct chip8_io *io);
static void update_chip8_keys(struct chip8_io *chip8_io, const Uint8 *keystate);
static void update_window_title(SDL_Window *window, enum chip8_clock clock_rate, bool buzzer_active);
static void print_help(const char *name);

int 
main(int argc, char* argv[])
{
    struct chip8 *p;
    enum chip8_clock clock_rate;
    struct chip8_io *chip8_io;
    struct rom *r;
    const Uint8 *keystate;
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Event event;
    bool running = true;
    Uint32 last_time, current_time;
    float sleep_time_ms;

    if (argc == 2 && (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0))
    {
        print_help(argv[0]);
        exit(0);
    }

    if (argc != 2)
    {
        fprintf(stderr, "usage:\n\t%s <ROM_FILE>\n", argv[0]);
        fprintf(stderr, "Try '%s --help' for more information.\n", argv[0]);
        exit(1);
    }

    /* Initialize SDL2 */
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) < 0)
    {
        fprintf(stderr, "SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        exit(1);
    }

    /* Create window */
    window = SDL_CreateWindow("CHIP-8 Emulator",
                              SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                              WINDOW_WIDTH, WINDOW_HEIGHT,
                              SDL_WINDOW_SHOWN);
    if (window == NULL)
    {
        fprintf(stderr, "Window could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_Quit();
        exit(1);
    }

    /* Create renderer */
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer == NULL)
    {
        fprintf(stderr, "Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    /* Initialize CHIP-8 */
    clock_rate = CHIP8_CLOCK_RATE_600Hz;
    sleep_time_ms = 1000.0f / (clock_rate * 60.0f);
    
    p = initialise_chip8(clock_rate);
    if (p == NULL)
    {
        fprintf(stderr, "Failed to initialize CHIP-8 emulator\n");
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    chip8_io = get_io_chip8(p);
    
    /* Load ROM */
    r = read_rom(argv[1]);
    if (r == NULL)
    {
        fprintf(stderr, "Failed to load ROM: %s\n", argv[1]);
        free_chip8(p);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    if (load_rom_chip8(p, r->data, r->num_bytes) != 0)
    {
        fprintf(stderr, "Failed to load ROM into CHIP-8\n");
        free_rom(r);
        free_chip8(p);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        exit(1);
    }

    printf("CHIP-8 Emulator \n");
    printf("Controls:\n");
    printf("  1 2 3 4     ->  1 2 3 C\n");
    printf("  Q W E R     ->  4 5 6 D\n");
    printf("  A S D F     ->  7 8 9 E\n");
    printf("  Z X C V     ->  A 0 B F\n");
    printf("  +/-: Change clock rate\n");
    printf("  ESC: Quit\n\n");

    /* Set initial window title with clock rate */
    update_window_title(window, clock_rate, false);

    last_time = SDL_GetTicks();

    /* Main loop */
    while (running)
    {
        /* Handle events */
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
                case SDL_QUIT:
                    running = false;
                    break;
                    
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym)
                    {
                        case SDLK_ESCAPE:
                            running = false;
                            break;
                            
                        case SDLK_PLUS:
                        case SDLK_EQUALS:
                            if (clock_rate < CHIP8_CLOCK_RATE_900Hz)
                            {
                                clock_rate += 1;
                                sleep_time_ms = 1000.0f / (clock_rate * 60.0f);
                                change_clock_rate_chip8(p, clock_rate);
                                update_window_title(window, clock_rate, chip8_io->buzzer_active);
                         
                            }
                            break;
                            
                        case SDLK_MINUS:
                        case SDLK_UNDERSCORE:
                            if (clock_rate > CHIP8_CLOCK_RATE_300Hz)
                            {
                                clock_rate -= 1;
                                sleep_time_ms = 1000.0f / (clock_rate * 60.0f);
                                change_clock_rate_chip8(p, clock_rate);
                                update_window_title(window, clock_rate, chip8_io->buzzer_active);
                            }
                            break;
                    }
                    break;
            }
        }

        /* get keyboard state (is high if key is down, not just on rising edge) */
        keystate = SDL_GetKeyboardState(NULL);
        update_chip8_keys(chip8_io, keystate);

        /* Execute a single CHIP-8 cycle */
        execute_cycle_chip8(p);

        /* Render display if updated */
        if (chip8_io->update_display)
        {
            draw_display(renderer, chip8_io);
            SDL_RenderPresent(renderer);
        }

        update_window_title(window, clock_rate, chip8_io->buzzer_active);

        current_time = SDL_GetTicks();
        if (current_time - last_time < sleep_time_ms)
        {
            SDL_Delay((Uint32)(sleep_time_ms - (current_time - last_time)));
        }
        last_time = SDL_GetTicks();
    }

    /* Cleanup */
    free_rom(r);
    free_chip8(p);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    printf("Thanks for playing ...\n");
    return 0;
}

static void
draw_display(SDL_Renderer *renderer, struct chip8_io *io)
{
    SDL_Rect pixel_rect;
    int x, y, pixel_value;

    /* Clear screen with black */
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    /* Draw white pixels */
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);

    pixel_rect.w = PIXEL_SIZE;
    pixel_rect.h = PIXEL_SIZE;

    for (y = 0; y < CHIP8_SCREEN_HEIGHT; y++)
    {
        for (x = 0; x < CHIP8_SCREEN_WIDTH; x++)
        {
            pixel_value = io->fbuff[y * CHIP8_SCREEN_WIDTH + x];
            if (pixel_value)
            {
                pixel_rect.x = x * PIXEL_SIZE;
                pixel_rect.y = y * PIXEL_SIZE;
                SDL_RenderFillRect(renderer, &pixel_rect);
            }
        }
    }
}

static void
update_window_title(SDL_Window *window, enum chip8_clock clock_rate, bool buzzer_active)
{
    char title[64];
    int clock_hz = (int)(clock_rate * 60.0f);
    
    if (buzzer_active)
    {
        sprintf(title, "CHIP-8 Emulator | CPU %d Hz - BEEP!", clock_hz);
    }
    else
    {
        sprintf(title, "CHIP-8 Emulator | CPU %d Hz", clock_hz);
    }
    
    SDL_SetWindowTitle(window, title);
}

static void
update_chip8_keys(struct chip8_io *chip8_io, const Uint8 *keystate)
{
    /* Map SDL keys to CHIP-8 keypad */
    chip8_io->keypad_state[0x1] = keystate[SDL_SCANCODE_1] ? 1 : 0;
    chip8_io->keypad_state[0x2] = keystate[SDL_SCANCODE_2] ? 1 : 0;
    chip8_io->keypad_state[0x3] = keystate[SDL_SCANCODE_3] ? 1 : 0;
    chip8_io->keypad_state[0xC] = keystate[SDL_SCANCODE_4] ? 1 : 0;

    chip8_io->keypad_state[0x4] = keystate[SDL_SCANCODE_Q] ? 1 : 0;
    chip8_io->keypad_state[0x5] = keystate[SDL_SCANCODE_W] ? 1 : 0;
    chip8_io->keypad_state[0x6] = keystate[SDL_SCANCODE_E] ? 1 : 0;
    chip8_io->keypad_state[0xD] = keystate[SDL_SCANCODE_R] ? 1 : 0;

    chip8_io->keypad_state[0x7] = keystate[SDL_SCANCODE_A] ? 1 : 0;
    chip8_io->keypad_state[0x8] = keystate[SDL_SCANCODE_S] ? 1 : 0;
    chip8_io->keypad_state[0x9] = keystate[SDL_SCANCODE_D] ? 1 : 0;
    chip8_io->keypad_state[0xE] = keystate[SDL_SCANCODE_F] ? 1 : 0;

    chip8_io->keypad_state[0xA] = keystate[SDL_SCANCODE_Z] ? 1 : 0;
    chip8_io->keypad_state[0x0] = keystate[SDL_SCANCODE_X] ? 1 : 0;
    chip8_io->keypad_state[0xB] = keystate[SDL_SCANCODE_C] ? 1 : 0;
    chip8_io->keypad_state[0xF] = keystate[SDL_SCANCODE_V] ? 1 : 0;
}

static void
print_help(const char *name)
{
    printf("CHIP-8 Emulator\n");
    printf("Usage: %s <ROM_FILE>\n", name);
    printf("\nControls:\n");
    printf("  1 2 3 4     ->  1 2 3 C\n");
    printf("  Q W E R     ->  4 5 6 D\n");
    printf("  A S D F     ->  7 8 9 E\n");
    printf("  Z X C V     ->  A 0 B F\n");
    printf("\n  +/-: Change clock rate\n");
    printf("  ESC: Quit\n");
}
