#include "snake.h"

Snake::Snake(spn::SpinachCore* psc) {
	sc = psc;
	Init();
}

void Snake::Restart() {
	Init();
}

void Snake::Init() 
{
	segments.clear();
	for (int i = 1; i < 5; i++) {
		segments.push_back(Segment(sc->GetCanvas()->GetWidth()/2.0 - i * 10, 
			sc->GetCanvas()->GetHeight()/2, 
			10, 10,
			sc->GetCanvas()->GetWidth()-1, 
			sc->GetCanvas()->GetHeight()-1, 
			MoveDirection::RIGHT));
	}

}

void Snake::Update() 
{
	int segmentCount = segments.size();
	for (int i = segmentCount - 1; i >= 1; --i) {
		segments[i].Move();
		segments[i].moveDir = segments[i - 1].moveDir;
	}
	//move head
	segments[0].Move();

}

void Snake::Render(spn::Canvas* canvas)
{
	int segmentCount = segments.size();
	for (int i = segmentCount - 1; i >= 0; --i) {
		segments[i].Render(canvas);
	}
}

bool Snake::CheckCollisionWithFruit(Segment& fruit) {
	return fruit.CheckCollision(segments[0]);
}

bool Snake::CheckCollisionWithSelf() {
	Segment head = segments[0];
	int segmentCount = segments.size();
	for (int i = 3; i < segmentCount; ++i) {
		const Segment& body = segments[i];
		if (head.CheckCollision(body)) {
			return true;
		}
	}
	return false;
}

void Snake::Grow() 
{
	int x, y;
	MoveDirection dir = MoveDirection::UP;
	int vecx, vecy;
	int segmentCount = segments.size();
	const Segment& s = segments[segmentCount - 1];
	vecx = s.x - segments[segmentCount - 2].x;
	vecy = s.y - segments[segmentCount - 2].y;

	x = s.x;
	y = s.y;

	if (vecx == 0)
	{
		if (vecy > 0)
		{
			dir = MoveDirection::UP;
			y += s.h;
		}
		else if (vecy < 0)
		{
			dir = MoveDirection::DOWN;
			y -= s.h;
		}
	}
	else if (vecy == 0) {
		if (vecx > 0)
		{
			dir = MoveDirection::LEFT;
			x += s.w;
		}
		else if (vecx < 0)
		{
			dir = MoveDirection::RIGHT;
			x -= s.w;
		}
	}
	segments.push_back(Segment(x, y, 
		10, 10, 
		sc->GetCanvas()->GetWidth() - 1,
		sc->GetCanvas()->GetHeight() - 1, 
		dir));

}

void Snake::ChangeHeadDirection(MoveDirection d)
{
	switch (segments[0].moveDir) {
	case MoveDirection::LEFT:
		if (d == MoveDirection::LEFT || d == MoveDirection::RIGHT)
			return;
		else
			break;
	case MoveDirection::RIGHT:
		if (d == MoveDirection::LEFT || d == MoveDirection::RIGHT)
			return;
		else
			break;
	case MoveDirection::UP:
		if (d == MoveDirection::UP || d == MoveDirection::DOWN)
			return;
		else break;
	case MoveDirection::DOWN:
		if (d == MoveDirection::UP || d == MoveDirection::DOWN)
			return;
		else
			break;
	}

	segments[0].moveDir = d;
}
