//
// Created by Bodor David on 25.04.2026.
//

#ifndef CHIP8_PLATFORM_HPP
#define CHIP8_PLATFORM_HPP
#include <cstdint>
#include <SDL.h>

class Platform {
    SDL_Window* window{};
    SDL_Renderer* renderer{};
    SDL_Texture* texture{};
public:
    Platform(const char* title, int windowW, int windowH, int textureW, int textureH);
    ~Platform();
    void Update(const void* buffer, int pitch);
    bool ProcessInput(uint8_t* keys);
};
#endif //CHIP8_PLATFORM_HPP