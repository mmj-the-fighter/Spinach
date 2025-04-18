#include "spn_move_alpha_animation.h"

namespace spn {
	MoveAlphaAnimation::MoveAlphaAnimation(
		spn::Image* img,
		spn::Vector2D from,
		spn::Vector2D to,
		float endAlpha,
		float totalTime)
	:
		duration(totalTime),
		alphaElapsedTime(0),
		posElapsedTime(0),
		isAnimFinished(false)
	{
		image = img->Clone();
		fromImage = img->Clone();
		toImage = img->Clone();
		toImage->GetCanvas()->SetAlpha(0);
		startPos = from;
		endPos = to;
		moveDelta.x = endPos.x - startPos.x;
		moveDelta.y = endPos.y - startPos.y;
	}

	MoveAlphaAnimation::~MoveAlphaAnimation() {
		delete image;
		delete fromImage;
		delete toImage;
	}

	void MoveAlphaAnimation::Update(float deltaTime) {
		UpdateAlpha(deltaTime);
		UpdatePos(deltaTime);
	}

	void MoveAlphaAnimation::UpdatePos(float deltaTime) {
		posElapsedTime += deltaTime;
		if (posElapsedTime >= duration) {
			posElapsedTime = duration;
			curPos.x = endPos.x;
			curPos.y = endPos.y;
			return;
		}
		float t = (posElapsedTime / duration);
		curPos.x = startPos.x + moveDelta.x * t;
		curPos.y = startPos.y + moveDelta.y * t;
	}

	void MoveAlphaAnimation::UpdateAlpha(float deltaTime)
	{
		alphaElapsedTime += deltaTime;
		if (alphaElapsedTime >= duration) {
			alphaElapsedTime = duration;
			return;
		}
		unsigned char* startPixels = fromImage->GetCanvas()->GetPixelBuffer();
		unsigned char* endPixels = toImage->GetCanvas()->GetPixelBuffer();
		spn::Canvas* canvas = image->GetCanvas();
		int height = canvas->GetHeight();
		int width = canvas->GetWidth();
		int pitch = canvas->GetPitch();
		unsigned char* pixels = canvas->GetPixelBuffer();
		float t;
		if (duration == 0) {
			t = 0;
		}
		else {
			t = (alphaElapsedTime / duration);
			if (t > 1.0f) {
				t = 0.0f;
			}
		}
		for (int y = 0; y < height; ++y) {
			for (int x = 0; x < width; ++x) {
				unsigned char* op = startPixels + pitch * y + x * 4;
				float sb = *op;
				float sg = *(op + 1);
				float sr = *(op + 2);
				float sa = *(op + 3);
				unsigned char* tgt = endPixels + pitch * y + x * 4;
				float db = *tgt;
				float dg = *(tgt + 1);
				float dr = *(tgt + 2);
				float da = *(tgt + 3);
				unsigned char* loc = pixels + pitch * y + x * 4;
				//currentValue = startValue + deltaValue * (elapsedTime / duration);
				*loc++ = sb + (db - sb) * t;
				*loc++ = sg + (dg - sg) * t;
				*loc++ = sr + (dr - sr) * t;
				*loc = sa + (da - sa) * t;
			}
		}
	};
}