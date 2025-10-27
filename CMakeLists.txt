# CMakeLists.txt
cmake_minimum_required(VERSION 3.14)
project(chip8emu)

# Set build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()


# The actual chip8 emulator library (using C90)
add_library(chip8emu_lib STATIC)
file(GLOB SRC_FILES src/*.h src/*.c)
file(GLOB HEADER_FILES include/*.h)
target_sources(chip8emu_lib PRIVATE ${SRC_FILES} ${HEADER_FILES})
target_include_directories(chip8emu_lib PUBLIC include)
set_property(TARGET chip8emu_lib PROPERTY C_STANDARD 90)

# Enable warnings
if(MSVC)
    target_compile_options(chip8emu_lib PRIVATE /W4)
else()
    target_compile_options(chip8emu_lib PRIVATE -Wall -Wextra -Wstrict-prototypes -pedantic -Werror)
endif()


if(BUILD_FRONTEND)
    include(FetchContent)
    # Fetch SDL and make it available
    FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-2.28.5
    )
    FetchContent_MakeAvailable(SDL2)
    add_executable(chip8emu_sdl frontends/main.c)
    target_link_libraries(chip8emu_sdl PRIVATE chip8emu_lib SDL2::SDL2 SDL2::SDL2main)
    # SDL requires C99, compile the frontend with that
    set_property(TARGET chip8emu_sdl PROPERTY C_STANDARD 99)
endif()

