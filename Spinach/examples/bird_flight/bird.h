#ifndef _BIRD_H_
#define _BIRD_H_

#include <spn_canvas.h>
#include <spn_core.h>
#include <spn_image.h>

#define MAX_JUMP_DELAY 0.15f
#define MAX_JUMP_ACCELERATION 17.5f
#define GRAVITY 10.0f

class Bird
{
public:
	Bird();
	~Bird();
	void Move(spn::Canvas* canvas);
	void Display(spn::Canvas* canvas);
	void Init(int worldWt, int worldHt);
	void Reset();
	void OnLeftMouseButtonDown();
	bool IsOutOfScreen();
	void CreateMask();
	void DrawCollider(spn::Canvas* canvas);

private:
	void OnJumpStarted();
	void OnJumpEnded();
	bool jumping;
	float jumpTime;
	float jumpAccelY;
	float velY;
	float gravityY;
	
public:
	int ww, wh;
	int x;
	int y;
	int width;
	int height;
	spn::Image image;
	int chromaR;
	int chromaG;
	int chromaB;
	unsigned char *mask;
	bool hasMask;
};

#endif

