#include <iostream>
#include <cstdint>
#include <array>
#include <fstream>
#include <chrono>
#include <thread>
#include <cstring>

class Chip8 {
public:
    uint8_t memory[4096]; //used for storing program data, font etc . 4090 bytes
    uint8_t V[16];         //16 8 bit general purpose register
    uint16_t I;            //Index register
    uint16_t pc;           
    uint16_t stack[16];    
    uint8_t sp;            
    uint8_t delayTimer;    //used for timing animations or input delay 
    uint8_t soundTimer;    //beeps when i treaches 0
    std::array<bool, 64 * 32> display;
    std::array<bool, 16> keypad;
    bool drawFlag;          

    Chip8() {
        reset();
    }

    void reset() {
        pc = 0x200;
        I = 0;
        sp = 0;
        delayTimer = 0;
        soundTimer = 0;
        drawFlag = false;
        std::fill(std::begin(memory), std::end(memory), 0);
        std::fill(std::begin(V), std::end(V), 0);
        std::fill(std::begin(stack), std::end(stack), 0);
        std::fill(std::begin(display), std::end(display), false);
        std::fill(std::begin(keypad), std::end(keypad), false);
    }

    void loadProgram(const uint8_t* program, size_t size) {
        for (size_t i = 0; i < size; i++) {
            memory[0x200 + i] = program[i];
        }
    }

    void executeCycle() {
        uint16_t opcode = memory[pc] << 8 | memory[pc + 1]; //program counter stores the intrusction to be executed next
        // right shift 8 means u get higher 8 bit and bitwise or gets lower 8 bits. 
        std::cout << "Executing opcode: " << std::hex << opcode << "\n";
        pc += 2; //opcode in this is 2 bytes long so inc by 2

        switch (opcode & 0xF000) { //here bitwinse & is used to extract the Most significant 4 bits
            case 0x0000: 
                if (opcode == 0x00E0) { 
                    std::cout << "Clearing display\n";
                    std::fill(display.begin(), display.end(), false); //sets it to false (off)
                    drawFlag = true; //tells emulator to draw screen basically
                } else if (opcode == 0x00EE) {
                    std::cout << "Returning from subroutine\n";
                    pc = stack[--sp];
                }
                break;

            case 0x1000: //used to jump to a mem addr 
                std::cout << "Jumping to address: " << (opcode & 0x0FFF) << "\n";
                pc = opcode & 0x0FFF; //0FFF is known as bitmask  it will extract the last 12 bits 1NNN instruction
                break;

            case 0x2000: //this is used to call subroutine at mem location 2XXX
                std::cout << "Calling subroutine at: " << (opcode & 0x0FFF) << "\n";
                stack[sp++] = pc; //storing the last instruction in the stack to know where to return to after performing jmp
                pc = opcode & 0x0FFF; //storing in pc where to jump cus PC only stores next intruction to be executed
                break;

            case 0x3000:
                std::cout << "Checking if V[" << ((opcode & 0x0F00) >> 8) << "] == " << (opcode & 0x00FF) << "\n";
                if (V[(opcode & 0x0F00) >> 8] == (opcode & 0x00FF)) pc += 2; //pc+=2 cus the instruction skips next instruction
                break;

            case 0x4000: //skip if not equal instruction 
                std::cout << "Checking if V[" << ((opcode & 0x0F00) >> 8) << "] != " << (opcode & 0x00FF) << "\n";
                if (V[(opcode & 0x0F00) >> 8] != (opcode & 0x00FF)) pc += 2; //exact same as SE but instead != now
                break;

            case 0x5000: //if 2nd nibble is equal to the third nibble , it skips (from left its counted)
                std::cout << "Checking if V[" << ((opcode & 0x0F00) >> 8) << "] == V[" << ((opcode & 0x00F0) >> 4) << "]\n";
                if (V[(opcode & 0x0F00) >> 8] == V[(opcode & 0x00F0) >> 4]) pc += 2;
                break;

            case 0x6000:
                std::cout << "Setting V[" << ((opcode & 0x0F00) >> 8) << "] to " << (opcode & 0x00FF) << "\n";
                V[(opcode & 0x0F00) >> 8] = opcode & 0x00FF;
                break;

            case 0x7000: //adds immediate value
                std::cout << "Adding " << (opcode & 0x00FF) << " to V[" << ((opcode & 0x0F00) >> 8) << "]\n";
                V[(opcode & 0x0F00) >> 8] += opcode & 0x00FF;
                break;

            case 0x8000:
                switch (opcode & 0x000F) { //we get the 1st nibble and check what to do
                    case 0x0000: //copy to V register
                        std::cout << "Copying V[" << ((opcode & 0x00F0) >> 4) << "] to V[" << ((opcode & 0x0F00) >> 8) << "]\n";
                        V[(opcode & 0x0F00) >> 8] = V[(opcode & 0x00F0) >> 4];
                        break;
                    case 0x0001://bitwise or and then copy
                        std::cout << "V[" << ((opcode & 0x0F00) >> 8) << "] |= V[" << ((opcode & 0x00F0) >> 4) << "]\n";
                        V[(opcode & 0x0F00) >> 8] |= V[(opcode & 0x00F0) >> 4];
                        break;
                    case 0x0002://bitwise and and then copy
                        std::cout << "V[" << ((opcode & 0x0F00) >> 8) << "] &= V[" << ((opcode & 0x00F0) >> 4) << "]\n";
                        V[(opcode & 0x0F00) >> 8] &= V[(opcode & 0x00F0) >> 4];
                        break;
                    case 0x0003: //bitwise xor and then copy
                        std::cout << "V[" << ((opcode & 0x0F00) >> 8) << "] ^= V[" << ((opcode & 0x00F0) >> 4) << "]\n";
                        V[(opcode & 0x0F00) >> 8] ^= V[(opcode & 0x00F0) >> 4];
                        break;
                }
                break;

            case 0xA000: //extracts last 12 bits and stores in index register
                std::cout << "Setting I to " << (opcode & 0x0FFF) << "\n";
                I = opcode & 0x0FFF;
                break;

            case 0xD000:
                std::cout << "Drawing sprite\n";
                drawFlag = true; //this set to true means display needs to be updated 
                break;

            default: //some opcode not recognised then this
                std::cout << "Unknown opcode: " << std::hex << opcode << "\n";
        }

        if (delayTimer > 0) delayTimer--; //decrements once every 1/60th a second (60hz) so 
        if (soundTimer > 0) soundTimer--;
    }


    void run() {
        while (true) {
            executeCycle();
            if (drawFlag) {
                drawScreen();
                drawFlag = false;
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(16)); //16ms delay
        }
    }

    void drawScreen() {
        std::cout << "\033[2J\033[H"; //special escape sequence that clears the entire screen
        for (int y = 0; y < 32; y++) {
            for (int x = 0; x < 64; x++) {
                std::cout << (display[x + y * 64] ? "█" : " ");// when storing 2d position in !d array , we use this formula
                //for calculating index (2d cus width and height we have but array is 1d) index = x+y*width
                //since its 64 pixels wide and 32 pixels tall
            }
            std::cout << "\n";
        }
    }

    void saveState(const std::string& filename) { //takes the filename as input and stores all the states as binary file
        std::ofstream file(filename, std::ios::binary); //opens bin file 
        if (file.is_open()) { //checks if file is open
            //reinterept_cast convers an unisigned integer to raw binary
            file.write(reinterpret_cast<const char*>(memory), sizeof(memory));
            file.write(reinterpret_cast<const char*>(V), sizeof(V));
            file.write(reinterpret_cast<const char*>(&I), sizeof(I));
            file.write(reinterpret_cast<const char*>(&pc), sizeof(pc));
            file.write(reinterpret_cast<const char*>(&sp), sizeof(sp));
            file.write(reinterpret_cast<const char*>(stack), sizeof(stack));
            file.write(reinterpret_cast<const char*>(&delayTimer), sizeof(delayTimer));
            file.write(reinterpret_cast<const char*>(&soundTimer), sizeof(soundTimer));
            file.write(reinterpret_cast<const char*>(display.data()), display.size());
            file.write(reinterpret_cast<const char*>(keypad.data()), keypad.size());
            file.close();
            std::cout << "State saved to " << filename << "\n";
        }
    }

    void loadState(const std::string& filename) {
        std::ifstream file(filename, std::ios::binary);
        if (file.is_open()) {
            file.read(reinterpret_cast<char*>(memory), sizeof(memory));
            file.read(reinterpret_cast<char*>(V), sizeof(V));
            file.read(reinterpret_cast<char*>(&I), sizeof(I));
            file.read(reinterpret_cast<char*>(&pc), sizeof(pc));
            file.read(reinterpret_cast<char*>(&sp), sizeof(sp));
            file.read(reinterpret_cast<char*>(stack), sizeof(stack));
            file.read(reinterpret_cast<char*>(&delayTimer), sizeof(delayTimer));
            file.read(reinterpret_cast<char*>(&soundTimer), sizeof(soundTimer));
            file.read(reinterpret_cast<char*>(display.data()), display.size());
            file.read(reinterpret_cast<char*>(keypad.data()), keypad.size());
            file.close();
            std::cout << "State loaded from " << filename << "\n";
        }
    }
};

int main() {
    Chip8 chip8;
    uint8_t testProgram[] = { 0x60, 0x0A, 0x61, 0x05, 0x70, 0x01, 0xA2, 0x0F };
    chip8.loadProgram(testProgram, sizeof(testProgram));

    for (int i = 0; i < 5; i++) {  
        chip8.executeCycle();

        std::cout << "Registers: ";
        for (int j = 0; j < 16; j++) {
            std::cout << "V[" << j << "]: " << +chip8.V[j] << " "; //prints content of all 15 V registers
        }
        std::cout << "I: " << chip8.I << " PC: " << chip8.pc << "\n";
    }

    return 0;
}



//Opcodes and what they do
/*00E0	Clear the screen (CLS)
00EE	Return from subroutine (RET)
1NNN	Jump to address NNN (JP addr)
2NNN	Call subroutine at NNN (CALL addr)
3XNN	Skip next instruction if Vx == NN (SE Vx, NN)
6XNN	Load NN into Vx (LD Vx, NN)
7XNN	Add NN to Vx (ADD Vx, NN)*/

//opcode formate- first number - intruction type
//second number - specifies with Vx register to use
//3 n 4 - immediate values