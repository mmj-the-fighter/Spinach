#ifndef _SNAKE_H_
#define _SNAKE_H_
#include <vector>
#include <spn_canvas.h>
#include <spn_core.h>
#include "segment.h"

class Snake
{
public:
	Snake(spn::SpinachCore* psc);
	void Init();
	void Restart();
	void Update();
	void Render(spn::Canvas* canvas);
	void Grow();
	void ChangeHeadDirection(MoveDirection md);
	bool CheckCollisionWithFruit(Segment& fruit);
	bool CheckCollisionWithSelf();
private:
	std::vector<Segment> segments;
	spn::SpinachCore* sc;
};
#endif