#include <string>
#include <vector>
#include <assert.h>
#include "spn_image.h"
#include "spn_canvas.h"
#include "spn_sprite.h"


namespace spn
{
		Sprite::Sprite(
			Image *spriteImage, 
			int rows, 
			int cols, 
			bool isAnimated, 
			float animDelay)
			: 
			image(spriteImage), 
			elpTime(0), 
			isAnimationEnabled(isAnimated)
		{
			assert(animDelay >= 0);
			assert(spriteImage && rows > 0 && cols > 0);
			delay = animDelay;
			w = image->GetCanvas()->GetWidth();
			h = image->GetCanvas()->GetHeight();
			mxc = cols;
			mxr = rows;
			fw = w / mxc;
			fh = h / mxr;
			mxf = mxr * mxc;
			cfm = 0;
			pos.x = 0;
			pos.y = 0;
			CalculateCurFramePosition();
		}		
		
		void Sprite::CalculateCurFramePosition() {
			int r = cfm / mxc;
			int c = cfm % mxc;
			frmPosx = c * fw;
			frmPosy = r * fh;
		}
		
		void Sprite::SetFrame(int n){
			if(n < 0 || n >= mxf) {
				return;
			}
			cfm = n;
			CalculateCurFramePosition();
		}
		
		void Sprite::SetPosition(int ax, int ay) {
			pos.x = ax;
			pos.y = ay;
		}
		
		void Sprite::EnableAnimation(bool flag){
			isAnimationEnabled = flag;
		}

		void Sprite::AdvanceFrame() {
			if (isCurAnimSet) {
				++cfm;
				if (cfm > curAnim.max) {
					cfm = curAnim.min;
				}
			}
			else {
				cfm = (cfm + 1) % mxf;
			}
			CalculateCurFramePosition();
		}
	
		void Sprite::Update(float dt) {
			if (!isAnimationEnabled) {
				return;
			}
			if (delay != 0) {
				elpTime += dt;
				while (elpTime >= delay) {
					elpTime -= delay;
					AdvanceFrame();
				}
			}
			else {
				AdvanceFrame();
			}
		}
		
		void Sprite::Display(Canvas* canvas) {
			canvas->DrawSubImage(image, pos.x, pos.y, frmPosx, frmPosy, fw, fh);
		}

		bool Sprite::AddAnim(int animId, int startFrame, int endFrame) {
			assert(startFrame >= 0 && startFrame <= endFrame && endFrame < mxf);
			auto it = animMap.find(animId);
			if (it != animMap.end()) {
				return false;
			}
			animMap[animId] = { startFrame, endFrame };
			return true;
		}

		void Sprite::SetAnim(int animId) {
			auto it = animMap.find(animId);
			if (it == animMap.end()) {
				return;
			}
			curAnim = it->second;
			cfm = curAnim.min;
			CalculateCurFramePosition();
			isCurAnimSet = true;
			elpTime = 0;
		}
	

}

