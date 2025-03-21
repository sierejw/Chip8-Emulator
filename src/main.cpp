#include "Chip8.h"
#include "SDL3/SDL.h"
#include <iterator>
#include <chrono>
#include <thread>

int main(int argc, char* argv[])
{
	if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO))
	{
		std::cout << "Failed to initialize SDL" << "\n";
		return -1;
	}

	constexpr std::array<uint8_t, 16> keyScanCodes
	{
		SDL_SCANCODE_X, //0
		SDL_SCANCODE_1, //1
		SDL_SCANCODE_2, //2
		SDL_SCANCODE_3, //3
		SDL_SCANCODE_Q, //4
		SDL_SCANCODE_W, //5
		SDL_SCANCODE_E, //6
		SDL_SCANCODE_A, //7
		SDL_SCANCODE_S, //8
		SDL_SCANCODE_D, //9
		SDL_SCANCODE_Z, //A
		SDL_SCANCODE_C, //B
		SDL_SCANCODE_4, //C
		SDL_SCANCODE_R, //D
		SDL_SCANCODE_F, //E
		SDL_SCANCODE_V  //F
	};

	uint8_t pressedKeyIndex{ 0 };
	constexpr int width{ 640 };
	constexpr int height{ 320 };
	constexpr int instuctPerFrame{ 11 };
	constexpr int channels{ 1 };
	constexpr int frequency{ 48000 };
	constexpr int fps{ 60 };
	constexpr float tone{ 400 };
	float audioPhase{};
	bool quit{ true };

	SDL_Window *window{ SDL_CreateWindow("Chip8 Emulator", width, height, 0) };
	SDL_Renderer *renderer{ SDL_CreateRenderer(window, NULL) };
	SDL_Texture *texture{ SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888, SDL_TEXTUREACCESS_STREAMING, 64, 32) };
	SDL_AudioSpec audiospec{ SDL_AUDIO_S16LE, channels, frequency };
	SDL_AudioStream* audio{ SDL_OpenAudioDeviceStream(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &audiospec, nullptr, nullptr) };
	
	Chip8 emulation;


	if (!emulation.load(argv[1]))
	{
		std::cout << "Error loading ROM" << "\n";
		return -1;
	}

	for (int i = 2; i < argc; i++)
	{
		if (!std::strcmp(argv[i], "-b"))
		{
			emulation.bitshiftFlag = true;
			
		}

		if (!std::strcmp(argv[i], "-o"))
		{
			emulation.offsetFlag = true;
		}

		if (!std::strcmp(argv[i], "-m"))
		{
			emulation.memoryFlag = true;
		}

		if (!std::strcmp(argv[i], "-v"))
		{
			emulation.vInterruptFlag = true;
		}
	}

	

	while (quit)
	{
		// Must execute a few operations before rendering to screen otherwise will cause slow performance
		for (int i = 0; i < instuctPerFrame; i++) 
		{
			if (emulation.draw && emulation.vInterruptFlag)
			{
				break;
			}

			emulation.fetch();
			emulation.execute();
		}

		emulation.updateTimers();
		std::this_thread::sleep_for(std::chrono::milliseconds(16));
		
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{

			switch (event.type)
			{
			case SDL_EVENT_QUIT:

				quit = false;
				break;
			
			case SDL_EVENT_KEY_DOWN:
			{
				if (event.key.scancode == SDL_SCANCODE_ESCAPE)
				{
					quit = false;
					break;
				}

				auto scancode{ std::find(keyScanCodes.begin(), keyScanCodes.end(), event.key.scancode) };

				if (scancode != keyScanCodes.end())
				{
					pressedKeyIndex = static_cast<uint8_t>(std::distance(keyScanCodes.begin(), scancode));
					emulation.keys[pressedKeyIndex] = 1;
				}


				break;
			}

			case SDL_EVENT_KEY_UP:
			{
				auto scancode{ std::find(keyScanCodes.begin(), keyScanCodes.end(), event.key.scancode) };

				if (scancode != keyScanCodes.end())
				{
					pressedKeyIndex = static_cast<uint8_t>(std::distance(keyScanCodes.begin(), scancode));
					emulation.keys[pressedKeyIndex] = 0;
				}
			}
			}
		}


		if (emulation.draw)
		{
			uint32_t pixels[64 * 32];

			for (int i = 0; i < 64 * 32; i++)
			{
				pixels[i] = (0x00FFFFFF * emulation.display[i]) | 0xFF000000;
			}

			SDL_UpdateTexture(texture, NULL, pixels, 64 * sizeof(uint32_t));
			SDL_RenderClear(renderer);
			SDL_RenderTexture(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
			emulation.draw = false;
		}

		//Generating square wave to put into vector for audio
		std::vector<int16_t> sampleBuffer(static_cast<unsigned int>((frequency / channels) / fps));


		if (emulation.playSound()) 
		{
			for (auto& sample : sampleBuffer)
			{
				sample = static_cast<int16_t>(audioPhase > 0.5f ? 4096 : -4096);
				audioPhase = std::fmod(audioPhase + (tone / frequency), 1.0f);
			}
			SDL_ResumeAudioStreamDevice(audio);
		}
		else 
		{
			audioPhase = 0.0f;
			SDL_PauseAudioStreamDevice(audio);
		}

		if (!SDL_AudioStreamDevicePaused(audio) && sampleBuffer.size())
		{
			SDL_PutAudioStreamData(audio, &sampleBuffer, static_cast<int32_t>(sampleBuffer.size() * sizeof(int16_t)));
		}
	}

	SDL_DestroyAudioStream(audio);
	SDL_DestroyTexture(texture);
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	SDL_Quit();
	
	return 0;
}