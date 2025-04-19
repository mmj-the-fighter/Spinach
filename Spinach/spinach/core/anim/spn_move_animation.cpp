#include "spn_move_animation.h"

namespace spn {
	MoveAnimation::MoveAnimation(spn::Image* img, Vector2D from, Vector2D to, float totalTime)
		: image(img), startPos(from), curPos(startPos), endPos(to),
		duration(totalTime), elapsedTime(0), isAnimFinished(false)
	{
		moveDelta.x = endPos.x - startPos.x;
		moveDelta.y = endPos.y - startPos.y;
	}

	void MoveAnimation::Update(float deltaTime)
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

	
	bool MoveAnimation::IsFinished() const
	{
		return (elapsedTime >= duration);
	}

	void MoveAnimation::Render(spn::Canvas* canvas) {
		canvas->DrawImage(image, curPos.x, curPos.y);
	}
}