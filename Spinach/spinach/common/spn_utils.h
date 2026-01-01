#ifndef SPN_UTILS_H
#define SPN_UTILS_H
#include <string>
namespace spn
{
	typedef struct _Rect
	{
		int left;
		int top;
		int width;
		int height;
	}Rect;
	void FindRectToRectIntersection(const Rect& r1, const Rect& r2, Rect& ir);
	std::string GetTimeBasedScreenShotFileName();
}

#endif

