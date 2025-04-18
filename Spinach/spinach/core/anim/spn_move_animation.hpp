#ifndef _MOVE_ANIM_H_
#define _MOVE_ANIM_H_

#include "../../common/spn_utils.h"
#include "../spn_image.h"
#include "../spn_canvas.h"

namespace spn {
	class MoveAnimation {
		spn::Image* image;
		Vector2D startPos;
		Vector2D endPos;
		Vector2D curPos;
		Vector2D moveDelta;
		float duration;
		float elapsedTime;
		bool isAnimFinished;
	public:
		MoveAnimation(spn::Image* img, Vector2D from, Vector2D to, float totalTime)
			: image(img), startPos(from), curPos(startPos), endPos(to),
			duration(totalTime), elapsedTime(0), isAnimFinished(false)
		{
			moveDelta.x = endPos.x - startPos.x;
			moveDelta.y = endPos.y - startPos.y;
		}

		inline void Update(float deltaTime)
		{
			elapsedTime += deltaTime;
			if (elapsedTime >= duration) {
				elapsedTime = duration;
				curPos.x = endPos.x;
				curPos.y = endPos.y;
				return;
			}
			float t = (elapsedTime / duration);
			curPos.x = startPos.x + moveDelta.x * t;
			curPos.y = startPos.y + moveDelta.y * t;
		}

		inline bool IsAnimFinished() const
		{
			return (elapsedTime >= duration);
		}

		inline void Render(spn::Canvas* canvas) {
			canvas->DrawImage(image, curPos.x, curPos.y);
		}
	};
}
#endif