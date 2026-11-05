#ifndef SPN_SPRITE_H
#define SPN_SPRITE_H
#include <unordered_map>
#include "spn_geom.h"

namespace spn
{
	struct AnimRange {
		int min;
		int max;
	};

	class Image;
	class Canvas;

	class Sprite
	{
	public:
		Sprite(
			Image* spriteImage, int rows, int cols,
			bool isAnimated, float animDelay = 0);

		void CalculateCurFramePosition();
		void SetFrame(int n);
		void SetPosition(int ax, int ay);
		void EnableAnimation(bool flag);
		void Update(float dt);
		void Display(Canvas* canvas);
		bool AddAnim(int animId, int startFrame, int endFrame);
		void SetAnim(int animId);

	private:
		void AdvanceFrame();
		Image* image; //sprite sheet image 
		int mxr; //maximum number of rows
		int mxc; //maximum number of columns
		int w; //width of the sprite sheet
		int h; //height of the sprite sheet
		int fw; //frame width
		int fh; //frame height
		int frmPosx; //frame stating position x in sprite sheet
		int frmPosy; //frame starting position y  in sprite sheet 

		int mxf; //maximum number of frames
		int cfm; //current frame number


		float delay; //animation delay
		float elpTime; //elapsed time
		bool isAnimationEnabled; //for making the sprite static or dynamic
		spn::Vec2d pos; //sprite postion in the screen


		AnimRange curAnim; //cur animation range
		std::unordered_map<int, AnimRange> animMap; //ds to store animations with id and range
		bool isCurAnimSet = false; //varible storing clue that atleast one animation is there
	};
}

#endif
