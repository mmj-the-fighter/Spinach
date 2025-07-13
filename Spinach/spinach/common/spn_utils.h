#ifndef _SPN_UTILS_H_
#define _SPN_UTILS_H_
#include <string>
namespace spn
{
	struct Rect
	{
		int left;
		int top;
		int width;
		int height;
	};
	void FindRectToRectIntersection(const Rect& r1, const Rect& r2, Rect& ir);
	std::string GetTimeBasedScreenShotFileName();

	void FindRectToRectUnion(const Rect& r1, const Rect& r2, Rect& ur);
	void testRectUnion();

	inline bool CheckCollision(
		const Rect& r1,
		const Rect& r2
	)
	{
		int left1 = r1.left, top1 = r1.top, right1 = r1.left + r1.width, bottom1 = r1.top + r1.height;
		int left2 = r2.left, top2 = r2.top, right2 = r2.left + r2.width, bottom2 = r2.top + r2.height;

		int  x1, x2;
		if (bottom2 < top1 || top2 > bottom1) {
			return false;
		}
		else
		{
			x1 = (left2 > left1) ? left2 : left1;
			x2 = (right2 < right1) ? right2 : right1;
			if (x1 > x2)
				return false;
			return true;
		}
	}
}

#endif

