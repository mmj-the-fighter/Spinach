#ifndef _ALPHA_ANIM_H_
#define _ALPHA_ANIM_H_

#include "../../common/spn_utils.h"
#include "../spn_image.h"
#include "../spn_canvas.h"
namespace spn {
	class AlphaAnimation {
		spn::Image* image;
		spn::Image* fromImage;
		spn::Image* toImage;
		spn::Vector2D pos;
		float duration;
		float finalAlpha;
		float elapsedTime;
		bool isAnimFinished;
	public:
		AlphaAnimation(spn::Image* img, spn::Vector2D from, float endAlpha, float totalTime);
		~AlphaAnimation();
		void Update(float deltaTime);

		inline bool IsAnimFinished() const
		{
			return (elapsedTime >= duration);
		}

		inline void Render(spn::Canvas* canvas) {
			canvas->DrawImage(image, pos.x, pos.y);
		}
	};
}

#endif