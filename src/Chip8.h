﻿#pragma once

#include <iostream>
#include <cstdint>
#include <array>
#include <vector>

class Chip8
{
private:
	std::array<uint8_t, 4096> memory{ {0} };		// 4kB memory
	uint16_t I{ 0 };								// index register
	uint16_t pc{ 0 };								// program counter
	uint16_t opcode{ 0 };							// operation codes
	std::vector<uint16_t> stack{ {0} };				// stack
	uint8_t dtimer{ 0 };							// delay timer
	uint8_t stimer{ 0 };							// sound timer
	std::array<uint8_t, 16> vr{ {0} };				// variable registers
	
	const std::array<uint8_t, 80> font				// font - represents 16 hex characters. 4 pixels wide, 5 pixels tall. 5 bytes per character
	{
		0xF0, 0x90, 0x90, 0x90, 0xF0, //0
		0x20, 0x60, 0x20, 0x20, 0x70, //1
		0xF0, 0x10, 0xF0, 0x80, 0xF0, //2
		0xF0, 0x10, 0xF0, 0x10, 0xF0, //3
		0x90, 0x90, 0xF0, 0x10, 0x10, //4
		0xF0, 0x80, 0xF0, 0x10, 0xF0, //5
		0xF0, 0x80, 0xF0, 0x90, 0xF0, //6
		0xF0, 0x10, 0x20, 0x40, 0x40, //7
		0xF0, 0x90, 0xF0, 0x90, 0xF0, //8
		0xF0, 0x90, 0xF0, 0x10, 0xF0, //9
		0xF0, 0x90, 0xF0, 0x90, 0x90, //A
		0xE0, 0x90, 0xE0, 0x90, 0xE0, //B
		0xF0, 0x80, 0x80, 0x80, 0xF0, //C
		0xE0, 0x90, 0x90, 0x90, 0xE0, //D
		0xF0, 0x80, 0xF0, 0x80, 0xF0, //E
		0xF0, 0x80, 0xF0, 0x80, 0x80  //F
	};


public:

	Chip8();
	bool bitshiftFlag{ false };
	bool offsetFlag{ false };
	bool memoryFlag{ false };
	bool vInterruptFlag{ false };
	bool draw{ false };
	std::array<bool, 64 * 32> display{ {0} };      // 64 x 32 pixel display
	std::array<bool, 16> keys{ {0} };			   // input keys
	void fetch();
	void execute();
	bool load(const std::string& filepath);
	void updateTimers();
	bool playSound();

};