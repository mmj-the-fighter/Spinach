#ifndef SPN_UTILS_H
#define SPN_UTILS_H

namespace spn
{
	struct Rect;
	void FindRectToRectIntersection(const Rect& r1, const Rect& r2, Rect& ir);
	void GetFilenameFromCurrentTime(char* buffer, const char* prefix, const char* extension);
}

#endif

