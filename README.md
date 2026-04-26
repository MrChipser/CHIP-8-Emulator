# CHIP-8 Emulator (C++)
This is a CHIP-8 Emulator written using C++ and the SDL2 library. The primary goal of the project was to gain some understanding about the architecture of emulators and also practice OOP.

## Requirements
To build this project you will need:
* A C++17 compatible compiler (e.g., Clang, GCC, or MSVC)
* [CMake](https://cmake.org/)
* [SDL2](https://www.libsdl.org/) development library

## Build and Run
1. Clone the repository
   ```bash
   git clone [https://github.com/YOUR_USERNAME/CHIP-8-Emulator-CPP.git](https://github.com/YOUR_USERNAME/CHIP-8-Emulator-CPP.git)
   cd CHIP-8-Emulator-CPP
   ```
2. Build the program
   ```bash
   mkdir build
   cd build
   cmake ..
   make
   ```
3. Run the program
   ```bash
   ./chip8 <Scale> <Speed> <FilePath>
   ```
   * Scale - CHIP-8 is originally 64x32 pixels which is too small to display, this number is a scaler to that (e.g. 10 would make a 640x320 screen)
   * Speed - it manipulates the clock speed of the CPU, bigger means faster, you can experiment with this, it changes from game to game
   * FilePath - the path of the .ch8 file

## Key Mapping
| CHIP-8 | Keyboard |
| :---: | :---: |
| 1 2 3 C | 1 2 3 4 |
| 4 5 6 D | Q W E R |
| 7 8 9 E | A S D F |
| A 0 B F | Z X C V |

* **ESC:** Closes the emulator.

## What I learned
* Low-level memory management and CPU register operations
* Reading and parsing binary files
* Integrating SDL2 
   
   
