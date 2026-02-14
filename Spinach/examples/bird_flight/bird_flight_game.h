#ifndef _GAME_H_
#define _GAME_H_

#include <SDL3/SDL.h>
#include <spn_canvas.h>
#include <spn_core.h>
#include <bird.h>
#include <pillar_manager.h>
#include <collision_state.h>

class Game
{
public:
	Game();
	~Game() {
		delete bird;
		delete pillarManager;
	}
	void Init(spn::SpinachCore* sc);
	void Restart();
	void UpdateAndRender(spn::Canvas* canvas);
	void HandleInput(const SDL_Event* sdlEvent);
private:
	void OnLmbDown();
	void OnRmbDown();
	bool gameOver;
	int ww, wh;
	Bird *bird;
	PillarManager* pillarManager;
	CollisionState collisionState;
	int lastHitPassageId;
	int score;
	int highScore;
};

#endif
