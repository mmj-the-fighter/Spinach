#ifndef SPN_IMGUI_IMGUI_H
#define SPN_IMGUI_IMGUI_H

#include <iostream>
#include <functional>

#include <spn_canvas.h>
#include <spn_image.h>
#include <spn_ui_scheme.h>
#include <spn_ui_event.h>
		
namespace spn::imgui {
	enum {
		BTN_PRESS,
		BTN_RELEASE,
		BTN_HOVER
	};

	enum AlertResult {
		None,
		Ok,
		Cancel
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
		spn::ui::UiEvent& uie,
		const char* labelText,
		int x, int y,
		bool& isChecked) {
		using namespace spn::ui;
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
		canvas->GetCStringDisplaySize(labelText, tw, th);
		canvas->SetPrimaryColorUint(textColor);
		canvas->DrawCString(labelText, x + sqSize + 4, y + (sqSize - th) / 2);
		canvas->RestoreColors();
		return (lastStatus != isChecked);
	}


	static bool Button(
		spn::Canvas* canvas, spn::ui::UiEvent& uie,
		const char* buttonText,
		int x, int y, int w, int h,
		int& state)
	{
		bool rv = false;

		switch (state)
		{
		case spn::imgui::BTN_PRESS:
			canvas->SetPrimaryColorUint(
				spn::ui::UiScheme::GetInstance().buttonPressColor);
			canvas->DrawRectangle(x, y, x + w, y + h);
			canvas->DrawFilledRectangle(x + 2, y + 2, x + w - 2, y + h - 2);
			canvas->SetPrimaryColorUint(
				spn::ui::UiScheme::GetInstance().textColor);
			canvas->DrawCString(buttonText, x + 12, y + 1);
			break;
		case spn::imgui::BTN_HOVER:
			canvas->SetPrimaryColorUint(
				spn::ui::UiScheme::GetInstance().buttonHoverColor);
			canvas->DrawRectangle(x, y, x + w, y + h);
			canvas->DrawFilledRectangle(x + 2, y + 2, x + w - 2, y + h - 2);
			canvas->SetPrimaryColorUint(
				spn::ui::UiScheme::GetInstance().textColor);
			canvas->DrawCString(buttonText, x + 8, y + 2);
			break;
		case spn::imgui::BTN_RELEASE:
			canvas->SetPrimaryColorUint(
				spn::ui::UiScheme::GetInstance().buttonReleaseColor);
			canvas->DrawRectangle(x, y, x + w, y + h);
			canvas->DrawFilledRectangle(x + 2, y + 2, x + w - 2, y + h - 2);
			canvas->SetPrimaryColorUint(
				spn::ui::UiScheme::GetInstance().textColor);
			canvas->DrawCString(buttonText, x + 8, y + 2);
			break;
		}

		switch (uie.eventType) {
		case spn::ui::UiEventType::MouseUp:
			if (uie.mouseButton == spn::ui::MouseButton::Left) {
				//leftbutton is up
				if (IsMouseInsideRectangle(uie.mouseX, uie.mouseY, x, y, w, h)) {
					rv = true;
					state = spn::imgui::BTN_RELEASE;
					//clear event
					uie = {};
				}
			}
			break;
		case spn::ui::UiEventType::MouseDown:
			if (uie.mouseButton == spn::ui::MouseButton::Left) {
				if (IsMouseInsideRectangle(uie.mouseX, uie.mouseY, x, y, w, h)) {
					state = spn::imgui::BTN_PRESS;
				}
			}
			break;
		case spn::ui::UiEventType::MouseMove:
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

	static AlertResult Alert(
		spn::Canvas* canvas, spn::ui::UiEvent& uie,
		const char* alertText,
		int x, int y, int w, int h,
		bool& alertStatus,
		int& okState, int& cancelState)
	{
		if (
			uie.eventType == spn::ui::UiEventType::MouseUp &&
			!IsMouseInsideRectangle(uie.mouseX, uie.mouseY, x, y, w, h)) {
			alertStatus = false;
			//clear event
			uie = {};
			return None;
		}
		int padding = 4;
		canvas->SetPrimaryColorUint(0xffffff);
		canvas->DrawFilledRectangle(x, y, x + w, y + h);
		canvas->SetPrimaryColorUint(0x0);


		int contentEndY = y + h - 40 - 2 * padding;
		float textWidth = 0, textHeight = 0;
		canvas->GetCStringDisplaySize(alertText, textWidth, textHeight);
		int centerX = x + w / 2;
		int centerY = y + (contentEndY - y) / 2;
		int textX = centerX - textWidth / 2;
		int textY = centerY - textHeight / 2;
		canvas->DrawCString(alertText, textX, textY);

		if (Button(canvas, uie, "Ok",
			x + padding,
			contentEndY + padding,
			w / 2 - 2 * padding,
			40,
			okState)) {
			alertStatus = false;
			return Ok;
		}
		else if (Button(canvas,
			uie,
			"Cancel",
			x + w / 2 + padding,
			contentEndY + padding,
			w / 2 - 2 * padding,
			40,
			cancelState)) {
			alertStatus = false;
			return Cancel;
		}
		return None;
	}


}



#endif 
