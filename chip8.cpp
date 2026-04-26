//
// Created by Bodor David on 24.04.2026.
//
#include "chip8.hpp"

uint8_t fontset[FONTSET_SIZE] =
{
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

Chip8::Chip8() : RNG(std::chrono::system_clock::now().time_since_epoch().count()) {
    //setting pc to the beginning of his accessible memory
    pc = ROM_START_ADDRESS;
    //loading the fontset into memory
    for (size_t i = 0; i < FONTSET_SIZE; i++)
        memory[FONTSET_START_ADDRESS + i] = fontset[i];

    //setting up a random byte from 0 to 255
    randByte = std::uniform_int_distribution<uint8_t>(0, 255U);

    //setting up the table structure for the operations
    table[0x0] = &Chip8::Table0;
    table[0x1] = &Chip8::OP_1nnn;
    table[0x2] = &Chip8::OP_2nnn;
    table[0x3] = &Chip8::OP_3xkk;
    table[0x4] = &Chip8::OP_4xkk;
    table[0x5] = &Chip8::OP_5xy0;
    table[0x6] = &Chip8::OP_6xkk;
    table[0x7] = &Chip8::OP_7xkk;
    table[0x8] = &Chip8::Table8;
    table[0x9] = &Chip8::OP_9xy0;
    table[0xA] = &Chip8::OP_Annn;
    table[0xB] = &Chip8::OP_Bnnn;
    table[0xC] = &Chip8::OP_Cxkk;
    table[0xD] = &Chip8::OP_Dxyn;
    table[0xE] = &Chip8::TableE;
    table[0xF] = &Chip8::TableF;

    //nulling every operation
    for (size_t i = 0; i <= 0xE; i++)
    {
        table0[i] = &Chip8::OP_NULL;
        table8[i] = &Chip8::OP_NULL;
        tableE[i] = &Chip8::OP_NULL;
    }
    for (size_t i = 0; i <= 0x65; i++) {
        tableF[i] = &Chip8::OP_NULL;
    }
    //setting functions for operations that exist
    table0[0x0] = &Chip8::OP_00E0;
    table0[0xE] = &Chip8::OP_00EE;

    table8[0x0] = &Chip8::OP_8xy0;
    table8[0x1] = &Chip8::OP_8xy1;
    table8[0x2] = &Chip8::OP_8xy2;
    table8[0x3] = &Chip8::OP_8xy3;
    table8[0x4] = &Chip8::OP_8xy4;
    table8[0x5] = &Chip8::OP_8xy5;
    table8[0x6] = &Chip8::OP_8xy6;
    table8[0x7] = &Chip8::OP_8xy7;
    table8[0xE] = &Chip8::OP_8xyE;

    tableE[0x1] = &Chip8::OP_ExA1;
    tableE[0xE] = &Chip8::OP_Ex9E;

    tableF[0x07] = &Chip8::OP_Fx07;
    tableF[0x0A] = &Chip8::OP_Fx0A;
    tableF[0x15] = &Chip8::OP_Fx15;
    tableF[0x18] = &Chip8::OP_Fx18;
    tableF[0x1E] = &Chip8::OP_Fx1E;
    tableF[0x29] = &Chip8::OP_Fx29;
    tableF[0x33] = &Chip8::OP_Fx33;
    tableF[0x55] = &Chip8::OP_Fx55;
    tableF[0x65] = &Chip8::OP_Fx65;
}

void Chip8::LoadFile(const char* filename) {
    //opening file in binary format and going to the end of it
    std::ifstream fin(filename, std::ios::binary | std::ios::ate);

    if (fin.is_open()) {
        //getting the size of the file
        const std::streamsize size = fin.tellg();
        if (size > 4096 - ROM_START_ADDRESS) {
            std::cout << "File too big!" << std::endl;
            std::exit(EXIT_FAILURE);
        }
        //setting the reader back to the beginning of the file
        fin.seekg(0, std::ios_base::beg);

        std::vector<char> buffer(size);
        if (fin.read(buffer.data(), size)) {
            for (size_t i = 0; i < size; i++)
                memory[ROM_START_ADDRESS + i] = buffer[i];
            std::cout << "File loaded successfully!" << std::endl;
        }

        fin.close();
    }
    else
        std::cout << "Loading file failed!" << std::endl;
}

void Chip8::UpdateTimers() {
    if (timerDelay > 0)
        timerDelay--;
    if (timerSound > 0)
        timerSound--;
}

//CLS - clearing the display
void Chip8::OP_00E0() {
    memset(video, 0, sizeof(video));
}
//RET - return from a subroutine
void Chip8::OP_00EE() {
    sp--;
    pc = stack[sp];
}
//JMP - jump to address nnn
void Chip8::OP_1nnn() {
    uint16_t address = opcode & 0x0FFFu;
    pc = address;
}
//CALL - Call subroutine at nnn
void Chip8::OP_2nnn() {
    uint16_t address = opcode & 0x0FFFu;
    stack[sp++] = pc;
    pc = address;
}
//SE Vx, byte - skip instruction if Vx == kk
void Chip8::OP_3xkk() {
    uint16_t kk = opcode & 0x00FFu;
    unsigned int reg = (opcode & 0x0F00u) >> 8u;
    if (registers[reg] == kk)
        pc+=2;
}
//SNE Vx, byte - skip instruction if Vx != kk
void Chip8::OP_4xkk() {
    uint16_t kk = opcode & 0x00FFu;
    unsigned int reg = (opcode & 0x0F00u) >> 8u;
    if (registers[reg] != kk)
        pc+=2;
}
//SE Vx, Vy - skip instruction if Vx == Vy
void Chip8::OP_5xy0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    if (registers[Vx] == registers[Vy])
        pc+=2;
}
//LD Vx, byte - set Vx to kk
void Chip8::OP_6xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = opcode & 0x00FFu;
    registers[Vx] = kk;
}
//ADD Vx, byte - add kk to Vx
void Chip8::OP_7xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = opcode & 0x00FFu;
    registers[Vx] += kk;
}
//LD Vx, Vy - set Vx to Vy
void Chip8::OP_8xy0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    registers[Vx] = registers[Vy];
}
//OR Vx, Vy - Vx = Vx OR Vy
void Chip8::OP_8xy1() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    registers[Vx] = registers[Vx] | registers[Vy];
}
//AND Vx, Vy - Vx = Vx AND Vy
void Chip8::OP_8xy2() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    registers[Vx] = registers[Vx] & registers[Vy];
}
//XOR Vx, Vy - Vx = Vx XOR Vy
void Chip8::OP_8xy3() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    registers[Vx] = registers[Vx] ^ registers[Vy];
}
//ADD Vx, Vy - Vx = Vx + Vy, VF = carry
void Chip8::OP_8xy4() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint16_t sum = registers[Vx] + registers[Vy];
    uint8_t flag = (sum >255u) ? 1 : 0;
    registers[Vx] = sum & 0xFFu;
    registers[0xF] = flag;

}
//SUB Vx, Vy - Vx = Vx - Vy, VF = NOT borrowing
void Chip8::OP_8xy5() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint8_t flag = (registers[Vx] >= registers[Vy]) ? 1 : 0;
    registers[Vx] = registers[Vx] - registers[Vy];
    registers[0xF] = flag;
}
//SHR Vx - Vx shifted right, LSB stored in VF, Vy ignored
void Chip8::OP_8xy6() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t flag = registers[Vx] & 0x1u;
    registers[Vx] = registers[Vx] >> 1;
    registers[0xF] = flag;
}
//SUBN Vx, Vy - Vx = Vy - Vx, VF = NOT borrowing
void Chip8::OP_8xy7() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint8_t flag = registers[Vy] >= registers[Vx] ? 1 : 0;
    registers[Vx] = registers[Vy] - registers[Vx];
    registers[0xF] = flag;
}
//SHL Vx - Vx shifted left, MSB stored in VF, Vy ignored
void Chip8::OP_8xyE() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t flag = (registers[Vx] & 128u) >> 7u;
    registers[Vx] = registers[Vx] << 1;
    registers[0xF] = flag;
}
//SNE Vx, Vy - skip instruction if Vx != Vy
void Chip8::OP_9xy0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    if (registers[Vx] != registers[Vy])
        pc+=2;
}
//LD I, addr - I = nnn
void Chip8::OP_Annn() {
    index = opcode & 0x0FFFu;
}
//JP V0, addr - jump to location V0 + nnn
void Chip8::OP_Bnnn() {
    uint16_t addr = opcode & 0x0FFFu;
    pc = registers[0x0] + addr;
}
//RND Vx, byte - Vx = randByte AND kk
void Chip8::OP_Cxkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t kk = opcode & 0x00FFu;
    registers[Vx] = randByte(RNG) & kk;
}
//DRW Vx, Vy, nibble - diplay an n-byte sprite starting at memory location I (Vx,Vy), VF = collision
void Chip8::OP_Dxyn() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    //we know that a sprite's width is 8 pixels
    uint8_t height = opcode & 0x000Fu;

    //wrap if exceeding screen boundaries
    uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
    uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;

    registers[0xF] = 0;

    for (unsigned int row = 0; row < height; row++) {
        uint8_t spriteByte = memory[index+row];

        for (unsigned int col = 0; col < 8; col++) {
            uint8_t spritePixel = spriteByte & (128u >> col);
            uint32_t* videoPixel = &video[((yPos+row) % VIDEO_HEIGHT) * VIDEO_WIDTH + ((xPos+col) % VIDEO_WIDTH)];

            if (spritePixel) {
                //collision
                if (*videoPixel == 0xFFFFFFFF)
                    registers[0xF] = 1;
                *videoPixel = *videoPixel ^ 0xFFFFFFFF;
            }
        }
    }
}
//SKP Vx - skip instruction if key with the value of Vx is pressed
void Chip8::OP_Ex9E() {
    uint8_t Vx = (opcode & 0x0F00) >> 8u;
    uint8_t key = registers[Vx];
    if (keyboard[key])
        pc+=2;
}
//SKNP Vx - skip instruction if key with the value of Vx is NOT pressed
void Chip8::OP_ExA1() {
    uint8_t Vx = (opcode & 0x0F00) >> 8u;
    uint8_t key = registers[Vx];
    if (!keyboard[key])
        pc+=2;
}
//LD Vx, DT - Vx = delay timer value
void Chip8::OP_Fx07() {
    uint8_t Vx = (opcode & 0x0F00) >> 8u;
    registers[Vx] = timerDelay;
}
//LD Vx, K - wait for a key press, store the key value in Vx
void Chip8::OP_Fx0A() {
    uint8_t Vx = (opcode & 0x0F00) >> 8u;
    if (keyboard[0])
        registers[Vx] = 0;
    else if (keyboard[1])
        registers[Vx] = 1;
    else if (keyboard[2])
        registers[Vx] = 2;
    else if (keyboard[3])
        registers[Vx] = 3;
    else if (keyboard[4])
        registers[Vx] = 4;
    else if (keyboard[5])
        registers[Vx] = 5;
    else if (keyboard[6])
        registers[Vx] = 6;
    else if (keyboard[7])
        registers[Vx] = 7;
    else if (keyboard[8])
        registers[Vx] = 8;
    else if (keyboard[9])
        registers[Vx] = 9;
    else if (keyboard[10])
        registers[Vx] = 10;
    else if (keyboard[11])
        registers[Vx] = 11;
    else if (keyboard[12])
        registers[Vx] = 12;
    else if (keyboard[13])
        registers[Vx] = 13;
    else if (keyboard[14])
        registers[Vx] = 14;
    else if (keyboard[15])
        registers[Vx] = 15;
    else
        //creating a cycle to wait for a key being pressed
        pc-=2;
}
//LD DT, Vx - set delay timer value to Vx
void Chip8::OP_Fx15() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    timerDelay = registers[Vx];
}
//LD ST, Vx - set sound timer value to Vx
void Chip8::OP_Fx18() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    timerSound = registers[Vx];
}
//ADD I, Vx - I = I + Vx
void Chip8::OP_Fx1E() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    index = index + registers[Vx];
}
//LD F, Vx - puts the memory location of the digit in Vx into I
void Chip8::OP_Fx29() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t digit = registers[Vx];
    //every digit is 5 bytes
    index = FONTSET_START_ADDRESS + (5*digit);
}
//LD B, Vx - store Vx in BCD format at I, I+1 and I+2
void Chip8::OP_Fx33() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t value = registers[Vx];
    memory[index] = value / 100;
    value -= memory[index]*100;
    memory[index+1] = value / 10;
    value -= memory[index+1]*10;
    memory[index+2] = value;
}
//LD [I], Vx - stores V0...Vx starting at location I
void Chip8::OP_Fx55() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    for (unsigned int i = 0; i <= Vx; i++) {
        memory[index+i] = registers[i];
    }
    //index = index + Vx + 1;
}
//LD Vx, [I] - reads registers V0....Vx starting at location I
void Chip8::OP_Fx65() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    for (unsigned int i = 0; i <= Vx; i++) {
        registers[i] = memory[index+i];
    }
    //index = index + Vx + 1;
}

//Functions for searching in Tables
void Chip8::Table0() {
    ((*this).*(table0[opcode & 0x000Fu]))();
}
void Chip8::Table8() {
    ((*this).*(table8[opcode & 0x000Fu]))();
}
void Chip8::TableE() {
    ((*this).*(tableE[opcode & 0x000Fu]))();
}
void Chip8::TableF() {
    ((*this).*(tableF[opcode & 0x00FFu]))();
}
//this is for safety, does nothing
void Chip8::OP_NULL() {}

//CPU Cycle
void Chip8::Cycle() {

    //putting together the operation because it's 16 bits, and it's stored in two memory addresses
    opcode = (memory[pc] << 8u) | memory[pc+1];

    //incrementing pc
    pc+=2;

    //executing the operation using the tables
    ((*this).*(table[(opcode & 0xF000u) >> 12u]))();
}