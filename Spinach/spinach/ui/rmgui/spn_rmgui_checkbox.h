#ifndef SPN_RMGUI_CHECKBOX_H
#define SPN_RMGUI_CHECKBOX_H

#include <iostream>
#include <functional>

#include <spn_canvas.h>
#include <spn_image.h>
#include <spn_ui_scheme.h>
#include <rmgui/spn_rmgui_widget.h>

namespace spn::rmgui {
	class Checkbox : public Widget
	{
	public:
		Checkbox() {
			Subscribe(spn::ui::UiEventType::MouseDown);
			Subscribe(spn::ui::UiEventType::MouseUp);
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
		bool OnMouseDown(spn::ui::MouseButton button, int x, int y) {
			if (button == spn::ui::MouseButton::Left) {
				return OnLmbDown(x, y);
			}
			return false;
		}

		bool OnMouseUp(spn::ui::MouseButton button, int x, int y) {
			if (button == spn::ui::MouseButton::Left) {
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
				spn::ui::UiScheme::GetInstance()
				.checkboxColor
			);
			canvas->DrawRectangle(x, y, x + sqSize, y + sqSize);
			if (isChecked) {
				canvas->SetPrimaryColorUint(
					spn::ui::UiScheme::GetInstance()
					.checkboxFillColor
				);
				canvas->DrawFilledRectangle(x + 2, y + 2, x + sqSize - 2, y + sqSize - 2);
			}
			float tw = 0, th = 0;
			canvas->GetCStringDisplaySize(labelText.c_str(), tw, th);
			canvas->SetPrimaryColorUint(
				spn::ui::UiScheme::GetInstance()
				.textColor
			);
			canvas->DrawCString(labelText.c_str(), x + sqSize + 4, y + (sqSize - th) / 2);

		}

		void SetCStringLabel(const char* t) {
			labelText = t;
		}

		void SetStringLabel(std::string&& s) {
			labelText = s;
		}
	private:
		bool IsPointInsideCb(int mx, int my) {
			sqSize = spn::ui::UiScheme::GetInstance().checkboxSize;
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
