#ifndef _INTERPOLATOR_H_
#define _INTERPOLATOR_H_
#include <spn_geom.h>
namespace utils {
	template <typename T>
	class Vec2dInterpolator
	{
	private:
		spn::Vec2d<T> startValue;
		spn::Vec2d<T> endValue;
		float duration;
		float elapsedTime;
		spn::Vec2d<T> currentValue;
	public:

		inline bool Set(spn::Vec2d<T> aStartValue, spn::Vec2d<T> aEndValue, T aDuration)
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

		inline spn::Vec2d<T> Update(T deltaTime)
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

		inline spn::Vec2d<T> GetCurrentValue() const
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
