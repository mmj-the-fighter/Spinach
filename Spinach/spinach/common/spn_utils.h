#ifndef SPN_UTILS_H
#define SPN_UTILS_H
#include <string>

namespace spn
{
	struct Rect;
	void FindRectToRectIntersection(const Rect& r1, const Rect& r2, Rect& ir);
	std::string GetTimeBasedScreenShotFileName();
	std::string GetTimeBasedScreenRecordingFileName();
}

#endif

