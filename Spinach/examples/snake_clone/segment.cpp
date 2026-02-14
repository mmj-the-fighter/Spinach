#include "segment.h"


Segment::Segment(int xpos, int ypos,
	int width, int height,
	int worldWidth, int worldHeight,
	MoveDirection direction
) : x(xpos), y(ypos), w(width), h(height), 
ww(worldWidth), wh(worldHeight), 
moveDir(direction){}

void Segment::Set(int xpos, int ypos,
	int width, int height,
	int worldWidth, int worldHeight,
	MoveDirection direction
) {
	x = xpos;
	y = ypos;
	w = width;
	h = height;
	ww = worldWidth;
	wh = worldHeight;
}

void Segment::SetPos(int xpos, int ypos) 
{
	x = xpos;
	y = ypos;
}

void Segment::Move() 
{
	switch (moveDir)
	{
	case MoveDirection::UP:
		y -= h;
		if (y < 0) {
			y = wh;
		}
		break;
	case MoveDirection::DOWN:
		y += h;
		y %= wh;
		break;
	case MoveDirection::RIGHT:
		x += w;
		x %= ww;
		break;
	case MoveDirection::LEFT:
		x -= w;
		if (x < 0) {
			x = ww;
		}
		break;
	}
}

void Segment::Render(spn::Canvas* canvas)
{
	canvas->DrawFilledRectangle(x, y, x + w, y + h);
}

bool Segment::CheckCollision(const Segment& s)
{
	return CheckCollision(
		s.x,
		s.y,
		s.x + s.w,
		s.y + s.h,
		x,
		y,
		x + w,
		y + h
	);
}
bool Segment::CheckCollision(
	int left1, int top1,
	int right1, int bottom1,
	int left2, int top2,
	int right2, int bottom2
)
{
	int  x1, x2;
	if (bottom2 < top1 || top2 > bottom1) {
		return false;
	}
	else
	{
		x1 = (left2 > left1) ? left2 : left1;
		x2 = (right2 < right1) ? right2 : right1;
		if (x1 > x2) {
			return false;
		}
		return true;
	}
}