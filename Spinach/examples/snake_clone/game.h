#ifndef _GAME_H_
#define _GAME_H_

#include <SDL3/SDL.h>
#include <spn_core.h>
#include <spn_canvas.h>
#include "snake.h"

class Game
{
public:
	Game() {
		snake = nullptr;
	}
	~Game() {
		if (nullptr != snake) {
			delete snake;
			snake = nullptr;
		}
	}
	void HandleInput(const SDL_Event* sdlEvent);
	void UpdateAndRender(spn::Canvas* canvas);
	void Init(spn::SpinachCore* canvas);
	void Restart();
	void UpdateScoreStr();
private:
	spn::SpinachCore* sc;
	Snake* snake;
	Segment fruit;
	bool isGameOver;
	int score;
	std::string scoreStr;
	int desiredFps;
	int w, h, ww, wh;
};


#endif