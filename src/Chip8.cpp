#include "Chip8.h"
#include <fstream>
#include <limits>
#include <random>

constexpr std::size_t startAddress{ 512 };

Chip8::Chip8(): pc { startAddress }{}

// Assigns first 2 consecutive bytes in memory to Opcode and increments PC by 2
void Chip8::fetch() 
{
	opcode = (memory[pc] << 8) | memory[pc + 1];		
	pc += 2;
}

// Decodes and executes opcode instructions for one cycle
void Chip8::execute()
{
	uint8_t first = (opcode & 0xF000) >> 12;	// represents 1st nibble of opcode, tells what type of instruction it is
	uint8_t x = (opcode & 0x0F00) >> 8;			// represents 2nd nibble of opcode, looks up one of 16 registers
	uint8_t y = (opcode & 0x00F0) >> 4;			// represents 3rd nibble of opcode, looks up one of 16 registers
	uint8_t n = opcode & 0x000F;				// represents 4th nibble of opcode, holds a 4-bit number
	uint8_t nn = opcode & 0x00FF;				// represents second byte of opcode, holds an 8-bit number
	uint16_t nnn = opcode & 0x0FFF;				// holds 2nd, 3rd, and 4th nibble of opcode, holds a 12-bit number
	
	switch (first)				// Checks OpCode category
	{
	case (0x00):	

		switch (n)
		{
		case(0x00):				// 00E0: Clear Screen Opcode
			display.fill(0);
			break;

		case(0x0E):				// 00EE: Return from subroutine
			pc = stack.back();
			stack.pop_back();
			break;
		}

		break;

	case (0x01):				// 1NNN: Jump tp NNN (Doesn't push to stack)

		pc = nnn;
		break;

	case (0x02):				// 2NNN: Calls Subroutine at NNN (Pushes to stack)

		stack.push_back(pc);
		pc = nnn;
		break;

	case (0x03):				// 3XNN: Skip Conditionally - if VX equals NN

		if (vr[x] == nn) 
		{
			pc += 2;
		}
		break;

	case (0x04):				// 4XNN: Skip Conditionally - if VX does not equal NN
		if (vr[x] != nn)
		{
			pc += 2;
		}
		break;

	case (0x05):				// 5XY0: Skip Conditionally - VX and VY are equal

		if (vr[x] == vr[y])
		{
			pc += 2;
		}
		break;

	case (0x06):				// 6XNN: Set value at Register X to NN

		vr[x] = nn;
		break;

	case (0x07):				// 7XNN: Add NN to Value at Register X Opcode

		vr[x] += nn;
		break;

	case (0x08):

		switch (n)
		{
		case (0x00):				// 8XY0: Set VX to value of VY

			vr[x] = vr[y];
			break;

		case (0x01):				// 8XY1: Set VX to bitwise OR of VX and VY

			vr[x] |= vr[y];
			break;

		case (0x02):				// 8XY2: Set VX to bitwise AND of VX and VY

			vr[x] &= vr[y];
			break;

		case (0x03):				// 8XY3: Set VX to logical XOR of VX and VY

			vr[x] ^= vr[y];
			break;

		case (0x04):				// 8XY4: Set VX to sum of VX and VY. Checks for overflow, sets VF to 1 if overflow occurs.

			if (vr[x] > (std::numeric_limits<uint8_t>::max() - vr[y])) 
			{
				vr[0x0F] = 1;
			}

			vr[x] += vr[y];
			break;

		case (0x05):				// 8XY5: Set VX to difference of VX - VY. Checks for underflow, sets VF to 0 if underflow occurs, 1 otherwise.

			if (vr[y] >= vr[x]) {
				vr[0x0F] = 1;
			}
			else {
				vr[0x0F] = 0;
			}

			vr[x] = vr[y] - vr[x];
			break;

		case (0x06):				// 8XY6: Bitshift VX by one to the right, store shifted out bit to VF. (Optional: Sets VX to VY before shifting bits. Feature for older roms expecting this behavior)

			if (bitshiftFlag) {
				vr[x] = vr[y];
			}
			vr[0x0F] = vr[x] & 0x01;
			vr[x] >>= 1;
			break;

		case (0x07):				// 8XY7: Set VX to difference of VY - VZ. Checks for underflow, sets VF to 0 if underflow occurs, 1 otherwise.

			if (vr[x] >= vr[y]) {
				vr[0x0F] = 1;
			}
			else {
				vr[0x0F] = 0;
			}

			vr[x] -= vr[y];
			break;

		case (0x0E):				// 8XYE: Bitshift VX by one to the left, store shifted out bit to VF. (Optional: Sets VX to VY before shifting bits. Feature for older roms expecting this behavior)

			if (bitshiftFlag) {
				vr[x] = vr[y];
			}
			vr[0x0F] = vr[x] & 0x80;
			vr[x] <<= 1;
			break;

		}

		break;
		
	case (0x09):				// 9XY0: Skip Conditionally - VX doesn't equal VY

		if (vr[x] != vr[y])
		{
			pc += 2;
		}
		break;

	case (0x0A):				// ANNN: Set Index Register I Opcode

		I = nnn;
		break;
	
	case (0x0B):				// BXNN: Jump to address NNN or XNN (same thing). If offsetFlag is true, will add value in register VX to address, otherwise add V0 to address. (Implemented for different implentations of this opcode)


		uint8_t tag{};

		if (offsetFlag) 
		{
			tag = x;
		} else {
			tag = 0;
		}

		pc = nnn + vr[tag];
		break;

	case (0x0C):				// CXNN: Random, generate random number and bitwise and it with NN. Store into VX

		std::random_device rd{};
		std::seed_seq ss{ rd(), rd(), rd(), rd(), rd(), rd(), rd(), rd() };
		std::mt19937 mt{ ss };
		std::uniform_int_distribution rand{ 0, 255 };

		vr[x] = rand(mt) & nn;

		break;

	case (0x0D):				// DXYN: Display/Draw Opcode

		uint8_t xcoord = vr[x] & 63;
		uint8_t xcoord2 = vr[x] & 63;
		uint8_t ycoord = vr[y] & 31;
		uint8_t ycoord2 = vr[y] & 31;
		vr[0x0F] = 0;
		for (int row = 0; row < n; row++) 
		{
			uint8_t currSprite = memory[I + row];
			for (int col = 0; col < 8; col++)
			{
				if((currSprite >> (7 - col)) & 1)
				{
					if(display[xcoord + col + ((ycoord + row) * 64) % (64 * 32)])
					{
						vr[0x0F] = 1;
					}

					display[xcoord + col + ((ycoord + row) * 64) %  (64 * 32)] ^= 1;
				}
			}
		}

		break;

	}
}

bool Chip8::load(const std::string& filepath)
{
	std::ifstream rom{ filepath, std::ios::binary | std::ios::in };
	char value;
	

	if (!rom)
	{
		return false;
	}

	for (int i = 0; rom.get(value); i++) 
	{
		if (startAddress + i >= 4096) {
			return false;
		}

		memory[startAddress + i] = static_cast<uint8_t>(value);
	}
	return true;

}