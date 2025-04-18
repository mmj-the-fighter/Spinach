#ifndef _MOVE_ALPHA_ANIM_H_
#define _MOVE_ALPHA_ANIM_H_

#include "../../common/spn_utils.h"
#include "../spn_image.h"
#include "../spn_canvas.h"
namespace spn {
	class MoveAlphaAnimation {
		spn::Image* image;
		spn::Image* fromImage;
		spn::Image* toImage;
		Vector2D startPos;
		Vector2D endPos;
		spn::Vector2D curPos;
		spn::Vector2D moveDelta;
		float duration;
		float finalAlpha;
		//float elapsedTime;
		float posElapsedTime;
		float alphaElapsedTime;
		bool isAnimFinished;
	public:
		MoveAlphaAnimation(
			spn::Image* img, 
			spn::Vector2D from, 
			spn::Vector2D to,
			float endAlpha, 
			float totalTime);
		~MoveAlphaAnimation();
		void Update(float deltaTime);

		inline bool IsAnimFinished() const
		{
			return (alphaElapsedTime >= duration && posElapsedTime >= duration);
		}

		inline void Render(spn::Canvas* canvas) {
			canvas->DrawImage(image, curPos.x, curPos.y);
		}

	private:
		void UpdatePos(float deltaTime);
		void UpdateAlpha(float deltaTime);
	};
}

#endif