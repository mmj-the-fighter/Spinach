#ifndef _PILLARMANAGER_H_
#define _PILLARMANAGER_H_

#include <spn_canvas.h>
#include "bird.h"

#include <vector>
#include "pillar.h"
#include "collision_state.h"

class PillarManager
{
public:
	PillarManager(int worldWt, int worldHt);
	~PillarManager();
	void Init(Bird* b);
	void Reset();
	void Move(spn::Canvas* canvas);
	void Display(spn::Canvas* canvas);
	void CheckCollisionWithBird(CollisionState* collisionState);
private:
	void GeneratePillars();
	Bird *bird;
	int birdHeight;
	int birdWidth;
	spn::Image pillarTopImage;
	spn::Image pillarBottomImage;
	std::vector<Pillar*> pillars;
	int leftMostPillarIndex;
	int rightMostPillarIndex;
	int chromaR;
	int chromaG;
	int chromaB;
	int ww;
	int wh;
};

#endif
