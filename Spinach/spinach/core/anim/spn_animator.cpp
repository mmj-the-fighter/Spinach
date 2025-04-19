#include "spn_move_animation.h"
#include "spn_alpha_animation.h"
#include "spn_alpha_rev_animation.h"
#include "spn_move_alpha_animation.h"
#include "spn_animator.h"

namespace spn {
	
	Animator::Animator() {
		isActive = false;
	}

	void Animator::EnqueueMoveAnim(spn::Image* image, 
		Vector2D srcPos, Vector2D dstPos, 
		float duration) 
	{
		animations.push_back(new MoveAnimation(image, srcPos, dstPos, duration));
	}
	
	void Animator::EnqueueAlphaAnim(spn::Image* image, 
		Vector2D pos, 
		float targetAlpha, 
		float duration) {
		animations.push_back(new AlphaAnimation(image, pos, targetAlpha, duration));
	}	
	
	void Animator::EnqueueAlphaRevAnim(spn::Image* image, 
		Vector2D pos, 
		float targetAlpha, 
		float duration) {
		animations.push_back(new AlphaRevAnimation(image, pos, targetAlpha, duration));
	}	
	
	void Animator::EnqueueMoveAlphaAnim(
		spn::Image* image,
		Vector2D srcPos, Vector2D dstPos,
		float targetAlpha,
		float duration) 
	{
		animations.push_back(new MoveAlphaAnimation(image, srcPos, dstPos, targetAlpha, duration));
	}


	void Animator::Update(float dt) {
		if (!isActive) {
			return;
		}
		for (auto& anim : animations) {
			if (!anim->IsFinished()) {
				anim->Update(dt);
			}
		}
	}

	void Animator::Render(spn::Canvas* canvas) {
		if (!isActive) {
			return;
		}

		for (auto& anim : animations) {
			anim->Render(canvas);
		}
	}

	bool Animator::DeactivateAnimsIfFinished() {
		for (auto& anim : animations) {
			if (anim->IsFinished() == false) {
				return false;
			}
		}
		Clear();
		return true;
	}


	void Animator::Clear() {
		for (auto& anim : animations) {
			delete anim;
		}
		animations.clear();
	}


}