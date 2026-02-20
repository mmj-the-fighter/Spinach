#ifndef IMGUI_CHECKBOX_H
#define IMGUI_CHECKBOX_H

#include <iostream>
#include <functional>

#include <spn_canvas.h>
#include <spn_image.h>
#include <ui_scheme.h>
#include <ui_event.h>
		
namespace spn::imgui {
	
	static bool IsMouseInsideSquare(int mx, int my, int x, int y, int sqSize) {
		return (mx >= x) && (mx <= x + sqSize) && (my >= y) && (my <= y + sqSize);
	}
	
	static bool ImGuiCheckbox(spn::Canvas* canvas, spn::rmgui::UiEvent& uie, const char* labelText, int x, int y, bool &isChecked) {
		using namespace spn::rmgui;
		int sqSize = UiScheme::GetInstance().checkboxSize;
		unsigned int cbColor = UiScheme::GetInstance().checkboxColor;
		unsigned int cbFillColor = UiScheme::GetInstance().checkboxFillColor;
		unsigned int textColor = UiScheme::GetInstance().textColor;
		if ((uie.eventType == UiEventType::MouseUp) && 
			IsMouseInsideSquare(uie.mouseX, uie.mouseY, x, y, sqSize)) {
			isChecked = !isChecked;
			//clear event
			uie = {};
		}

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
		return isChecked;
	}
}


#endif 
