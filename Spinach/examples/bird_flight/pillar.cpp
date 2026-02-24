#include <algorithm>
#include <spn_rng.h>
#include "bird_flight_game.h"
#include "pillar.h"
#include "collision_utils.hpp"


Pillar::Pillar(int worldWt, int worldHt):
pillarTopX(worldWt), 
pillarBottomX(worldWt),
velX(MAX_PILLAR_VELOCITY),
randomY(0)
{
	ww = worldWt;
	wh = worldHt;
}

void Pillar::SetChromaKey(int red, int green, int blue)
{
	chromaR = red;
	chromaG = green;
	chromaB = blue;
}

void Pillar::SetBird(Bird *b){
	bird = b;
	birdHeight = b->height;
}

void Pillar::SetPillarImages(spn::Image* topImage, spn::Image* bottomImage) {
	pillarTopImage = topImage;
	pillarTopImageWidth = topImage->GetCanvas()->GetWidth();
	pillarTopImageHeight = topImage->GetCanvas()->GetHeight();

	pillarBottomImage = bottomImage;
	pillarBottomImageWidth = bottomImage->GetCanvas()->GetWidth();
	pillarBottomImageHeight = bottomImage->GetCanvas()->GetHeight();
}


void Pillar::MakePillar()
{
	randomY = spn::RandomGen::GetInstance().GenerateInRange(0, wh);
	randomY = std::min(600 - birdHeight * 4, randomY);
	pillarTopY = randomY - pillarTopImageHeight;
	pillarBottomY = randomY + birdHeight * 3;
	pillarTopX = ww;
	pillarBottomX = ww;
}

void Pillar::Move(spn::Canvas *canvas)
{
	if (pillarTopX + pillarBottomImageWidth >= 0) {
		pillarTopX -= velX * canvas->GetLastFrameTime();
		pillarBottomX -= velX * canvas->GetLastFrameTime();
	}
}

bool Pillar::IsPillarOutOfScreen()
{
	return (pillarTopX + pillarBottomImageWidth < 0);
}

bool Pillar::IsBirdNear()
{
	int dx = pillarTopX - bird->x;
	return abs(dx) <= (pillarTopImageWidth + bird->width);
}

void Pillar::Display(spn::Canvas* canvas)
{
	canvas->DrawImageChromaKeyed(pillarTopImage, pillarTopX, pillarTopY, chromaR, chromaG, chromaB);
	canvas->DrawImageChromaKeyed(pillarBottomImage, pillarBottomX, pillarBottomY, chromaR, chromaG, chromaB);
}

void Pillar::CheckCollisionWithBird(CollisionState* collisionState)
{
	bool isCollidedWithTopPillar = CheckCollision(pillarTopX, pillarTopY,
		pillarTopX + pillarTopImageWidth, pillarTopY + pillarTopImageHeight,
		bird->x, bird->y,
		bird->x + bird->width, bird->y + bird->height
	);

	if (isCollidedWithTopPillar) {
		collisionState->location = HITPILLAR;
		return;
	}

	bool isCollidedWithBottomPillar = CheckCollision(pillarBottomX, pillarBottomY,
		pillarBottomX + pillarBottomImageWidth, pillarBottomY + pillarBottomImageHeight,
		bird->x, bird->y,
		bird->x + bird->width, bird->y + bird->height
	);

	if (isCollidedWithBottomPillar) {
		collisionState->location = HITPILLAR;
		return;
	}

	bool isCollidedWithPassage = CheckCollision(pillarTopX, 0,
		pillarTopX + pillarTopImageWidth, wh,
		bird->x, bird->y,
		bird->x + bird->width, bird->y + bird->height
	);


	if (isCollidedWithPassage) {
		collisionState->location = HITPASSAGE;
		return;
	}

	collisionState->location = HITNOTHING;
	return;

}