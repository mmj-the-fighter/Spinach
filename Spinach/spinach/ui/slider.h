#ifndef _SLIDER_H_
#define _SLIDER_H_

#include <iostream>
#include <sstream>
#include <iomanip>
#include <functional>
#include <string>

#include "../spinach/core/spn_canvas.h"
#include "../spinach/core/spn_image.h"
#include "../spinach/common/spn_utils.h"
#include "ui_scheme.h"
#include "widget.h"

namespace spn::rmgui {

	enum {
		SLIDER_KNOB_PRESS,
		SLIDER_KNOB_DRAG,
		SLIDER_KNOB_RELEASE,
		SLIDER_KNOB_HOVER,
		SLIDER_TRACK_RELEASE,
		SLIDER_TRACK_PRESS,
	};

	class Slider : public Widget
	{
	public:
		Slider() {
			Subscribe(UiEventType::MouseDown);
			Subscribe(UiEventType::MouseUp);
			Subscribe(UiEventType::MouseMove);
			Subscribe(UiEventType::MouseDrag);
		}
		void SetCallback(std::function<void(int id, float value)> onValueChangedCallback = nullptr) {
			onValueChangedFn = onValueChangedCallback;
		}

		void SetRangeAndValue(float start, float end, float val) {
			rangeStart = start;
			rangeEnd = end;
			if (val < start) {
				val = start;
			}
			else if (val > end) {
				val = end;
			}
			value = val;
		}

		void CalculateKnobPosition() {
			knobx = (value - rangeStart) * (x + w - x - knobw) / (rangeEnd - rangeStart) + x;
			knoby = y;

			value = rangeStart + (knobx - x) * (rangeEnd - rangeStart) / (x + w - x - knobw);
			PrepareText(value);
			if (onValueChangedFn != nullptr) {
				onValueChangedFn(id, value);
			}
		}

		void SetSize(int width, int height) {
			w = width;
			h = height;
			knobh = h;
		}

		void SetPosition(int ax, int ay) {
			x = knobx = ax;
			y = knoby = ay;
		}


		bool OnMouseDown(MouseButton button, int x, int y) {
			if (button == MouseButton::Left) {
				return OnLmbDown(x, y);
			}
			return false;
		}

		bool OnMouseUp(MouseButton button, int x, int y) {
			if (button == MouseButton::Left) {
				return OnLmbUp(x, y);
			}
			return false;
		}

		bool OnLmbDown(int mx, int my) {
			if (IsPointInsideKnob(mx, my)) {
				SetFocus(true);
				//std::cout << "inside\n";
				lastMouseX = mx;
				state = SLIDER_KNOB_PRESS;
				isHover = true;
			}
			else if (IsPointInsideTrack(mx, my)) {
				state = SLIDER_TRACK_PRESS;
				isHover = false;
				SetFocus(true);
				return true;
			}

			return false;
		}

		bool OnMouseMove(int mx, int my) {
			isHover = IsPointInsideKnob(mx, my);
			if (!IsPointInsideTrack(mx, my)) {
				SetFocus(false);
			}
			return false;
		}

		bool OnMouseDrag(int mx, int my) {
			isHover = IsPointInsideKnob(mx, my);

			if (state == SLIDER_KNOB_PRESS) {
				state = SLIDER_KNOB_DRAG;
			}
			if (state == SLIDER_KNOB_DRAG) {
				isHover = true;

				float mouseDelta = mx - lastMouseX;
				lastMouseX = mx;
				knobx += mouseDelta * sensitivity;
				if (knobx < x) {
					knobx = x;
				}
				else if (knobx + knobw > x + w) {
					knobx = x + w - knobw;
				}

				float nv = rangeStart + ((knobx - x) * (rangeEnd - rangeStart) / (x + w - x - knobw));
				bool valueChanged = false;
				if (nv != value) {
					valueChanged = true;
					value = nv;
					PrepareText(nv);
				}
				if (onValueChangedFn != nullptr && valueChanged) {
					onValueChangedFn(id, value);
				}

				return true;
			}
			else {
				return false;
			}
		}


		bool OnLmbUp(int mx, int my) {
			SetFocus(false);
			if (state == SLIDER_TRACK_PRESS && IsPointInsideTrack(mx, my)) {
				knobx = mx;
				float nv = ((knobx - x) * (rangeEnd - rangeStart) / (x + w - x - knobw));
				//std::cout <<"\n" <<value << "\t\t";
				bool valueChanged = false;
				if (nv != value) {
					valueChanged = true;
					value = nv;
					PrepareText(nv);
				}
				if (onValueChangedFn != nullptr && valueChanged) {
					onValueChangedFn(id, value);
				}
				state = SLIDER_TRACK_RELEASE;
				return true;
			}
			isHover = IsPointInsideKnob(mx, my);


			state = SLIDER_TRACK_RELEASE;
			return false;
		}
		void Display(spn::Canvas* canvas) {
			canvas->SetPrimaryColorUint(
				UiScheme::GetInstance().sliderTrackColor);
			canvas->DrawRectangle(x, y, x + w, y + h);
			canvas->DrawFilledRectangle(x + 2, y + 2, x + w - 2, y + h - 2);
			if (isHover) {
				canvas->SetPrimaryColorUint(
					UiScheme::GetInstance().sliderKnobColor);
				canvas->DrawRectangle(knobx, knoby, knobx + knobw, knoby + h);
				canvas->SetPrimaryColorUint(
					UiScheme::GetInstance().sliderKnobHoverColor);
				canvas->DrawFilledRectangle(
					knobx + 2, knoby + 2, knobx + knobw - 2, knoby + knobh - 2);
			}
			else {
				canvas->SetPrimaryColorUint(
					UiScheme::GetInstance().sliderKnobColor);
				canvas->DrawFilledRectangle(knobx, knoby, knobx + knobw, knoby + h);
			}



			canvas->SetPrimaryColorUint(
				UiScheme::GetInstance().sliderTextColor);
			float tw = 0, th = 0;
			canvas->GetStringDisplaySize(text, tw, th);

			canvas->SetPrimaryColorUint(
				UiScheme::GetInstance()
				.textColor
			);
			canvas->DrawString(text, x + (w - tw) / 2, y + (h - th) / 2);
			canvas->DrawString(labelText, x + w + 4, y + (h - th) / 2);
		}

		void SetSensitivity(float s) {
			sensitivity = s;
		}
		void SetDecimalPlaces(int dp) {
			decimalPlaces = dp;
		}
		void SetCStringLabel(const char* t) {
			labelText = t;
		}
		void SetStringLabel(std::string&& s) {
			labelText = s;
		}
	private:

		void PrepareText(float v) {
			std::ostringstream oss;
			oss << std::fixed << std::setprecision(decimalPlaces) << v;
			text = oss.str();
		}

		bool IsPointInsideKnob(int mx, int my) {
			return (mx >= knobx) &&
				(mx <= knobx + knobw) &&
				(my >= knoby) &&
				(my <= knoby + knobh);
		}

		bool IsPointInsideTrack(int mx, int my) {
			return (mx >= x) &&
				(mx <= x + w) &&
				(my >= y) &&
				(my <= y + h);
		}



		std::function<void(int, float)> onValueChangedFn;
		float x = 0;
		float y = 0;
		float w = 0;
		float h = 0;
		float knobx = 0;
		float knoby = 0;
		float knobw = 16;
		float knobh = 0;
		int state = SLIDER_TRACK_RELEASE;
		float value = 0;
		float rangeStart = 0;
		float rangeEnd = 0;
		float sensitivity = 1.0f;
		float lastMouseX = 0;
		bool isHover = false;
		int decimalPlaces = 2;

		std::string text;
		std::string labelText;
	};

}



#endif 
