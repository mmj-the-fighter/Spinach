#ifndef SPN_RMGUI_TEXT_SCROLLVIEW_H
#define SPN_RMGUI_TEXT_SCROLLVIEW_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <spn_canvas.h>
#include <spn_utils.h>
#include <spn_ui_scheme.h>
#include <rmgui/spn_rmgui_widget.h>

namespace spn::rmgui {
	
	enum TextScrollViewSliderState {
		THUMB_PRESS,
		THUMB_DRAG,
		THUMB_RELEASE,
	};

	enum TextScrollViewRegion {
		OUTSIDE,
		THUMB,
		TRACK,
		TOPPOCKET,
		BOTTOMPOCKET
	};

	class TextScrollView : public Widget
	{
	public:

		TextScrollView() {
			Subscribe(spn::ui::UiEventType::ActionKeyDown);
			Subscribe(spn::ui::UiEventType::MouseDown);
			Subscribe(spn::ui::UiEventType::MouseMove);
			Subscribe(spn::ui::UiEventType::MouseDrag);
			Subscribe(spn::ui::UiEventType::MouseUp);
		}

		inline void SetSensitivity(float v) {
			sensitivity = v;
		}

		inline void Init(int posX, int posY, int width, int height) {
			//init size
			w = width;
			h = height;

			//init position
			x = posX;
			y = posY;

			//init slider
			trackMin = y + thumbH;
			trackMax = y + h - thumbH;
			thumbY = trackMin;
			thumbX = x + w - thumbW;
			thumbState = THUMB_RELEASE;
			lastMouseY = trackMin;

			//init text
			ClearText();
		}

		inline void CalculateThumbPosition() {
			float trialThumbY = trackMin + value * (trackMax - thumbH - trackMin);
			thumbY = ClampValue<float>(trialThumbY, trackMin, trackMax - thumbH);
		}

		bool OnMouseMove(int mx, int my) {
			if (IsPointInsideThumb(mx, my)) {
				isHover = true;
				mouseOverRegion = THUMB;
				return true;
			}
			else if (IsPointInsideTopPocket(mx, my)) {
				isHover = true;
				mouseOverRegion = TOPPOCKET;
				return true;
			}
			else if (IsPointInsideBottomPocket(mx, my)) {
				isHover = true;
				mouseOverRegion = BOTTOMPOCKET;
				return true;
			}
			else {
				isHover = false;
				mouseOverRegion = OUTSIDE;
				return false;
			}
		}

		bool OnMouseDrag(int mx, int my) {
			if (thumbState == THUMB_PRESS) {
				thumbState = THUMB_DRAG;
			}
			if (thumbState == THUMB_DRAG) {
				isHover = true;
				float mouseDelta = my - lastMouseY;
				lastMouseY = my;
				thumbY = ClampValue<float>(
					thumbY + mouseDelta*sensitivity, 
					trackMin, 
					trackMax - thumbH);
				value = (thumbY - trackMin) / (trackMax - thumbH - trackMin);
				float lsz = lines.size();
				lineStart = ClampValue<int>(
					static_cast<int>(lsz * value),
					0, 
					lsz-1);
				return true;
			}
			else {
				return false;
			}
		}

		bool OnMouseDown(spn::ui::MouseButton button, int x, int y) {
			if (button == spn::ui::MouseButton::Left) {
				return OnLmbDown(x, y);
			}
			return false;
		}

		bool OnLmbDown(int mx, int my) {
			if (IsPointInsideTextScrollView(mx, my)) {
				SetFocus(true);
				if (IsPointInsideTopPocket(mx, my)) {
					OnUpArrowKey();
				}
				else if (IsPointInsideBottomPocket(mx, my)) {
					OnDownArrowKey();
				}
				else if (IsPointInsideThumb(mx, my)) {
					thumbState = THUMB_PRESS;
					lastMouseY = my;
					isHover = true;
				}
				else if (IsPointInsideTrack(mx, my)) {
					value = (my - trackMin) / (trackMax - trackMin);
					CalculateThumbPosition();
					lineStart = ClampValue<int>(
						static_cast<int>(lines.size() * value), 
						0, 
						lines.size()-1);
				}
				else {
					isHover = false;
				}
			}
			else {
				SetFocus(false);
				lastMouseY = thumbY;
				isHover = false;
			}
			return false;
		}

		bool OnMouseUp(spn::ui::MouseButton button, int x, int y) {
			if (button == spn::ui::MouseButton::Left) {
				return OnLmbUp(x, y);
			}
			return false;
		}

		bool OnLmbUp(int mx, int my) {
			thumbState = THUMB_RELEASE;
			lastMouseY = thumbY;
			return false;
		}

		bool OnActionKeyDown(spn::ui::KeyCode kc) {
			switch (kc) {
			case spn::ui::KeyCode::Down:
				return OnDownArrowKey();
			case spn::ui::KeyCode::Up:
				return OnUpArrowKey();
			case spn::ui::KeyCode::Right:
				std::cout << value << std::endl;
				return false;
			}
			return false;
		}

		template <typename ValueType>
		constexpr ValueType ClampValue(const ValueType& v, 
			const ValueType& min, 
			const ValueType& max)
		{
			if (v < min) {
				return min;
			}
			else if (v > max) {
				return max;
			}
			else {
				return v;
			}
		}

		bool OnDownArrowKey() {
			if (!HasFocus()) {
				return  false;
			}
			if (lines.size() == 0) {
				return true;
			}
			lineStart = ClampValue<int>(lineStart+1, 0, lines.size()-1);
			float lsz = lines.size();
			value = ClampValue<float>(value + (1.0f / lsz), 0.0f, 1.0f);
			CalculateThumbPosition();
			return true;
		}

		bool OnUpArrowKey() {
			if (!HasFocus()) {
				return  false;
			}
			if (lines.size() == 0) {
				return true;
			}
			lineStart = ClampValue<int>(lineStart-1, 0, lines.size()-1);
			float lsz = lines.size();
			value = ClampValue<float>(value - (1.0f / lsz), 0.0f, 1.0f);
			CalculateThumbPosition();
			return true;
		}

		int GetLineNum() {
			return lines.size();
		}
		

		std::string* GetTextAtLine(size_t lineNo) 
		{
			if (lineNo >= lines.size())
				return nullptr;

			return &lines[lineNo];
		}

		std::string* GetTextAtCurrentLine()
		{
			if (lines.size() == 0)
				return nullptr;

			return &lines[lines.size()-1];
		}

		void AddText(const char* str) {
			lines.push_back(std::string(str));
		}
		
		void ClearText(){
			lines.clear();
			lineStart = 0;
			value = 0.0f;
			CalculateThumbPosition();
		}

		inline const std::vector<std::string>& GetAllLines() {
			return lines;
		}

		void Display(spn::Canvas* canvas) {
			int cornerBoxSize = w;
			canvas->SetClippingRectangle(x, y, w, h);
			auto& scheme = spn::ui::UiScheme::GetInstance();
			int cwidth = canvas->GetWidth();
			int cheight = canvas->GetHeight();
			float lsz = lines.size();
			if (lines.size() == 0) {
				lsz = 1;
			}
			float trackHeight = (trackMax - trackMin);
			float th, tw;
			canvas->GetCharDisplaySize('h', tw, th);
			float maxLinesInView = h/th;
			thumbH = ClampValue<float>(
				trackHeight * (maxLinesInView / lsz),
				16, 
				trackHeight - 16);
			canvas->SetPrimaryColorUint(scheme.textColor);

			if (lines.size() > 0) {
				int k = 0;
				for (int i = lineStart; i < lines.size() && i < lineStart+maxLinesInView; ++i, ++k) {
					//canvas->DrawCString(lines[i].c_str(), x + 4, y + th * k + 2);
					canvas->DrawCString(lines[i].c_str(), x + 4, y + th * k);
				}
			}
			
			
			//Track
			canvas->DrawRectangle(x, y, x + w- thumbW, y + h);
			canvas->DrawRectangle(x + w - thumbW, y, x + w, y + h);

			//Pockets
			int tpmidx = (x + w) - (thumbW/2);
			int tpxmin = x + w - thumbW;
			int tpxmax = x + w;
			int tpymin = y;
			int tpymax = y + thumbW;
			canvas->DrawRectangle(tpxmin,tpymin, tpxmax, tpymax);
			if (isHover && mouseOverRegion == TOPPOCKET) {
				canvas->SetPrimaryColorUint(scheme.buttonHoverColor);
			}
			else {
				canvas->SetPrimaryColorUint(scheme.textColor);
			}
			canvas->DrawLine(tpxmin+4, tpymax-4, tpmidx, tpymin+4);
			canvas->DrawLine(tpmidx, tpymin+4,tpxmax-4,tpymax-4);

			int bpmidx = tpmidx;
			int bpxmin = tpxmin;
			int bpxmax = tpxmax;
			int bpymin = y + h - thumbW;
			int bpymax = y + h;
			canvas->SetPrimaryColorUint(scheme.textColor);
			canvas->DrawRectangle(bpxmin, bpymin, bpxmax, bpymax);
			if (isHover && mouseOverRegion == BOTTOMPOCKET) {
				canvas->SetPrimaryColorUint(scheme.buttonHoverColor);
			}
			else {
				canvas->SetPrimaryColorUint(scheme.textColor);
			}
			canvas->DrawLine(bpxmin+4, bpymin+4, bpmidx, bpymax-4);
			canvas->DrawLine(bpmidx, bpymax-4, bpxmax-4, bpymin+4);

			if (isHover && mouseOverRegion == THUMB) {
				canvas->SetPrimaryColorUint(scheme.buttonHoverColor);
			}
			else {
				canvas->SetPrimaryColorUint(scheme.textColor);
			}
			//Thumb
			canvas->DrawFilledRectangle(thumbX+4, thumbY+4, thumbX+thumbW-4, thumbY+thumbH-4);

			canvas->SetClippingRectangle(0, 0, cwidth, cheight);
		}
	private:
		inline bool IsPointInsideTextScrollView(int mx, int my) {
			return (mx >= x) &&
				(mx <= x + w) &&
				(my >= y) &&
				(my <= y + h);
		}
		inline bool IsPointInsideTopPocket(int mx, int my) {
			return (mx >= x + w - thumbW) &&
				(mx <= x + w) &&
				(my >= y) &&
				(my <= y + thumbW);
		}
		inline bool IsPointInsideBottomPocket(int mx, int my) {
			return (mx >= x + w - thumbW) &&
				(mx <= x + w) &&
				(my >= y + h - thumbW) &&
				(my <= y + h);
		}

		inline bool IsPointInsideThumb(int mx, int my) {
			return (mx >= thumbX) &&
				(mx <= thumbX + thumbW) &&
				(my >= thumbY) &&
				(my <= thumbY + thumbH);
		}

		inline bool IsPointInsideTrack(int mx, int my) {
			return (mx >= x+w-thumbW) &&
				(mx <= x + w) &&
				(my >= trackMin) &&
				(my <= trackMax);
		}


		
		float sensitivity = 1.0f;
		float x = 0;
		float y = 0;
		float w = 0;
		float h = 0;
		float thumbX = 0;
		float thumbY = 0;
		float thumbW = 16;
		float thumbH = 16;
		float trackMin = 0;
		float trackMax = 0;
		float value = 0;
		bool isHover = false;
		int lineStart = 0;
		TextScrollViewSliderState thumbState = THUMB_RELEASE;
		TextScrollViewRegion mouseOverRegion = OUTSIDE;
		int lastMouseY = 0;

		std::vector<std::string> lines;
	};

}

#endif 
