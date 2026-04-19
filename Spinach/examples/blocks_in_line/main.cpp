#include <iostream>
#include <spn_canvas.h>
#include <spn_core.h>
#include <spn_profiler.h>
#include "game.h"

BlocksInLineGame blocksGame;

void HandleInput(const SDL_Event* sdlEvent) {
	static bool debg = false;
	switch (sdlEvent->type) {
	case SDL_EVENT_KEY_DOWN:
		switch (sdlEvent->key.key) {
		case SDLK_P:
			blocksGame.OnPause();
			break;
		case SDLK_R:
			blocksGame.OnRestart();
			break;
		case SDLK_LEFT:
			blocksGame.OnMoveLeft();
			break;
		case SDLK_RIGHT:
			blocksGame.OnMoveRight();
			break;
		case SDLK_UP:
		{
			SDL_Keymod mod = SDL_GetModState();
			if (mod & SDL_KMOD_SHIFT) {
				blocksGame.OnRotateCW();
			}
			else {
				blocksGame.OnRotateCCW();
			}
		}
		break;
		case SDLK_DOWN:
			blocksGame.OnFallFast();
			break;
		case SDLK_BACKSPACE:
			//blocksGame.OnUndo();
			break;
		case SDLK_SPACE:
			//debg = !debg;
			blocksGame.SetDebugEnabled(debg);
			//blocksGame.ClearBoard();
			break;
		}
		break;
	}
}


int main() {
	
	spn::SpinachCore sc(800, 600, "../res/");
	if (sc.IsInitFailed()) {
		std::cout << "initialization failed with error "
			<< sc.GetInitializationResult()
			<< std::endl;
		return 1;
	}
	blocksGame.Init(&sc);
	sc.SetUpdateAndRenderHandler(
		std::bind(&BlocksInLineGame::UpdateAndRender,
			&blocksGame,
			std::placeholders::_1));

	sc.SetInputHandler(HandleInput);
	sc.MainLoop();
	return 0;
}