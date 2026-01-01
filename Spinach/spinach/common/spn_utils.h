#ifndef SPN_UTILS_H
#define SPN_UTILS_H
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
}

#endif

