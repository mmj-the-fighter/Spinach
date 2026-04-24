#include <iostream>
#include <spn_canvas.h>
#include <spn_core.h>
#include <game.h>

int main(int argc, char* argv[])
{
	Game game;
	spn::SpinachCore sc;
	if (!sc.Init(640, 480, "../res/")) {
		std::cout << "initialization failed with error "
			<< sc.GetInitializationResult()
			<< std::endl;
		return 1;
	}
	sc.SetWindowTitle("Example Game for Spinach");
	game.Init(&sc);
	sc.SetUpdateAndRenderHandler(
		std::bind(&Game::UpdateAndRender,
			&game,
			std::placeholders::_1));

	sc.SetInputHandler(
		std::bind(&Game::HandleInput,
			&game,
			std::placeholders::_1));
	sc.MainLoop();
	return 0;
}
