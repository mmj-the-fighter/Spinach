#ifndef _IMAGE_ANIM_H_
#define _IMAGE_ANIM_H_
#include <vector>
#include "../../common/spn_utils.h"
#include "../spn_image.h"
#include "../spn_canvas.h"
#include "spn_move_animation.hpp"
#include "spn_alpha_animation.h"
#include "spn_alpha_rev_animation.h"
#include "spn_move_alpha_animation.h"


namespace spn {

	class Animator
	{
	public:
		Animator();
		void Clear();
		void EnqueueMoveAnim(spn::Image* image, Vector2D srcPos, Vector2D dstPos, float duration);
		void EnqueueAlphaAnim(spn::Image* image, Vector2D pos, float targetAlpha, float duration);
		void EnqueueAlphaRevAnim(spn::Image* image, Vector2D pos, float targetAlpha, float duration);
		void EnqueueMoveAlphaAnim(
			spn::Image* image, 
			Vector2D srcPos, Vector2D dstPos, 
			float targetAlpha, 
			float duration);
		inline void SetActive(bool flag) {
			isActive = flag;
		}
		void Update(float dt);
		void Render(spn::Canvas* canvas);
		bool DeactivateAnimsIfFinished();
	private:
		bool isActive;
		std::vector<MoveAnimation*> moveAnimQueue;
		std::vector<AlphaAnimation*> alphaAnimQueue;
		std::vector<AlphaRevAnimation*> alphaRevAnimQueue;
		std::vector<MoveAlphaAnimation*> moveAlphaAnimQueue;
	};
}


#endif