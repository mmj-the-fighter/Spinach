#ifndef COLLISION_UTILS
#define COLLISION_UTILS
#include <spn_image.h>

namespace spn
{
	bool CheckCollision(
		int left1, int top1, int right1, int bottom1,
		int left2, int top2, int right2, int bottom2
	);

	unsigned char* CreateCollisionMask(
		spn::Image* srcImage,
		unsigned int redChroma,
		unsigned int greenChroma,
		unsigned int blueChroma,
		bool* flag
	);

	bool CheckCollisionPixelLevel(
		int ax,
		int ay,
		int maskAWidth,
		int maskAHeight,
		unsigned char* maskA,
		int bx,
		int by,
		int maskBWidth,
		int maskBHeight,
		unsigned char* maskB
	);	
}

#endif