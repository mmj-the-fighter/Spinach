#include <iostream>
#include "bird_flight_game.h"

Game::Game() {

}

void Game::Init(spn::SpinachCore* sc) 
{
	sc->GetCanvas()->SetPrimaryColor(255, 255, 255);
	sc->GetCanvas()->SetClearColor(0, 0, 128);
	ww = sc->GetCanvas()->GetWidth();
	wh = sc->GetCanvas()->GetHeight();
	bird = new Bird;
	bird->Init(ww, wh);
	pillarManager = new PillarManager(ww, wh);
	pillarManager->Init(bird);
	score = 0;
	highScore = 0;
	lastHitPassageId = -1;
}

void Game::Restart() 
{
	score = 0;
	lastHitPassageId = -1;
	pillarManager->Reset();
	bird->Reset();
}

void Game::UpdateAndRender(spn::Canvas* canvas)
{
	char scoreBuffer[256];
	sprintf(scoreBuffer, "Score: %d", score);

	if (gameOver) {
		canvas->Clear();
		canvas->SetPrimaryColor(0, 0, 0);
		canvas->DrawFilledRectangle(10,10,250,150);
		canvas->SetPrimaryColor(255, 255, 255);
		canvas->DrawCString(scoreBuffer, 50, 50);
		if (score > highScore) {
			highScore = score;
		}
		char highScoreBuffer[256];
		sprintf(highScoreBuffer, "High Score: %d", highScore);
		canvas->DrawCString(highScoreBuffer, 50, 80);
	}

	if (bird->IsOutOfScreen()) {
		gameOver = true;
		return;
	}
	
	pillarManager->CheckCollisionWithBird(&collisionState);

	switch (collisionState.location)
	{
		case HITPASSAGE:
			if (lastHitPassageId != collisionState.pillarPassageId) {
				++score;
				lastHitPassageId = collisionState.pillarPassageId;
			}
			//std::cout << "bird hit passage\n";
		case HITNOTHING:
			bird->Move(canvas);
			pillarManager->Move(canvas);
			//std::cout << "bird hit nothing\n";
			break;
		case HITPILLAR:
		//	std::cout << "bird hit pillar\n";
			gameOver = true;
			return;
			break;
		default:
			break;
	}


	canvas->Clear();
	pillarManager->Display(canvas);
	bird->Display(canvas);
	canvas->SetPrimaryColor(255, 255, 255);
	canvas->DrawCString(scoreBuffer, 50, 50);

}

void Game::HandleInput(const SDL_Event* sdlEvent) 
{
	if (sdlEvent->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
		if (sdlEvent->button.button == SDL_BUTTON_LEFT) {
			OnLmbDown();
		}
		else if (sdlEvent->button.button == SDL_BUTTON_RIGHT) {
			OnRmbDown();
		}
	}
}

void Game::OnLmbDown() {
	//std::cout << "lmb down\n";
	bird->OnLeftMouseButtonDown();
}

void Game::OnRmbDown() {
	Restart();
	//std::cout << "rmb down\n";
}

