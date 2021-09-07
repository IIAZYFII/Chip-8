#include "Chip8.hpp"
#include <cstdint>
#include <stdio.h>
#include <time.h>
#include <stdint.h>
#include <stdlib.h>
#include <iostream>
#include <cstring>


const unsigned int FONTSET_SIZE = 80;
const unsigned int FONTSET_START_ADDRESS = 0x50;
const unsigned int START_ADDRESS = 0x200;   

	uint8_t memory[MEMORY_SIZE]{};
	uint8_t registers[REGISTER_COUNT]{};
	uint16_t index{};
	uint16_t pc{};
	uint8_t delayTimer{};
	uint8_t soundTimer{};
	uint16_t stack[STACK_LEVELS]{};
	uint8_t sp{};
	uint16_t opcode{};
    uint8_t randByte{};
    uint32_t video[64 * 32]{};
    uint8_t keypad[16]{};

uint8_t generateRandomNumber() {
    uint8_t randomNumber;

    srand(time(NULL));
    
    randomNumber = rand() % 256;

    return randomNumber;
} 
void loadFont() {
    
    uint8_t fontset[FONTSET_SIZE] ={
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
	0xF0, 0x80, 0xF0, 0x80, 0x80 // F
    };  

    for( unsigned int  i = 0; i < FONTSET_SIZE; ++ i) {
        memory[FONTSET_START_ADDRESS + i] = fontset[i];
    }


}

 Chip8::Chip8() {
    
     // Initialize PC
	pc = START_ADDRESS;

    //Load Fonts in memory
    loadFont();

    randByte = generateRandomNumber();
 }

 //CLS - clear the display
void OP_00E0() {
     memset(video, 0, sizeof(video));
}

// RET  - Return from a subroutine
void OP_00EE() {
    --sp;
    pc = stack[sp];
}

//JP addr - Jump to loaction nnn
void OP_1nnn() {
    uint16_t address = opcode & 0x0FFu;

    pc = address;
}

//CALL addr  - call subroutine at nnn
void OP_2nnn() {
    uint16_t address = opcode & 0xFFFu;

    stack[sp] = pc;
    ++sp;
    pc = address;
}

// SE Vx, byte - Skip next instructions  if Vx = kk
void OP_3xKK() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFU;
    
    if(registers[Vx] == byte) {
        pc += 2;
    }
}

// SNE Vx, byte - Skip next instruction if Vx != kk
void OP_4xkk(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00Fu;

    if (registers[Vx] != byte) {
        pc += 2;
    }
}

// SE Vx, Vy - Skip next instruction if Vx = Vy
 void OP_5xy0() {
     uint8_t Vx = (opcode & 0x0F00u) >> 8u;
     uint8_t Vy = (opcode & 0x00F0u) >> 4u;

     if (registers[Vx] == registers[Vy]) {
         pc += 2;
     }
}

//LD VX, byte - Set Vx = kk
void OP_6xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] = byte;
}

//ADD Vx, byte - Set Vx = Vx + KK
void OP_7xkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t byte = opcode & 0x00FFu;

    registers[Vx] += byte;
}

//LD Vx, Vy - Set Vx = Vy
void OP_8xy0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	registers[Vx] = registers[Vy];
}

//OR Vx, Vy - Set Vx = Vx  OR Vy
void OP_8xy1() {
     uint8_t Vx = (opcode & 0x0F00u) >>  8u;
     uint8_t Vy =  (opcode & 0x00F0u) >> 4u;
     
     registers[Vx] |= registers[Vy];
}

//AND Vx, Vy - Set Vx AND Vy
void OP_8xy2() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    registers[Vx] &= registers[Vy];
}

// XOR Vx, Vy - Set Vx = Vx XOR Vy
void OP_8xy3() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode * 0x00F0u) >> 4u;

    registers[Vx] ^= registers[Vy];
}

//ADD Vx, Vy - Set  Vx + Vy, set VF = carry
void OP_8xy4(){
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    uint16_t sum = registers[Vx] + registers[Vy];

    if (sum > 255U) {
        registers[0xF] = 1;
    } else {
        registers[0xF] = 0;
    }
    registers[Vx] = sum & 0xFFu;
}

//SUB Vx, Vy = Set Vx = Vx - Vy, set VF = NOT borrow
void OP_8xy5() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vx] > registers[Vy]) {
        registers[0xF] = 1;
    } else {
        registers[0xF] = 0;
    }

    registers[Vx] -= registers[Vy];
}

//SHR Vx - Set Vx = Vx SHR 1
void OP_8xy6() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    //Save LSB in VF
    registers[0xF] = (registers[Vx] & 0x1u);

    registers[Vx] >>= 1;

}

//SUBN Vx, Vy - Set Vx = Vy - Vx, set VF = NOT borrow
void OP_8xy7() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;

    if (registers[Vy] > registers[Vx]) {

        registers[0xF] = 1;
    } else {
        registers[0xF] = 0;
    }

    registers[Vx] = registers[Vy] - registers[Vx];
}

//SHL Vx {, Vy} - Set Vx = Vx SHL 1
void OP_8xyE() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    //Save MSB in VF
    registers[0xF] = (registers[Vx] & 0x80u) >> 7u;

    registers[Vx] <<= 1;
}

//SNE Vx, Vy - Skip next instruction if Vx != Vy
void OP_9xy0() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t Vy = (opcode & 0x00F0u) >> 4u;

	if (registers[Vx] != registers[Vy]) {
		pc += 2;
	}
}

// Annn - LD I, addr - Set I = nnn
void OP_Annn() {
    uint16_t address = opcode & 0x0FFFu;

	index = address;
}

//Bnnn - JP V0, addr - Jump to location nnn + V0
void OP_Bnnn() {
   uint16_t address = opcode & 0x0FFFu;

	pc = registers[0] + address;
}
  
//CxKK - RND Vx, byte - Set Vx = random byte AND kk
void OP_Cxkk() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	uint8_t byte = opcode & 0x00FFu;

	registers[Vx] = generateRandomNumber() & byte;
} 

//Dxyn - DRW Vx, Vy, nibble - Display n-byte sprite starting at memory location I at (Vx, Vy), set VF = collision 
void OP_Dxyn() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t Vy = (opcode & 0x00F0u) >> 4u;
    uint8_t height = opcode & 0x000Fu;

    //Wrap if going beyond screen boundaries
    uint8_t xPos = registers[Vx] % VIDEO_WIDTH;
	uint8_t yPos = registers[Vy] % VIDEO_HEIGHT;

    registers[0xF] = 0;

   for (unsigned int row = 0; row < height; ++row) {
		uint8_t spriteByte = memory[index + row];

		for (unsigned int col = 0; col < 8; ++col) {
			uint8_t spritePixel = spriteByte & (0x80u >> col);
			uint32_t* screenPixel = &video[(yPos + row) * VIDEO_WIDTH + (xPos + col)];

			// Sprite pixel is on
			if (spritePixel) {
				// Screen pixel also on - collision
				if (*screenPixel == 0xFFFFFFFF) {
					registers[0xF] = 1;
				}

				// Effectively XOR with the sprite pixel
				*screenPixel ^= 0xFFFFFFFF;
			}
		}
	}
}

//Ex9E - SKP Vx - Skip next instruction if key with the value of Vx is pressed
void OP_Ex9E() {
    uint8_t Vx = (opcode * 0x0F00u) >> 8u;
    
    uint8_t key = registers[Vx];
    
    if(keypad[key]) {
        pc += 2;
    } 
 }

 //ExA1 - SKNP Vx - Skip next instruction if key with teh value of Vx is not pressed
 void OP_ExA1() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	uint8_t key = registers[Vx];

	if (!keypad[key]) {
		pc += 2;
	}

    
 }

 //Fx07 - LD Vx, DT - Set Vx = delay timer value
 void OP_Fx07() {
     uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    registers[Vx] = delayTimer;
 }

 //Fx0A - LD Vx, K - Wait for a key press, store the value of the key in Vx
 void OP_Fx0A() {
     uint8_t Vx = (opcode & 0x0F00u) >> 8u;

	if (keypad[0])
	{
		registers[Vx] = 0;
	}
	else if (keypad[1])
	{
		registers[Vx] = 1;
	}
	else if (keypad[2])
	{
		registers[Vx] = 2;
	}
	else if (keypad[3])
	{
		registers[Vx] = 3;
	}
	else if (keypad[4])
	{
		registers[Vx] = 4;
	}
	else if (keypad[5])
	{
		registers[Vx] = 5;
	}
	else if (keypad[6])
	{
		registers[Vx] = 6;
	}
	else if (keypad[7])
	{
		registers[Vx] = 7;
	}
	else if (keypad[8])
	{
		registers[Vx] = 8;
	}
	else if (keypad[9])
	{
		registers[Vx] = 9;
	}
	else if (keypad[10])
	{
		registers[Vx] = 10;
	}
	else if (keypad[11])
	{
		registers[Vx] = 11;
	}
	else if (keypad[12]) {
		registers[Vx] = 12;
	}
	else if (keypad[13]) {
		registers[Vx] = 13;
	}
	else if (keypad[14]) {
		registers[Vx] = 14;
	}
	else if (keypad[15]) {
		registers[Vx] = 15;
	}
	else {
		pc -= 2;
	
    }
 } 

//Fx15 - LD DT, VX - Set delay timer = Vx
void OP_Fx15() {
uint8_t Vx = (opcode & 0x0F00u) >> 8u;
	delayTimer = registers[Vx];

 }
 //FX18 - LD ST,  - Set sound timer = Vx
 void OP_Fx18(){
     uint8_t Vx = (opcode & 0x0F00u) >> 8u;

     soundTimer = registers[Vx];
 }
 
 //FX1E - ADD I, VX = Set I = I + Vx
 void OP_FX1E(){
     uint8_t Vx = (opcode & 0x0F00u) >> 8u;

     index += registers[Vx];
 }

 //Fx29 - LD F, Vx - Set I = location of sprite for digit Vx
 void OP_Fx29() {
     uint8_t Vx = (opcode & 0x0F00u) >> 8u;
     uint8_t digit = registers[Vx];

     index = FONTSET_START_ADDRESS + (5 * digit);
    }

//Fx33 - LD B, VX - Store BCD representation of Vx in memory locations I, I+1, and I+2
void OP_Fx33() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;
    uint8_t value = registers[Vx];

    //Ones-place 
    memory[index + 2] = value % 10;
    value /= 10;

    //Tens-place 
    memory[index + 1] = value % 10;
    value /= 10;

    //Hundreds-Place
    memory[index] = value % 10;
}

//Fx55 LD [I], Vx - Store registers V0 through Vx in memory starting at location I
void OP_Fx55() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    for (uint8_t i = 0; i <= Vx; ++i) {
        memory[index + 1] = registers[i];
    }
}

//Fx65 - LD Vx, [I] - Read Registers V0 through Vx from memory starting at location I.
void OP_Fx65() {
    uint8_t Vx = (opcode & 0x0F00u) >> 8u;

    for (uint8_t i = 0; i <= Vx; ++i) {
        registers[i] = memory[index + 1];
    }
}


 //Decode the OPCode
void Decode_OPCode() {
    switch (opcode & 0xF000) {
        case 0x0000:
                switch (opcode & 0x00FF) {
                case 0x00E0:
                OP_00E0();
                break;
                case 0x00EE:
                    OP_00EE();
                        break;   
                    default:
                            printf("Unkown op: 0x%X\n", opcode);
                        break;
        }
        break;
        case 0x1000:
            OP_1nnn();
            break;
        case 0x200:
            OP_2nnn();
            break;
        case 0x3000:
            OP_3xKK();
            break;
        case 0x4000:
            OP_4xkk();
            break;
        case 0x5000:
            OP_5xy0();
            break;
        case 0x6000:
            OP_6xkk();
            break;
        case 0x7000:
            OP_7xkk();
            break;
        case 0x8000:
            switch (opcode & 0x00F) {
            case 0x0000:
                OP_8xy0();
                break;
            case 0x0001:
                OP_8xy1();
                break;
            case 0x0002:
                OP_8xy2();
                break;
            case 0x0003:
                OP_8xy3();
                break;
            case 0x0004:
                OP_8xy4();
                break;
            case 0x0005:
                OP_8xy5();
                break;
            case 0x0006:
                OP_8xy6();
                break;
            case 0x0007:
                OP_8xy7();
                break;
            case 0x000E:
                OP_8xyE();
                break;
            default:
                printf("Uknown op: 0x%X\n", opcode);
                break;
            }
            break; 
        case 0x9000:
            OP_9xy0();
            break;
        case 0xA000:
            OP_Annn();
            break;
        case 0xB000:
            OP_Bnnn();
            break;
        case 0xC000:
            OP_Cxkk();
            break;
        case 0xD000:
            OP_Dxyn;
            break;
        case 0xE000:
            switch (opcode & 0x00FF) {
            case 0x009E:
                OP_Ex9E();
                break;
            case 0x00A1:
                OP_ExA1();
                break;       
            default:
                printf("Unknown op: 0x%X\n", opcode);
                break;
            }    
             break;
        case 0xF00:
            switch (opcode & 0x00FF) {
            case 0x0007:
                OP_Fx07();
                break;
            case 0x000A:
                OP_Fx0A();
                break;
            case 0x0015:
                OP_Fx15();
                break;
            case 0x0018:
                OP_Fx18();
                break;
            case 0x001E:
                OP_FX1E();
                break;
            case 0x0029:
                OP_Fx29();
                break;
            case 0x0033:
                OP_Fx33();
                break;
            case 0x0055:
                OP_Fx55();
                   break;
             case 0x0065:
                OP_Fx65();
                break;
             default:
                printf("Unknown op: 0x%X\n", opcode);
                break;
            }
            default:
                printf("Unknown op: 0x%X\n", opcode);
                break;


    }
}

 void Chip8::cycle()
{
	// Fetch
	opcode = (memory[pc] << 8u) | memory[pc + 1];

	// Increment the PC before we execute anything
	pc += 2;

	// Decode and Execute
	Decode_OPCode();

	// Decrement the delay timer if it's been set
	if (delayTimer > 0)
	{
		--delayTimer;
	}

	// Decrement the sound timer if it's been set
	if (soundTimer > 0)
	{
		--soundTimer;
	}
}


 void loadRom(char const* filename) {
    // Open the file as a stream of binary and move the file pointer to the end
    FILE *fp;
    fp = fopen(filename, "rb");
    fseek(fp, 0,SEEK_END);

    if(fp != NULL) {
        //ftell returns the current file position of a specified stream
        // Get size of file
       unsigned int size = ftell(fp);
       // allocate a buffer to hold the contents
       char* buffer = new char[size];


        // Go back to the beginning of the file and fill the buffer
        fseek(fp, 0, SEEK_SET);
        fread(buffer, size, 0,fp);
    
        // Load the ROM contents into the Chip8's memory, starting at 0x200
        for (long i = 0 ; i < size; ++i) {
            memory[START_ADDRESS + i] = buffer[i];
        }

        //clears the array
        delete[] buffer;
         

    }
}





