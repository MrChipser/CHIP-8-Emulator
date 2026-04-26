//
// Created by Bodor David on 24.04.2026.
//
#ifndef CHIP8_CHIP8_HPP
#define CHIP8_CHIP8_HPP
#include <cstdint>
#include <iostream>
#include <fstream>
#include <chrono>
#include <random>

const unsigned int ROM_START_ADDRESS = 0x200;
const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_START_ADDRESS = 0x50;
const unsigned int VIDEO_WIDTH = 64;
const unsigned int VIDEO_HEIGHT = 32;

class Chip8 {
public:
    Chip8();
    uint8_t keyboard[16]{};
    uint32_t video[64*32]{};
    void LoadFile(const char* filename);
    void UpdateTimers();
    //CPU Cycle
    void Cycle();
private:
    uint8_t memory[4096]{};
    uint8_t registers[16]{};
    uint16_t index = 0;
    uint16_t pc = ROM_START_ADDRESS;
    uint16_t stack[16]{};
    uint8_t sp = 0;
    uint8_t timerSound = 0;
    uint8_t timerDelay = 0;
    uint16_t opcode = 0;

    std::default_random_engine RNG;
    std::uniform_int_distribution<uint8_t> randByte;

    typedef void (Chip8::*Chip8Func)();
    Chip8Func table[0xF + 1];
    Chip8Func table0[0xE +1];
    Chip8Func table8[0xE + 1];
    Chip8Func tableE[0xE + 1];
    Chip8Func tableF[0x65 + 1];

    //CLS - clearing the display
    void OP_00E0();
    //RET - return from a subroutine
    void OP_00EE();
    //JMP - jump to address nnn
    void OP_1nnn();
    //CALL - Call subroutine at nnn
    void OP_2nnn();
    //SE Vx, byte - skip instruction if Vx == kk
    void OP_3xkk();
    //SNE Vx, byte - skip instruction if Vx != kk
    void OP_4xkk();
    //SE Vx, Vy - skip instruction if Vx == Vy
    void OP_5xy0();
    //LD Vx, byte - set Vx to kk
    void OP_6xkk();
    //ADD Vx, byte - add kk to Vx
    void OP_7xkk();
    //LD Vx, Vy - set Vx to Vy
    void OP_8xy0();
    //OR Vx, Vy - Vx = Vx OR Vy
    void OP_8xy1();
    //AND Vx, Vy - Vx = Vx AND Vy
    void OP_8xy2();
    //XOR Vx, Vy - Vx = Vx XOR Vy
    void OP_8xy3();
    //ADD Vx, Vy - Vx = Vx + Vy, VF = carry
    void OP_8xy4();
    //SUB Vx, Vy - Vx = Vx - Vy, VF = NOT borrowing
    void OP_8xy5();
    //SHR Vx - Vx shifted right, LSB stored in VF, Vy ignored
    void OP_8xy6();
    //SUBN Vx, Vy - Vx = Vy - Vx, VF = NOT borrowing
    void OP_8xy7();
    //SHL Vx - Vx shifted left, LSB stored in VF, Vy ignored
    void OP_8xyE();
    //SNE Vx, Vy - skip instruction if Vx != Vy
    void OP_9xy0();
    //LD I, addr - I = nnn
    void OP_Annn();
    //JP V0, addr - jump to location V0 + nnn
    void OP_Bnnn();
    //RND Vx, byte - Vx = randByte AND kk
    void OP_Cxkk();
    //DRW Vx, Vy, nibble - diplay an n-byte sprite starting at memory location I (Vx,Vy), VF = collision
    void OP_Dxyn();
    //SKP Vx - skip instruction if key with the value of Vx is pressed
    void OP_Ex9E();
    //SKNP Vx - skip instruction if key with the value of Vx is NOT pressed
    void OP_ExA1();
    //LD Vx, DT - Vx = delay timer value
    void OP_Fx07();
    //LD Vx, K - wait for a key press, store the key value in Vx
    void OP_Fx0A();
    //LD DT, Vx - set delay timer value to Vx
    void OP_Fx15();
    //LD ST, Vx - set sound timer value to Vx
    void OP_Fx18();
    //ADD I, Vx - I = I + Vx
    void OP_Fx1E();
    //LD F, Vx - puts the memory location of the digit in Vx into I
    void OP_Fx29();
    //LD B, Vx - store Vx in BCD format at I, I+1 and I+2
    void OP_Fx33();
    //LD [I], Vx - stores V0...Vx starting at location I
    void OP_Fx55();
    //LD Vx, [I] - reads registers V0....Vx starting at location I
    void OP_Fx65();

    //Functions for searching in Tables
    void Table0();
    void Table8();
    void TableE();
    void TableF();
    //this is for safety, does nothing
    void OP_NULL();

};
#endif //CHIP8_CHIP8_HPP