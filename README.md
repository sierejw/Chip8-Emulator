Simple Chip8 Emulator in C++. Chip8 is an interpreted language from the 1970s. This emulator is modeled after the original version created for the COSMAC VIP and Telmac 1800. Using this project as an entry into lower level development and emulation.

Requires: SDL3 and CMake

Compiling: 

1. Create folder for build
2. cmake [src dir]
3. cmake --build .

Running: 
1. Go into Debug folder
2. Chip8_Emulator.exe [path to rom]

Additional Arguments:

-b, flag that enables quirk for older implementations of bitshifting where value in variable register x is replaced with the value in variable register y (recommended)
-m, changes how emulator increments in memory when copying values from variable registers. When on, will increment value of index register (recommended)
-o, when on, adds value of variable register x to jump address, otherwise will add value at variable register 0 to jump address. (not recommended)
-v, enables vertical interrupt used in original Chip8. (recommended)

All recommended arguements are to emulate original Chip8 behavior. 


References:

Guide by Tobias V. Langhoff: https://tobiasvl.github.io/blog/write-a-chip-8-emulator/
Test Roms provided by Timendus here: https://github.com/Timendus/chip8-test-suite?tab=readme-ov-file
Shoutout to r/EmuDev discord server for clarification and guidance with finishing touches
