#ifndef JIGSAW_SPRITE_H
#define JIGSAW_SPRITE_H
#include <memory>
#include <spn_image.h>
#include <spn_canvas.h>
#include <spn_utils.h>


class JigsawSprite
{
public:
	int id;
	int row;
	int col;
	int layer;
	int x;
	int y;
	int width;
	int height;
	bool isSelected;
	spn::Rect imgSubRect;
	int leftNeighbourId;
	int rightNeighbourId;
	int upNeighbourId;
	int downNeighbourId;
	bool hasFoundLeftNeighbour;
	bool hasFoundRightNeighbour;
	bool hasFoundUpNeighbour;
	bool hasFoundDownNeighbour;
	bool isVisited;
public:
	JigsawSprite();
	void DisconnectWithNeighbours();
	void Set(int aid, int arow, int acol, int alayer, int ax, int ay, int awidth, int aheight, spn::Rect& imgSubRect);
	bool IsMouseOver(int mx, int my);
	void RegisterSelection(int mx, int my);
	void UnRegisterSelection();
	void ApplyDrag(int dx, int dy);
	void Display(spn::Canvas* canvas, spn::Image* fullImage);
};

#endif