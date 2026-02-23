#ifndef GAME_H
#define GAME_H

#include <ctime>
#include <memory>
#include <spn_canvas.h>
#include <spn_core.h>
#include <spn_image.h>
#include "jigsaw_sprite_manager.h"


#define GAME_RESOLUTION_X 800
#define GAME_RESOLUTION_Y 600

typedef enum GameState { SHOW_PIC, SHOW_GRID, SHOW_GAME_WON_MESSAGE};

class Game
{
public:
	Game();
	void Init(spn::Canvas* canvas);
	void Restart();
	void HandleInput(const SDL_Event* event);
	void UpdateAndRender(spn::Canvas* canvas);
	void Update();
	void Display(spn::Canvas* canvas);
	void SetGameState(GameState state);
public:
	GameState gameState;
	double showPicDelay;
	spn::Image gameWonImage;
	int gameWonImageX;
	int gameWonImageY;
	std::unique_ptr<JigsawSpriteManager> spriteManager;
	clock_t startTime, endTime;
};

#endif
