# chip8-emu-core

CHIP-8 emulator library in ANSI C. You can use this library to build your own CHIP-8 emulator application. There is an example application (for Linux and MacOS) included that uses ncurses to render to the terminal.

## Getting Started
To clone the repository, run:

```bash
git clone git@github.com:BenJSouthwell/chip8-emu-core.git
```
To build the library and example frontend, run:

```bash
cd chip8-emu-core
mkdir build
cd build
cmake ..
make
```
This will create a static library `libchip8emu_lib.a` in the `build` directory, as well as an example executable `chip8emu` in the same directory if you are using Linux or macOS (Windows builds will only get the static lib).

## Frontend Usage
To run the example frontend, navigate to the `build` directory and run:

```bash
./chip8emu ../roms/snek.ch8
```
Replace `snek.ch8` with the path to any CHIP-8 ROM you want to run.

## ROMs
There are some example CHIP-8 ROMs you can use to test the emulator located in the `/chip8-emu-core/roms/` directory. These are public domain ROMs sourced from [here](https://johnearnest.github.io/chip8Archive/?sort=platform#chip8) released under the [creative commons zero](https://creativecommons.org/public-domain/cc0/) license.

I also recommend checking out [this collection](https://github.com/Timendus/chip8-test-suite) for testing.

## API
You only need to include the `chip8.h` header file to use the library. The following functions and structures are all that you need to interface with the emulator.

```c
struct chip8 *initialise_chip8(enum chip8_clock clock);
struct chip8_io *get_io_chip8(struct chip8 *p);
int load_rom_chip8(struct chip8 *p, uint8_t *data, uint16_t num_bytes);
void execute_cycle_chip8(struct chip8 *p);
int change_clock_rate_chip8(struct chip8 *p, enum chip8_clock clock);
void free_chip8(struct chip8 *p);
```
### chip8_io Structure
This structure is used to interface with the emulator for input and output. You can get a pointer to this structure using the `get_io_chip8` function. The `keypad_state` array is used to set the state of the 16 keys on the CHIP-8 keypad. The `fbuff` array contains the current state of the framebuffer, which is 64x32 pixels. Each pixel is represented as a byte, where 0 is off and 1 is on. The `update_display` flag is set to 1 when the display needs to be updated, and the `buzzer_active` flag is set to 1 when the buzzer should be active.
```c
struct chip8_io
{
    /* inputs */
    uint8_t     keypad_state[16];
    /* outputs */
    uint8_t     fbuff[CHIP8_SCREEN_WIDTH * CHIP8_SCREEN_HEIGHT];
    char        update_display;
    char        buzzer_active;            
};
```

### chip8_clock Rates
These are the valid enums you can use to initialize the emulator with different clock rates or change the clock rate at runtime. These are essentially just the clock rate divided by 60. This is used internally as a clock divider to run the delay and sound timers at 60Hz. The caller is responsible for calling `execute_cycle_chip8` the appropriate number of times per second to achieve the promised clock rate.
```c
enum chip8_clock
{
    CHIP8_CLOCK_RATE_300Hz = 5,
    CHIP8_CLOCK_RATE_360Hz = 6,
    CHIP8_CLOCK_RATE_420Hz = 7,
    CHIP8_CLOCK_RATE_480Hz = 8,
    CHIP8_CLOCK_RATE_540Hz = 9,
    CHIP8_CLOCK_RATE_600Hz = 10,
    CHIP8_CLOCK_RATE_660Hz = 11,
    CHIP8_CLOCK_RATE_720Hz = 12,
    CHIP8_CLOCK_RATE_780Hz = 13,
    CHIP8_CLOCK_RATE_840Hz = 14,
    CHIP8_CLOCK_RATE_900Hz = 15
};
```

## Example

```c
struct chip8 *emu = initialise_chip8(CHIP8_CLOCK_RATE_600Hz);
struct chip8_io *io = get_io_chip8(emu);
load_rom_chip8(emu, rom_data, rom_size);

while (running) {
    // Set io->keypad_state based on your input
    execute_cycle_chip8(emu);
    if (io->update_display) {
        // Render io->fbuff to your display
    }
    if (io->buzzer_active) {
        // Play beep
    }
}
```

### Changing Clock Rate
You can change the clock rate of the emulator at any time using the following function:
```c
int change_clock_rate_chip8(struct chip8 *p, enum chip8_clock clock);
```
The application must call `execute_cycle_chip8` the appropriate number of times per second to achieve the desired clock rate. We need to set the clock rate so that the internal timers run at 60Hz. When we set the clock rate, we are actually setting a clock divider that divides the clock rate by 60 to get the timer update rate. Here is an example of changing the clock rate based on user input:
```c
enum chip8_clock current_rate = CHIP8_CLOCK_RATE_600Hz;
struct chip8 *emu = initialise_chip8(CHIP8_CLOCK_RATE_600Hz);
struct chip8_io *io = get_io_chip8(emu);
load_rom_chip8(emu, rom_data, rom_size);

while (running) {
    // Set io->keypad_state based on your input
    execute_cycle_chip8(emu);
    if (increase_speed) {
        if (current_rate < CHIP8_CLOCK_RATE_900Hz)
        {
            current_rate++;
            change_clock_rate_chip8(emu, current_rate);
        }
    }
    if (decrease_speed) {
        if (current_rate > CHIP8_CLOCK_RATE_300Hz)
        {
            current_rate--;
            change_clock_rate_chip8(emu, current_rate);
        }
    }
}
```
