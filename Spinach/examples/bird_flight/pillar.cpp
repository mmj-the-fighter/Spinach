#include <algorithm>
#include <spn_rng.h>
#include "bird_flight_game.h"
#include <spn_collision_utils.h>
#include "pillar.h"



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

void Pillar::SetMasks(unsigned char* topMask, unsigned char* bottomMask) {
	topMaskRef = topMask;
	bottomMaskRef = bottomMask;
}

void Pillar::Display(spn::Canvas* canvas)
{
	canvas->DrawImageChromaKeyed(pillarTopImage, pillarTopX, pillarTopY, chromaR, chromaG, chromaB);
	canvas->DrawImageChromaKeyed(pillarBottomImage, pillarBottomX, pillarBottomY, chromaR, chromaG, chromaB);
}

void Pillar::DrawCollider(spn::Canvas* canvas) {
	canvas->DrawRectangle(
		pillarBottomX,
		pillarBottomY,
		pillarBottomX + pillarBottomImageWidth,
		wh-1
	);

	canvas->DrawRectangle(
		pillarTopX, 
		0, 
		pillarTopX + pillarTopImageWidth, 
		pillarTopY + pillarTopImageHeight
	);
	
}

void Pillar::CheckCollisionWithBird(CollisionState* collisionState)
{
	bool isCollidedWithTopPillar = spn::CheckCollisionPixelLevel(
		pillarTopX,
		pillarTopY,
		pillarTopImageWidth,
		pillarTopImageHeight,
		topMaskRef,
		bird->x,
		bird->y,
		bird->width,
		bird->height,
		bird->mask
	);

	if (isCollidedWithTopPillar) {
		collisionState->location = HITPILLAR;
		return;
	}

	bool isCollidedWithBottomPillar = spn::CheckCollisionPixelLevel(
		pillarBottomX,
		pillarBottomY,
		pillarBottomImageWidth,
		pillarBottomImageHeight,
		bottomMaskRef,
		bird->x,
		bird->y,
		bird->width,
		bird->height,
		bird->mask
	);

	if (isCollidedWithBottomPillar) {
		collisionState->location = HITPILLAR;
		return;
	}

	bool isCollidedWithPassage = spn::CheckCollision(pillarTopX, 0,
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