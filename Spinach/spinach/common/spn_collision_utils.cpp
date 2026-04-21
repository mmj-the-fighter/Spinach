#include <algorithm>
#include <spn_collision_utils.h>
#include <spn_canvas.h>

namespace spn
{
	
	bool CheckCollision(
		int left1, int top1, int right1, int bottom1,
		int left2, int top2, int right2, int bottom2
	)
	{
		int  x1, x2;
		if (bottom2 < top1 || top2 > bottom1) {
			return false;
		}
		else
		{
			x1 = (left2  > left1) ? left2 : left1;
			x2 = (right2 < right1) ? right2 : right1;
			if (x1 > x2) {
				return false;
			}
			return true;
		}
	}

	unsigned char* CreateCollisionMask(
		spn::Image* srcImage,
		unsigned int redChroma,
		unsigned int greenChroma,
		unsigned int blueChroma,
		bool *flag
	) {
		spn::Canvas* srcCanvas = srcImage->GetCanvas();
		if (srcCanvas == nullptr) {
			*flag = false;
			return nullptr;
		}

		unsigned char* srcPixels = srcCanvas->GetPixelBuffer();
		if (srcPixels == nullptr) {
			*flag = false;
			return nullptr;
		}
		
		const int numSrcPixels = srcCanvas->GetNumOfPixels();
		unsigned char* mask = new unsigned char[numSrcPixels];
		if (mask == nullptr) {
			*flag = false;
			return nullptr;
		}
	
		unsigned char* loc = srcPixels;
		unsigned char red = 0, blue = 0, green = 0;
		for (int i = 0; i < numSrcPixels; ++i) {
			blue = *loc;
			green = *(loc + 1);
			red = *(loc + 2);
			if (red == redChroma &&
				green == greenChroma &&
				blue == blueChroma
				)
			{
				*(mask + i) = 0;
			}
			else {
				*(mask + i) = 255;
			}
			loc += 4;
		}
		*flag = true;
		return mask;
	}


	bool CheckCollisionPixelLevel(
		int aLeft, 
		int aTop,
		int maskAWidth,
		int maskAHeight,
		unsigned char* maskA,
		int bLeft, 
		int bTop,
		int maskBWidth,
		int maskBHeight,
		unsigned char* maskB
	)
	{
		int aWidth = maskAWidth;
		int aHeight = maskAHeight;
		int a1x = aLeft;
		int a2x = aLeft + maskAWidth;
		int a1y = aTop;
		int a2y = aTop + maskAHeight;
	
		int bWidth = maskBWidth;
		int bHeight = maskBHeight;
		int b1x = bLeft;
		int b2x = bLeft + maskBWidth;
		int b1y = bTop;
		int b2y = bTop + maskBHeight;

		if (!CheckCollision(a1x, a1y, a2x, a2y, b1x, b1y, b2x, b2y)){
			return false;
		}
		
		/*intersect */
		int p1x = a1x;
		int p1y = a1y;
		int p2x = a2x;
		int p2y = a2y;

		int p3x = b1x;
		int p3y = b1y;
		int p4x = b2x;
		int p4y = b2y;

		int p5x = std::max(p1x, p3x);
		int p5y = std::max(p1y, p3y);

		int p6x = std::min(p2x, p4x);
		int p6y = std::min(p2y, p4y);
		
		/*clip */
		int asx = p5x - p1x;
		int asy = p5y - p1y;
		int atw = abs(p5x - p6x);
		int ath = abs(p5y - p6y);

		int bsx = p5x - p3x;
		int bsy = p5y - p3y;
		int btw = abs(p5x - p6x);
		int bth = abs(p5y - p6y);

		int dy, dx;
		int aoffset, boffset;
		unsigned char* aloc;
		unsigned char* bloc;

		for (dy = 0; dy < ath; ++dy)
		{
			for (dx = 0; dx < atw; ++dx)
			{
				aoffset = (aWidth * (asy + dy)) + (asx + dx);
				boffset = (bWidth * (bsy + dy)) + (bsx + dx);
				aloc = maskA + aoffset;
				bloc = maskB + boffset;
				if (*aloc != 0 && *bloc != 0) {
					return true;
				}
			}
		}
		return false;
	}
}