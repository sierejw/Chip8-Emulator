# CMakeList.txt : CMake project for Chip8_Emulator, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

project ("Chip8_Emulator")

find_package(SDL3 REQUIRED CONFIG REQUIRED COMPONENTS SDL3-shared)

# Add source to this project's executable.
add_executable (Chip8_Emulator "src/main.cpp" "src/Chip8.h" "src/Chip8.cpp")

target_link_libraries(Chip8_Emulator PRIVATE SDL3::SDL3)

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET Chip8_Emulator PROPERTY CXX_STANDARD 23)
endif()

if(WIN32)
    add_custom_command(
        TARGET Chip8_Emulator POST_BUILD
        COMMAND "${CMAKE_COMMAND}" -E copy $<TARGET_FILE:SDL3::SDL3-shared> $<TARGET_FILE_DIR:Chip8_Emulator>
        VERBATIM
    )
endif()
