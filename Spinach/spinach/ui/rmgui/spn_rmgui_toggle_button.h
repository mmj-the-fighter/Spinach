#ifndef SPN_RMGUI_TOGGLE_BUTTON_H
#define SPN_RMGUI_TOGGLE_BUTTON_H

#include <iostream>
#include <functional>
#include <string>

#include <spn_canvas.h>
#include <spn_image.h>
#include <spn_ui_scheme.h>
#include <rmgui/spn_rmgui_widget.h>

namespace spn::rmgui {
	enum {
		TB_PRESS,
		TB_NORMAL,
	};

	class ToggleButton : public Widget
	{
	public:
		ToggleButton() {
			Subscribe(spn::ui::UiEventType::MouseDown);
			Subscribe(spn::ui::UiEventType::MouseUp);
		}
		void SetCallback(std::function<void(int id, bool action)> onClickCallback = nullptr) {
			onClickFn = onClickCallback;
		}
		void SetSize(int width, int height) {
			w = width;
			h = height;
		}
		void SetPosition(int ax, int ay) {
			x = ax;
			y = ay;
		}
		void SetCString(const char* t) {
			text = t;
		}
		void SetString(std::string&& s) {
			text = s;
		}

		void Toggle() {
			if (stateCopy == TB_NORMAL) {
				stateCopy = state = TB_PRESS;
				action = true;
			}
			else {
				stateCopy = state = TB_NORMAL;
				action = false;
			}
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
			if (IsPointInsideBtn(mx, my)) {
				SetFocus(true);
				return true;
			}
			return false;
		}

		bool OnLmbUp(int mx, int my) {
			SetFocus(false);
			if (IsPointInsideBtn(mx, my)) {
				Toggle();
				if (onClickFn != nullptr) {
					onClickFn(id, action);
				}
				return true;
			}
			return false;
		}
		
		bool IsPressed() {
			return action;
		}
		void Display(spn::Canvas* canvas) {
			auto& scheme = spn::ui::UiScheme::GetInstance();
			switch (state)
			{
			case TB_PRESS:
				canvas->SetPrimaryColorUint(scheme.buttonReleaseColor);
				canvas->DrawRectangle(x, y, x + w, y + h);
				canvas->DrawFilledRectangle(x + 2, y + 2, x + w - 2, y + h - 2);
				canvas->SetPrimaryColorUint(scheme.textColor);
				canvas->DrawCString(text.c_str(), x + 12, y + 1);
				break;
			case TB_NORMAL:
				canvas->SetPrimaryColorUint(scheme.buttonPressColor);
				canvas->DrawRectangle(x, y, x + w, y + h);
				canvas->DrawFilledRectangle(x + 2, y + 2, x + w - 2, y + h - 2);
				canvas->SetPrimaryColorUint(scheme.textColor);
				canvas->DrawCString(text.c_str(), x + 8, y + 2);
				break;
			}
			//canvas->RestoreColors();
		}
	private:
		bool IsPointInsideBtn(int mx, int my) {
			return (mx >= x) &&
				(mx <= x + w) &&
				(my >= y) &&
				(my <= y + h);
		}

		std::function<void(int, bool)> onClickFn;
		int x = 0;
		int y = 0;
		int w = 0;
		int h = 0;
		int state = TB_NORMAL;
		int stateCopy = TB_NORMAL;
		int stateBeforeToggle = TB_NORMAL;
		std::string text;
		bool action = false;

	};
}

#endif 
