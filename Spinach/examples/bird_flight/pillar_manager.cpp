#include "pillar_manager.h"
#include "bird_flight_game.h"

#define MAX_PILLAR_COUNT 4
#define MAX_INTER_PILLAR_DISTANCE_MULTIPLIER 8

PillarManager::PillarManager(int worldWt, int worldHt):ww(worldWt), wh(worldHt)
{
};

PillarManager::~PillarManager(){
	for (auto& pillar : pillars) {
		delete pillar;
	}
}

void PillarManager::Init(Bird* b)
{
	bird = b;
	birdHeight = b->height;
	birdWidth = b->width;
	chromaR = 0;
	chromaG = 255;
	chromaB = 0;
	pillarTopImage.CreateFromPpmRaw("../examples/res_for_examples/pillar_top.ppm");
	pillarBottomImage.CreateFromPpmRaw("../examples/res_for_examples/pillar_bottom.ppm");
	GeneratePillars();
}


void PillarManager::GeneratePillars()
{
	for (int i = 0; i < MAX_PILLAR_COUNT; i++) {
		Pillar * pillar = new Pillar(ww, wh);
		pillar->pillarPassageId = i;
		pillar->SetPillarImages(&pillarTopImage, &pillarBottomImage);
		pillar->SetChromaKey(chromaR, chromaG, chromaB);
		pillar->SetBird(bird);
		pillar->MakePillar();
		pillar->velX = MAX_PILLAR_VELOCITY;
		pillar->pillarTopX = 
			pillar->pillarBottomX = 
			ww + 
			i * (MAX_INTER_PILLAR_DISTANCE_MULTIPLIER * birdWidth);
		pillars.push_back(pillar);
	}
	leftMostPillarIndex = 0;
	rightMostPillarIndex = MAX_PILLAR_COUNT - 1;
}


void PillarManager::Reset()
{
	int i = 0;
	for (auto& pillar : pillars) {
		pillar->MakePillar();
		pillar->pillarTopX = 
			pillar->pillarBottomX = 
			ww + 
			i * (MAX_INTER_PILLAR_DISTANCE_MULTIPLIER * birdWidth);
		++i;
	}
	leftMostPillarIndex = 0;
	rightMostPillarIndex = MAX_PILLAR_COUNT - 1;
}

void PillarManager::Move(spn::Canvas *canvas)
{
	Pillar *leftMostPillar = pillars[leftMostPillarIndex];
	if (leftMostPillar->IsPillarOutOfScreen()){
		leftMostPillar->MakePillar();
		leftMostPillar->pillarTopX = 
		leftMostPillar->pillarBottomX = 
		pillars[rightMostPillarIndex]->pillarTopX + 
		MAX_INTER_PILLAR_DISTANCE_MULTIPLIER * birdWidth;
		rightMostPillarIndex = leftMostPillarIndex;
		leftMostPillarIndex = (leftMostPillarIndex + 1) % MAX_PILLAR_COUNT;
	}
	
	for (auto& pillar : pillars) {
		pillar->Move(canvas);
	}
}

void PillarManager::Display(spn::Canvas* canvas)
{
	for (auto& pillar : pillars) {
		if (!pillar->IsPillarOutOfScreen()) {
			pillar->Display(canvas);
		}
	}
}

void PillarManager::CheckCollisionWithBird(CollisionState* collisionState) {
	collisionState->location = HITNOTHING;
	
	int count = 2;
	int index = leftMostPillarIndex;
	while (collisionState->location == HITNOTHING && --count >= 0) {
		Pillar* pillar = pillars[index];
		collisionState->pillarPassageId = index;
		if (pillar->IsBirdNear()) {
			pillar->CheckCollisionWithBird(collisionState);
		}
		index = (index + 1) % MAX_PILLAR_COUNT;
	}
}