
#ifndef COLLISION_UTILS_HPP
#define COLLISION_UTILS_HPP

inline bool CheckCollision(
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
		x1 = (left2 > left1) ? left2 : left1;
		x2 = (right2 < right1) ? right2 : right1;
		if (x1 > x2)
			return false;
		return true;
	}
}

#endif