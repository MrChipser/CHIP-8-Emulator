#include <iostream>
#include "chip8.hpp"
#include "platform.hpp"

int main(int argc, char** argv) {

    if (argc!=4) {
        std::cout << "Wrong usage! Usage of " << argv[0] << ": <Scale> <Speed> <File>" << std::endl;
        std::exit(EXIT_FAILURE);
    }

    int scale = std::stoi(argv[1]);
    int speed = std::stoi(argv[2]);
    int cyclesPerFrame = (speed > 0) ? speed : 2;
    const char* filename = argv[3];

    Platform platform("CHIP-8 Emulator", VIDEO_WIDTH*scale, VIDEO_HEIGHT*scale, VIDEO_WIDTH, VIDEO_HEIGHT);

    Chip8 chip8;
    chip8.LoadFile(filename);

    int videoPitch = sizeof(chip8.video[0]) * VIDEO_WIDTH;

    auto lastFrameTime = SDL_GetTicks();
    bool quit = false;

    while (!quit) {
        quit = platform.ProcessInput(chip8.keyboard);

        //batching CPU cycles
        for (int i = 0; i < cyclesPerFrame; i++) {
            chip8.Cycle();
        }
        //updating at 60Hz
        chip8.UpdateTimers();
        platform.Update(chip8.video, videoPitch);

        uint32_t currentFrameTime = SDL_GetTicks();
        uint32_t frameDuration = currentFrameTime - lastFrameTime;
        //if execution was too fast, we wait to get roughly 60 FPS
        if (frameDuration < 16) {
            SDL_Delay(16 - frameDuration);
        }

        lastFrameTime = SDL_GetTicks();
    }
    return 0;
}