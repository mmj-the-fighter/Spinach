#ifndef _MOVE_ANIM_H_
#define _MOVE_ANIM_H_

#include "../../common/spn_utils.h"
#include "../spn_image.h"
#include "../spn_canvas.h"
#include "spn_animation.h"

namespace spn {
	class MoveAnimation : public Animation {
		spn::Image* image;
		Vector2D startPos;
		Vector2D endPos;
		Vector2D curPos;
		Vector2D moveDelta;
		float duration;
		float elapsedTime;
		bool isAnimFinished;
	public:
		MoveAnimation(spn::Image* img, Vector2D from, Vector2D to, float totalTime);
		void Update(float deltaTime);
		bool IsFinished() const;
		void Render(spn::Canvas* canvas);
	};
}
#endif