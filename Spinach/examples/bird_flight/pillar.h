#ifndef _PILLAR_H_
#define _PILLAR_H_

#include <spn_canvas.h>
#include <spn_core.h>
#include "bird.h"
#include "collision_state.h"

#define MAX_PILLAR_VELOCITY 140.0f

class Pillar
{
public:
	Pillar(int worldWt, int worldHt);
	void Move(spn::Canvas* canvas);	
	void MakePillar();
	void Display(spn::Canvas* canvas);
	bool IsPillarOutOfScreen();
	bool IsBirdNear();
	

	void SetBird(Bird* b);
	void SetChromaKey(int red, int green, int blue);
	void SetPillarImages(spn::Image* topImage, spn::Image* bottomImage);
	void CheckCollisionWithBird(CollisionState *collisionState);

	spn::Image *pillarTopImage;
	spn::Image *pillarBottomImage;
	
	int randomY;
	int birdHeight;
	float pillarTopX;
	float pillarTopY;
	float pillarBottomX;
	float pillarBottomY;
	float velX;
	int pillarTopImageWidth;
	int pillarTopImageHeight;
	int pillarBottomImageWidth;
	int pillarBottomImageHeight;
	int pillarPassageId;
	int chromaR;
	int chromaG;
	int chromaB;
	int ww;
	int wh;
	Bird *bird;
};



#endif