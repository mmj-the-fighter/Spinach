#include "Game.h"

Game::Game()
{
	showPicDelay = 0;
	spriteManager = std::make_unique<JigsawSpriteManager>();
	gameWonImage.CreateFromPng("../examples/res_for_examples/congrats.png");
	gameWonImageX = GAME_RESOLUTION_X / 2 - gameWonImage.GetCanvas()->GetWidth() / 2;
	gameWonImageY = GAME_RESOLUTION_Y / 2 - gameWonImage.GetCanvas()->GetHeight() / 2;
}

void Game::Init(spn::Canvas* canvas)
{
	SetGameState(SHOW_PIC);
	spriteManager->SetCanvas(canvas);
	spriteManager->SetGameWorldLimits(0, 0, GAME_RESOLUTION_X, GAME_RESOLUTION_Y);
	spriteManager->Load("../examples/res_for_examples/road.png", 3, 4);
}

void Game::HandleInput(const SDL_Event* e)
{
    int button_index;
    float x, y;
    if (e->type == SDL_EVENT_KEY_DOWN) {
        switch (e->key.scancode)
        {
        case SDL_SCANCODE_SPACE:
        case SDL_SCANCODE_F5:
            Restart();
            break;
        }
    }
    else {
        switch (e->type)
        {
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
        case SDL_EVENT_MOUSE_BUTTON_UP:
            button_index = e->button.button;
            x = e->button.x;
            y = e->button.y;
            if (e->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
                if (button_index == 1)
                    spriteManager->OnLeftMouseButtonDown(x, y);
                else if (button_index == 3)
                    spriteManager->dbg_PrintLayers();
            } else {
                if (button_index == 1)
					spriteManager->OnLeftMouseButtonUp(x, y);
            }
            break;
        case SDL_EVENT_MOUSE_MOTION:
            x = e->motion.x;
            y = e->motion.y;
            spriteManager->OnMouseMoved(x, y);
            break;
        }
    }
}

void Game::UpdateAndRender(spn::Canvas* canvas)
{
	canvas->Clear();
	Update();
	Display(canvas);
}

void Game::Restart()
{
	//TODO:
	spriteManager->BreakConnections();
	spriteManager->RandomizePositions();
	spriteManager->RandomizeLayers();
	SetGameState(SHOW_PIC);
}


void Game::SetGameState(GameState state)
{
	if (state == SHOW_PIC) {
		startTime = clock();
		showPicDelay = 4.0;
	}
	gameState = state;
}

void Game::Update()
{
	switch (gameState)
	{
	case SHOW_PIC:
		endTime = clock();
		if ((static_cast<double>(endTime - startTime) / CLOCKS_PER_SEC)
			> showPicDelay){
			SetGameState(SHOW_GRID);
		}
		break;
	case SHOW_GRID:
		if (spriteManager->IsAllSpritesAreInSameLayer()){
			SetGameState(SHOW_GAME_WON_MESSAGE);
		}
		break;
	}

}


void Game::Display(spn::Canvas* canvas)
{
	switch (gameState)
	{
	case SHOW_PIC:
		spriteManager->DisplaySourceImage();
		break;
	case SHOW_GRID:
		spriteManager->Display();
		break;
	case SHOW_GAME_WON_MESSAGE:
		canvas->DrawImage(&gameWonImage, gameWonImageX, gameWonImageY);
		break;
	}
	
}