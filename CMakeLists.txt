# CMakeLists.txt
cmake_minimum_required(VERSION 3.14)
project(chip8emu)

# Set build type to Debug if not specified
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()
# Set C standard to C90
set(CMAKE_C_STANDARD 90)

# The actual chip8 emulator library
add_library(chip8emu_lib STATIC)
file(GLOB SRC_FILES src/*.h src/*.c)
file(GLOB HEADER_FILES include/*.h)
target_sources(chip8emu_lib PRIVATE ${SRC_FILES} ${HEADER_FILES})
target_include_directories(chip8emu_lib PUBLIC include)

# Enable warnings
if(MSVC)
    target_compile_options(chip8emu_lib PRIVATE /W4)
else()
    target_compile_options(chip8emu_lib PRIVATE -Wall -Wextra -Wstrict-prototypes -pedantic -Werror)
endif()

# example frontend that just renders to console
add_executable(chip8emu frontends/main.c)
target_link_libraries(chip8emu PRIVATE chip8emu_lib)