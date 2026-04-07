#include <string>
#include <spn_collision_utils.h>
#include "bird.h"
#include "bird_flight_game.h"

Bird::Bird():mask(nullptr){
}

Bird::~Bird() {
	if (mask != nullptr) {
		delete[] mask;
		mask = nullptr;
	}
}

void Bird::OnJumpStarted()
{
	jumping = true;
	jumpTime = MAX_JUMP_DELAY;
	velY = 0;
	jumpAccelY = MAX_JUMP_ACCELERATION;
}

void Bird::OnJumpEnded()
{
	jumping = false;
	//velY = 0;
	jumpAccelY = 0;
}

void Bird::OnLeftMouseButtonDown()
{
	if (!jumping) {
		OnJumpStarted();
	}
}


void Bird::Move(spn::Canvas* canvas)
{
	if (jumping) {
		if (jumpTime <= 0.0f) {
			OnJumpEnded();
		}
		else {
			jumpTime -= canvas->GetLastFrameTime();
			velY -= (jumpAccelY)*canvas->GetLastFrameTime();
		}
	}
	else {
		velY += (gravityY)*canvas->GetLastFrameTime();
	}
	y += velY;
}

bool Bird::IsOutOfScreen()
{
	int top = (int)y;
	int bottom = top + height;
	return (top >= wh) || (bottom < 0);
}

void Bird::Display(spn::Canvas* canvas)
{
	canvas->DrawImageChromaKeyed(&image, x, y, chromaR, chromaG, chromaB);
	//DrawCollider(canvas, x, y);
}

void Bird::DrawCollider(spn::Canvas* canvas) 
{
	canvas->DrawRectangle(
		x, y,
		x + width,
		y + height);
}


void Bird::Init(int worldWt, int worldHt)
{
	x = worldWt / 3;
	y = worldHt / 2;
	ww = worldWt;
	wh = worldHt;
	velY = 0;
	gravityY = GRAVITY;
	jumpAccelY = 0;
	jumping = false;
	chromaR = 0;
	chromaG = 255;
	chromaB = 0;
	image.CreateFromPpmRaw("../examples/res_for_examples/bird.ppm");
	width = image.GetCanvas()->GetWidth();
	height = image.GetCanvas()->GetHeight();
	hasMask = false;
	mask = nullptr;
	CreateMask();
}

void Bird::Reset()
{
	x = ww / 3;
	y = wh / 2;
	velY = 0;
	gravityY = GRAVITY;
	jumpAccelY = 0;
	jumping = false;
}

void Bird::CreateMask() {
	mask = spn::CreateCollisionMask(&image, chromaR, chromaG, chromaB, &hasMask);
}
