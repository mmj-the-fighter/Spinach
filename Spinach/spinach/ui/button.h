#ifndef _BUTTON_H_
#define _BUTTON_H_

#include <iostream>
#include <functional>
#include <string>

#include "../spinach/core/spn_canvas.h"
#include "../spinach/core/spn_image.h"
#include "ui_scheme.h"
#include "widget.h"

enum {
	BTN_PRESS,
	BTN_RELEASE,
	BTN_HOVER
};

class Button : public Widget
{
public:
	Button() {
		Subscribe(UiEventType::MouseDown);
		Subscribe(UiEventType::MouseUp);
		Subscribe(UiEventType::MouseMove);
	}
	void SetCallback(std::function<void(int id)> onClickCallback = nullptr) {
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

	bool OnMouseDown(MouseButton button, int x, int y) {
		if (button == MouseButton::Left) {
			return OnLmbDown(x,y);
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

		if (IsPointInsideBtn(mx, my)) {
			state = BTN_PRESS;
			SetFocus(true);
			return true;
		}
		return false;
	}

	bool OnMouseMove(int mx, int my) {
		if (IsPointInsideBtn(mx, my)) {
			if (state == BTN_RELEASE)
			{
				state = BTN_HOVER;
			}
			return true;
		}
		else {
			state = BTN_RELEASE;
		}
		return false;
	}

	bool OnLmbUp(int mx, int my) {
		SetFocus(false);
		if (IsPointInsideBtn(mx, my)) {
			
			if (onClickFn != nullptr) {
				onClickFn(id);
			}
			state = BTN_RELEASE;
			return true;
		}
		return false;
	}
	void Display(spn::Canvas* canvas) {

		switch (state)
		{
		case BTN_PRESS:
			canvas->SetPrimaryColorUint(
				UiScheme::GetInstance().buttonPressColor);
			canvas->DrawRectangle(x, y, x + w, y + h);
			canvas->DrawFilledRectangle(x + 2, y + 2, x + w - 2, y + h - 2);
			canvas->SetPrimaryColorUint(
				UiScheme::GetInstance().textColor);
			canvas->DrawString(text, x + 12, y + 1);
			break;
		case BTN_HOVER:
			canvas->SetPrimaryColorUint(
				UiScheme::GetInstance().buttonHoverColor);
			canvas->DrawRectangle(x, y, x + w, y + h);
			canvas->DrawFilledRectangle(x + 2, y + 2, x + w - 2, y + h - 2);
			canvas->SetPrimaryColorUint(
				UiScheme::GetInstance().textColor);
			canvas->DrawString(text, x + 8, y + 2);
			break;
		case BTN_RELEASE:
			canvas->SetPrimaryColorUint(
				UiScheme::GetInstance().buttonReleaseColor);
			canvas->DrawRectangle(x, y, x + w, y + h);
			canvas->DrawFilledRectangle(x + 2, y + 2, x + w - 2, y + h - 2);
			canvas->SetPrimaryColorUint(
				UiScheme::GetInstance().textColor);
			canvas->DrawString(text, x + 8, y + 2);
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

	std::function<void(int)> onClickFn;
	int x = 0;
	int y = 0;
	int w = 0;
	int h = 0;
	int state = BTN_RELEASE;
	std::string text;

};





#endif 
