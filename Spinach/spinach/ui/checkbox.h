#ifndef _CHECKBOX_H_
#define _CHECKBOX_H_

#include <iostream>
#include <functional>

#include "../spinach/core/spn_canvas.h"
#include "../spinach/core/spn_image.h"
#include "ui_scheme.h"
#include "widget.h"

namespace spn::rmgui {
	class Checkbox : public Widget
	{
	public:
		Checkbox() {
			Subscribe(UiEventType::MouseDown);
			Subscribe(UiEventType::MouseUp);
		}
		void SetCallback(std::function<void(int id, bool isChecked)> onCheckboxChangedCallback = nullptr) {
			onCheckboxChangedFn = onCheckboxChangedCallback;
		}
		void SetPosition(int ax, int ay) {
			x = ax;
			y = ay;
		}
		void SetChecked(bool checked) {
			isChecked = checked;
		}
		bool GetChecked() {
			return isChecked;
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
			if (IsPointInsideCb(mx, my)) {
				SetFocus(true);
				return true;
			}
			return false;
		}
		bool OnLmbUp(int mx, int my) {
			if (IsPointInsideCb(mx, my)) {
				SetFocus(false);
				isChecked = !isChecked;
				if (onCheckboxChangedFn != nullptr) {
					onCheckboxChangedFn(id, isChecked);
				}
				return true;
			}
			return false;
		}
		void Display(spn::Canvas* canvas) {
			canvas->SetPrimaryColorUint(
				UiScheme::GetInstance()
				.checkboxColor
			);
			canvas->DrawRectangle(x, y, x + sqSize, y + sqSize);
			if (isChecked) {
				canvas->SetPrimaryColorUint(
					UiScheme::GetInstance()
					.checkboxFillColor
				);
				canvas->DrawFilledRectangle(x + 2, y + 2, x + sqSize - 2, y + sqSize - 2);
			}
			float tw = 0, th = 0;
			canvas->GetStringDisplaySize(labelText, tw, th);
			canvas->SetPrimaryColorUint(
				UiScheme::GetInstance()
				.textColor
			);
			canvas->DrawString(labelText, x + sqSize + 4, y + (sqSize - th) / 2);

		}

		void SetCStringLabel(const char* t) {
			labelText = t;
		}

		void SetStringLabel(std::string&& s) {
			labelText = s;
		}
	private:
		bool IsPointInsideCb(int mx, int my) {
			sqSize = UiScheme::GetInstance().checkboxSize;
			return (mx >= x) &&
				(mx <= x + sqSize) &&
				(my >= y) &&
				(my <= y + sqSize);
		}

		std::function<void(int id, bool isChecked)> onCheckboxChangedFn;
		bool isChecked = false;
		unsigned int sqSize = 14;
		int x = 0;
		int y = 0;
		std::string labelText;
	};

}


#endif 
