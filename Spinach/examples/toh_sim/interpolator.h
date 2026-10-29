#ifndef THREE_SEGMENT_ANIMATOR
#define THREE_SEGMENT_ANIMATOR

#include <algorithm>
#include <spn_geom.h>
#include <iostream>

class ThreeSegmentAnimator {
private:
	spn::Vec2d points[4];
	spn::Vec2d currentValue;
	float elapsedTime;
	float duration;
	bool isInited;

public:
	void Set(float aDuration, spn::Vec2d inputArray[4]) {
		duration = aDuration;
		elapsedTime = 0;
		points[0] = inputArray[0];
		points[1] = inputArray[1];
		points[2] = inputArray[2];
		points[3] = inputArray[3];
		currentValue = points[0];
		isInited = true;
	}

	inline spn::Vec2d& GetPoint(int i) {
		return points[i];
	}

	inline spn::Vec2d GetCurrentValue() const
	{
		return currentValue;
	}

	inline bool IsEndValueReached() const
	{
		return (elapsedTime >= duration);
	}

	inline void Reset()
	{
		elapsedTime = 0;
		currentValue = points[0];
	}

	spn::Vec2d Update(float deltaTime)
	{
		if (!isInited) {
			return { 0,0 };
		}
		elapsedTime += deltaTime;
		if (elapsedTime >= duration) {
			elapsedTime = duration;
			currentValue = points[3];
			return currentValue;
		}
		currentValue = Interpolate(elapsedTime / duration);
		return currentValue;
	}

	spn::Vec2d Interpolate(float dt)
	{
		constexpr float delta = 1.0f / 3.0f;
		int p0 = static_cast<int>(dt * 3.0f);
		int p1 = p0 + 1;
		float t = (dt - delta * p0) / delta;
		float tQuad = t * t * t * t;
		return spn::Lerp(points[p0], points[p1], tQuad);
	}
};

#endif