#include "spn_animator.h"

namespace spn {
	
	Animator::Animator() {
		isActive = false;
	}

	void Animator::EnqueueMoveAnim(spn::Image* image, 
		Vector2D srcPos, Vector2D dstPos, 
		float duration) 
	{
		moveAnimQueue.push_back(new MoveAnimation(image, srcPos, dstPos, duration));
	}
	
	void Animator::EnqueueAlphaAnim(spn::Image* image, 
		Vector2D pos, 
		float targetAlpha, 
		float duration) {
		alphaAnimQueue.push_back(new AlphaAnimation(image, pos, targetAlpha, duration));
	}	
	
	void Animator::EnqueueAlphaRevAnim(spn::Image* image, 
		Vector2D pos, 
		float targetAlpha, 
		float duration) {
		alphaRevAnimQueue.push_back(new AlphaRevAnimation(image, pos, targetAlpha, duration));
	}	
	
	void Animator::EnqueueMoveAlphaAnim(
		spn::Image* image,
		Vector2D srcPos, Vector2D dstPos,
		float targetAlpha,
		float duration) 
	{
		moveAlphaAnimQueue.push_back(new MoveAlphaAnimation(image, srcPos, dstPos, targetAlpha, duration));
	}


	void Animator::Update(float dt) {
		if (!isActive) {
			return;
		}
		for (auto& anim : moveAnimQueue) {
			if (!anim->IsAnimFinished()) {
				anim->Update(dt);
			}
		}
		
		for (auto& anim : alphaAnimQueue) {
			if (!anim->IsAnimFinished()) {
				anim->Update(dt);
			}
		}		
		
		for (auto& anim : moveAlphaAnimQueue) {
			if (!anim->IsAnimFinished()) {
				anim->Update(dt);
			}
		}
		
		for (auto& anim : alphaRevAnimQueue) {
			if (!anim->IsAnimFinished()) {
				anim->Update(dt);
			}
		}
	}

	void Animator::Render(spn::Canvas* canvas) {
		if (!isActive) {
			return;
		}

		for (auto& anim : moveAnimQueue) {
			anim->Render(canvas);
		}
		for (auto& anim : alphaAnimQueue) {
			anim->Render(canvas);
		}		
		
		for (auto& anim : moveAlphaAnimQueue) {
			anim->Render(canvas);
		}
		
		for (auto& anim : alphaRevAnimQueue) {
			anim->Render(canvas);
		}
	}

	bool Animator::DeactivateAnimsIfFinished() {
		for (auto& anim : moveAnimQueue) {
			if (anim->IsAnimFinished() == false) {
				return false;
			}
		}

		for (auto& anim : alphaAnimQueue) {
			if (anim->IsAnimFinished() == false) {
				return false;
			}
		}		
		
		for (auto& anim : moveAlphaAnimQueue) {
			if (anim->IsAnimFinished() == false) {
				return false;
			}
		}
		
		for (auto& anim : alphaRevAnimQueue) {
			if (anim->IsAnimFinished() == false) {
				return false;
			}
		}

		Clear();
		return true;
	}


	void Animator::Clear() {
		for (auto& anim : moveAnimQueue) {
			delete anim;
		}
		for (auto& anim : alphaAnimQueue) {
			delete anim;
		}		
		
		for (auto& anim : moveAlphaAnimQueue) {
			delete anim;
		}
		
		for (auto& anim : alphaRevAnimQueue) {
			delete anim;
		}
		moveAnimQueue.clear();
		alphaAnimQueue.clear();
		moveAlphaAnimQueue.clear();
		alphaRevAnimQueue.clear();
	}


}