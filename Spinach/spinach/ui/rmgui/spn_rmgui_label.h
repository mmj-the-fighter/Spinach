#ifndef SPN_RMGUI_LABEL_H
#define SPN_RMGUI_LABEL_H

#include <iostream>
#include <functional>
#include <string>

#include <spn_canvas.h>
#include <spn_image.h>
#include <spn_ui_scheme.h>
#include <rmgui/spn_rmgui_widget.h>

namespace spn::rmgui {
	class Label : public Widget
	{
	public:
		void SetId(int aId) {
			id = aId;
		}
		int GetId() {
			return id;
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
		void Display(spn::Canvas* canvas) {
			canvas->SetPrimaryColorUint(
				spn::ui::UiScheme::GetInstance()
				.textColor
			);
			canvas->DrawCString(text.c_str(), x, y);
		}
	private:
		std::string text;
		int x = 0;
		int y = 0;
	};

}

#endif 
