#include "spn_alpha_rev_animation.h"

namespace spn {
	AlphaRevAnimation::AlphaRevAnimation(spn::Image* img,
		Vector2D from,
		float endAlpha,
		float totalTime) :
		pos(from),
		duration(totalTime),
		elapsedTime(0),
		isAnimFinished(false) 
	{
		image = img->Clone();
		fromImage = img->Clone();
		fromImage->GetCanvas()->SetAlpha(0);
		toImage = img->Clone();
	}

	AlphaRevAnimation::~AlphaRevAnimation(){
		delete image;
		delete fromImage;
		delete toImage;
	}

	void AlphaRevAnimation::Update(float deltaTime)
	{
		elapsedTime += deltaTime;
		if (elapsedTime >= duration) {
			elapsedTime = duration;
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
			t = (elapsedTime / duration);
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