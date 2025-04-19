#ifndef _ALPHA_REV_ANIM_H_
#define _ALPHA_REV_ANIM_H_

#include "../../common/spn_utils.h"
#include "../spn_image.h"
#include "../spn_canvas.h"
namespace spn {
	class AlphaRevAnimation {
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