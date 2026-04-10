#ifndef IMGUI_CHECKBOX_H
#define IMGUI_CHECKBOX_H

#include <iostream>
#include <functional>

#include <spn_canvas.h>
#include <spn_image.h>
#include <ui_scheme.h>
#include <ui_event.h>
		
namespace spn::imgui {
	enum {
		BTN_PRESS,
		BTN_RELEASE,
		BTN_HOVER
	};


	static bool IsMouseInsideSquare(int mx, int my, int x, int y, int sqSize) {
		return (mx >= x) && (mx <= x + sqSize) && (my >= y) && (my <= y + sqSize);
	}

	static bool IsMouseInsideRectangle(int mx, int my, int x, int y, int w, int h) {
		return (mx >= x) &&
			(mx <= x + w) &&
			(my >= y) &&
			(my <= y + h);
	}

	static bool Checkbox(
		spn::Canvas* canvas,
		spn::rmgui::UiEvent& uie,
		const char* labelText,
		int x, int y,
		bool& isChecked) {
		using namespace spn::rmgui;
		int sqSize = UiScheme::GetInstance().checkboxSize;
		unsigned int cbColor = UiScheme::GetInstance().checkboxColor;
		unsigned int cbFillColor = UiScheme::GetInstance().checkboxFillColor;
		unsigned int textColor = UiScheme::GetInstance().textColor;
		bool checkboxChanged;
		bool lastStatus = isChecked;
		if ((uie.eventType == UiEventType::MouseUp) &&
			IsMouseInsideSquare(uie.mouseX, uie.mouseY, x, y, sqSize)) {
			isChecked = !isChecked;
			//clear event
			uie = {};
		}
		canvas->SaveColors();
		canvas->SetPrimaryColorUint(cbColor);
		canvas->DrawRectangle(x, y, x + sqSize, y + sqSize);


		if (isChecked) {
			canvas->SetPrimaryColorUint(cbFillColor);
			canvas->DrawFilledRectangle(x + 2, y + 2, x + sqSize - 2, y + sqSize - 2);
		}

		float tw = 0, th = 0;
		canvas->GetStringDisplaySize(labelText, tw, th);
		canvas->SetPrimaryColorUint(textColor);
		canvas->DrawString(labelText, x + sqSize + 4, y + (sqSize - th) / 2);
		canvas->RestoreColors();
		return (lastStatus != isChecked);
	}


	static bool Button(
		spn::Canvas* canvas, spn::rmgui::UiEvent& uie,
		const char* buttonText,
		int x, int y, int w, int h,
		int& state)
	{
		bool rv = false;

		switch (state)
		{
		case spn::imgui::BTN_PRESS:
			canvas->SetPrimaryColorUint(
				spn::rmgui::UiScheme::GetInstance().buttonPressColor);
			canvas->DrawRectangle(x, y, x + w, y + h);
			canvas->DrawFilledRectangle(x + 2, y + 2, x + w - 2, y + h - 2);
			canvas->SetPrimaryColorUint(
				spn::rmgui::UiScheme::GetInstance().textColor);
			canvas->DrawString(buttonText, x + 12, y + 1);
			break;
		case spn::imgui::BTN_HOVER:
			canvas->SetPrimaryColorUint(
				spn::rmgui::UiScheme::GetInstance().buttonHoverColor);
			canvas->DrawRectangle(x, y, x + w, y + h);
			canvas->DrawFilledRectangle(x + 2, y + 2, x + w - 2, y + h - 2);
			canvas->SetPrimaryColorUint(
				spn::rmgui::UiScheme::GetInstance().textColor);
			canvas->DrawString(buttonText, x + 8, y + 2);
			break;
		case spn::imgui::BTN_RELEASE:
			canvas->SetPrimaryColorUint(
				spn::rmgui::UiScheme::GetInstance().buttonReleaseColor);
			canvas->DrawRectangle(x, y, x + w, y + h);
			canvas->DrawFilledRectangle(x + 2, y + 2, x + w - 2, y + h - 2);
			canvas->SetPrimaryColorUint(
				spn::rmgui::UiScheme::GetInstance().textColor);
			canvas->DrawString(buttonText, x + 8, y + 2);
			break;
		}

		switch (uie.eventType) {
		case spn::rmgui::UiEventType::MouseUp:
			if (uie.mouseButton == spn::rmgui::MouseButton::Left) {
				//leftbutton is up
				if (IsMouseInsideRectangle(uie.mouseX, uie.mouseY, x, y, w, h)) {
					rv = true;
					state = spn::imgui::BTN_RELEASE;
					//clear event
					uie = {};
				}
			}
			break;
		case spn::rmgui::UiEventType::MouseDown:
			if (uie.mouseButton == spn::rmgui::MouseButton::Left) {
				if (IsMouseInsideRectangle(uie.mouseX, uie.mouseY, x, y, w, h)) {
					state = spn::imgui::BTN_PRESS;
				}
			}
			break;
		case spn::rmgui::UiEventType::MouseMove:
			if (IsMouseInsideRectangle(uie.mouseX, uie.mouseY, x, y, w, h)) {
				state = spn::imgui::BTN_HOVER;
			}
			else {
				state = spn::imgui::BTN_RELEASE;
			}
			break;
		}

		
		return rv;
	}
}



#endif 
