/*This Chip 8 emulator was built using Austin Morlan's Guide.
https://austinmorlan.com/posts/chip8_emulator/
Most things remain the same however there are some major differences such as using a switch statement instead of a function pointer table
or the way the file is opened and the ROM is loaded as well as other things.
This emulator is done but not completed. There are lot of improvements that needs to be done such as the way you choose the rom file.
 Hopefully soon I will make some changes.
*/

#include "Platform.cpp"
#include "Chip8.hpp"
#include <chrono>
#include <iostream>

int main(int argc, char **argv)
{

	if (argc != 4)
	{
		std::cerr << "Usage: " << argv[0] << " <Scale> <Delay> <ROM>\n";
		std::exit(EXIT_FAILURE);
	}

	int videoScale = 10;
	int cycleDelay = 3;
	char const *romFilename = "Tetris.ch8";

	Platform platform("CHIP-8 Emulator", VIDEO_WIDTH * videoScale, VIDEO_HEIGHT * videoScale, VIDEO_WIDTH, VIDEO_HEIGHT);

	Chip8 chip8;
	chip8.loadRom(romFilename);

	int videoPitch = sizeof(chip8.video[0]) * VIDEO_WIDTH;

	auto lastCycleTime = std::chrono::high_resolution_clock::now();
	bool quit = false;

	while (!quit)
	{
		quit = platform.ProcessInput(chip8.keypad);

		auto currentTime = std::chrono::high_resolution_clock::now();
		float dt = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - lastCycleTime).count();

		if (dt > cycleDelay)
		{
			lastCycleTime = currentTime;

			chip8.cycle();

			platform.Update(chip8.video, videoPitch);
		}
	}

	return 0;
}
