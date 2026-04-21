#include "jigsaw_sprite.h"

JigsawSprite::JigsawSprite() {
	leftNeighbourId = -1;
	rightNeighbourId = -1;
	upNeighbourId = -1;
	downNeighbourId = -1;
	hasFoundLeftNeighbour = false;
	hasFoundRightNeighbour = false;
	hasFoundUpNeighbour = false;
	hasFoundDownNeighbour = false;
}

void JigsawSprite::DisconnectWithNeighbours() {
	hasFoundLeftNeighbour = false;
	hasFoundRightNeighbour = false;
	hasFoundUpNeighbour = false;
	hasFoundDownNeighbour = false;
}

void JigsawSprite::Set(int aid, int arow, 
	int acol, int alayer, 
	int ax, int ay, 
	int awidth, int aheight, 
	spn::Rect& rect
) {
	id = aid;
	row = arow;
	col = acol;
	layer = alayer;
	x = ax;
	y = ay;
	width = awidth;
	height = aheight;
	imgSubRect = rect;
}

bool JigsawSprite::IsMouseOver(int mx, int my) {
	return (
		(mx > x) && mx < (x + width) &&
		(my > y) && my < (y + height)
		);
}

void JigsawSprite::RegisterSelection(int mx, int my) {
	isSelected = true;
}

void JigsawSprite::UnRegisterSelection() {
	isSelected = false;
}

void JigsawSprite::ApplyDrag(int dx, int dy) {
	if (isSelected) {
		x += dx;
		y += dy;
	}
}

void JigsawSprite::Display(spn::Canvas* canvas, spn::Image* fullImage) {
	canvas->DrawSubImage(fullImage, x, y, imgSubRect);
}
