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
}

#endif

