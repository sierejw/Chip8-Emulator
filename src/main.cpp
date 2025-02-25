#include "Chip8.h"
#include "SDL3/SDL.h"

int main(int argc, char* argv[])
{
	SDL_Window* window{ NULL };
	SDL_Renderer* renderer{ NULL };
	SDL_Texture* texture{ NULL };
	constexpr int width{ 700 };
	constexpr int height{ 400 };
	
	
	if (!SDL_Init(SDL_INIT_VIDEO))
	{
		return -1;
	}

	window = SDL_CreateWindow("Chip8 Emulator", width, height, 0);
	renderer = SDL_CreateRenderer(window, NULL);
	texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32);
	
	Chip8 emulation;
	if (!emulation.load(argv[1])) 
	{
		return -1;
	}
	uint32_t pixels[64 * 32];

	int count = 0;
	while(true)
	{
		emulation.fetch();
		emulation.execute();
		for (int i = 0; i < 64 * 32; i++) 
		{
			pixels[i] = (0x00FFFFFF * emulation.display[i]) | 0xFF000000;
			//std::cout << "Display val: " << emulation.display[i] << "\n";
			//std::cout << "Pixel val: " << std::hex << pixels[i] << "\n";
		}
		SDL_UpdateTexture(texture, NULL, pixels, 64 * sizeof(uint32_t));
		SDL_RenderClear(renderer);
		SDL_RenderTexture(renderer, texture, NULL, NULL);
		SDL_RenderPresent(renderer);
		//++count;
	}
	
	return 0;
}