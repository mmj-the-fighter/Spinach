#ifndef _ALPHA_REV_ANIM_H_
#define _ALPHA_REV_ANIM_H_

#include "../../common/spn_utils.h"
#include "../spn_image.h"
#include "../spn_canvas.h"
#include "spn_animation.h"

namespace spn {
	class AlphaRevAnimation : public Animation{
		spn::Image* image;
		spn::Image* fromImage;
		spn::Image* toImage;
		spn::Vector2D pos;
		float duration;
		float finalAlpha;
		float elapsedTime;
		bool isAnimFinished;
	public:
		AlphaRevAnimation(spn::Image* img, spn::Vector2D from, float endAlpha, float totalTime);
		~AlphaRevAnimation();
		void Update(float deltaTime);
		bool IsFinished() const;
		void Render(spn::Canvas* canvas);

	};
}

#endif