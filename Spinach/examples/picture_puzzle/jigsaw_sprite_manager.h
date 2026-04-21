#ifndef JIGSAW_SPRITE_MANAGER_H
#define JIGSAW_SPRITE_MANAGER_H

#include <vector>
#include <memory>
#include <spn_image.h>
#include <spn_canvas.h>
#include <spn_utils.h>
#include "jigsaw_sprite.h"

struct ImageSliceMetaInfo {
	int row;
	int col;
	spn::Rect imageSliceRect;
};

class JigsawSpriteManager
{
private:
	int curSelectionId;
	std::vector<ImageSliceMetaInfo> slices;
	std::vector<JigsawSprite*> spriteRenderList;
	std::vector<JigsawSprite*> spriteList;
	std::unique_ptr<spn::Image> fullImage;
	spn::Canvas* canvas;
	int desiredCols;
	int desiredRows;
	int spriteCount;
	int prevClickX;
	int prevClickY;
	int curClickX;
	int curClickY;
	int curDragX;
	int curDragY;

	int gameWorldStartX;
	int gameWorldEndX;
	int gameWorldStartY;
	int gameWorldEndY;

	int fullImageStartX;
	int fullImageStartY;

public:
	JigsawSpriteManager();
	~JigsawSpriteManager();
	void Slice(int width, int height, int rows, int cols, int &xStep, int& yStep);
	void SetGameWorldLimits(int startX, int startY, int endX, int endY);
	void SetCanvas(spn::Canvas* cv) {
		canvas = cv;
	}
	void Load(const char* filename, int rows, int cols);
	void RandomizePositions();
	void RandomizeLayers();
	void BreakConnections();
	void HighlightTheCurrentSelection();
	void Display();
	void DisplaySourceImage();
	void OnLeftMouseButtonDown(int x, int y);
	void OnLeftMouseButtonUp(int x, int y);
	void OnMouseMoved(int x, int y);
	void UnLoad();
	void InitGraphTraversal();
	int GetAdjacentUnVisitedSpriteId(int spriteId);
	void LinkAdjacentSpritesForASingleSprite(int spriteId);
	void AttachAdjacentJigsawSetIfAny(int spriteId);
	void MoveAttachedJigsawSetIfAny(int spriteId, int dragX, int dragY);
	bool IsAllSpritesAreInSameLayer();
	void dbg_PrintLayers();
};



bool compareByLayer(const JigsawSprite* s1, const JigsawSprite* s2); 




#endif