#include <iostream>
#include <ctime>
#include "game.h"


void Game::HandleInput(const SDL_Event* sdlEvent) {

	switch (sdlEvent->type) {
		case SDL_EVENT_KEY_DOWN:
			switch (sdlEvent->key.key) {
				case SDLK_F5:
					Restart();
					break;
				case SDLK_LEFT:
					//std::cout << "Left\n";
					snake->ChangeHeadDirection(MoveDirection::LEFT);
					break;
				case SDLK_RIGHT:
					snake->ChangeHeadDirection(MoveDirection::RIGHT);
					//std::cout << "Right\n";
					break;
				case SDLK_UP:
					snake->ChangeHeadDirection(MoveDirection::UP);
					//std::cout << "Up\n";
					break;
				case SDLK_DOWN:
					snake->ChangeHeadDirection(MoveDirection::DOWN);
					//std::cout << "Down\n";
					break;
			}
		break;
	}
}

void Game::Restart() {
	fruit.Set(400, 300, 10, 10, ww, wh,
		MoveDirection::RIGHT);
	isGameOver = false;
	desiredFps = 5;
	sc->SetTargetFramesPerSecond(desiredFps);
	snake->Restart();
	score = 0;
	UpdateScoreStr();
}

void Game::UpdateAndRender(spn::Canvas* canvas) {
	//Update
	if (isGameOver) {
		return;
	}
	snake->Update();
	if (snake->CheckCollisionWithSelf()) {
		isGameOver = true;
		UpdateScoreStr();
	}
	if (snake->CheckCollisionWithFruit(fruit)) {
		int availWidth = (fruit.ww - 1 - (fruit.w));
		int availHeight = (fruit.wh - 1 - (fruit.h));
		int randX = rand() % availWidth;
		int randY = rand() % availHeight;
		fruit.SetPos(randX,randY);
		snake->Grow();
		++desiredFps;
		if (desiredFps > 60) {
			desiredFps = 60;
		}
		sc->SetTargetFramesPerSecond(desiredFps);
		++score;
		UpdateScoreStr();
	}
	//Render
	canvas->Clear();
	canvas->SetPrimaryColor(128, 128, 0);
	fruit.Render(canvas);
	canvas->SetPrimaryColor(255, 255, 255);
	snake->Render(canvas);
	canvas->SetPrimaryColor(0, 255, 0);
	canvas->DrawString(scoreStr, 10, 10);
}

void Game::UpdateScoreStr() {
	char buffer[256];
	if (!isGameOver) {
		sprintf(buffer, "Score: %d", score);
		scoreStr = std::string(buffer);
	}
	else {
		sprintf(buffer, "Game Over! Score: %d", score);
		scoreStr = std::string(buffer);
	}
}

void Game::Init(spn::SpinachCore* core) {
	sc = core;
	desiredFps = 5;
	srand(static_cast<unsigned int>(time(nullptr)));
	spn::Canvas* canvas = core->GetCanvas();
	canvas->SetClearColor(0, 0, 128);
	
	if(nullptr == snake) {
		snake = new Snake(core);
	}
	ww = core->GetCanvas()->GetWidth();
	wh = core->GetCanvas()->GetHeight();
	Restart();
}
