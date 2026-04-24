#include <iostream>
#include <algorithm>
#include <string>
#include <memory>
#include <functional>
#include <spn_canvas.h>
#include <spn_core.h>
#include "game.h"

int main(int argc, char* argv[])
{
	spn::SpinachCore sc;
	if (!sc.Init(GAME_RESOLUTION_X, GAME_RESOLUTION_Y, "../res/")) {
		std::cout << "initialization failed with error "
			<< sc.GetInitializationResult()
			<< std::endl;
		return 1;
	}
	std::unique_ptr<Game> game = std::make_unique<Game>();
	game->Init(sc.GetCanvas());
	sc.SetUpdateAndRenderHandler(
		std::bind(&Game::UpdateAndRender,
			game.get(),
			std::placeholders::_1));

	sc.SetInputHandler(
		std::bind(&Game::HandleInput,
			game.get(),
			std::placeholders::_1));	
	sc.SetWindowTitle("Picture Puzzle");
	sc.GetCanvas()->SetPrimaryColor(255, 255, 0);
	sc.GetCanvas()->SetClearColor(0, 0, 0);
	sc.SetTargetFramesPerSecond(30);
	sc.LockFps(true);
	sc.MainLoop();
	return 0;
}