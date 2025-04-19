#ifndef _SPN_ANIMATION_H_
#define _SPN_ANIMATION_H_

class Canvas;

namespace spn {
	class Animation {
	public:
		virtual void Update(float deltaTime) = 0;
		virtual bool IsFinished() const = 0;
		virtual void Render(Canvas* canvas) = 0;
		virtual ~Animation() {};
	};
}

#endif
