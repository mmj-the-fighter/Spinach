#ifndef _INTERPOLATOR_H_
#define _INTERPOLATOR_H_
#include <spn_geom.h>
namespace utils {
	class Vec2dInterpolator
	{
	private:
		spn::Vec2d startValue;
		spn::Vec2d endValue;
		float duration;
		float elapsedTime;
		spn::Vec2d currentValue;
	public:

		inline bool Set(spn::Vec2d aStartValue, spn::Vec2d aEndValue, float aDuration)
		{
			bool isValidArgs = true;
			startValue = aStartValue;
			endValue = aEndValue;
			if (aDuration <= 0) {
				duration = 0;
				isValidArgs = false;
			}
			else {
				duration = aDuration;
			}
			elapsedTime = 0;
			currentValue = startValue;
			return isValidArgs;
		}

		inline spn::Vec2d Update(float deltaTime)
		{
			elapsedTime += deltaTime;
			if (elapsedTime >= duration) {
				elapsedTime = duration;
				currentValue = endValue;
				return endValue;
			}
			currentValue = spn::Lerp(startValue, endValue, (elapsedTime / duration));
			return currentValue;
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
			currentValue = startValue;
		}
	};
}
#endif
