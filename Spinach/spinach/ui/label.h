#ifndef _LABEL_H_
#define _LABEL_H_

#include <iostream>
#include <functional>
#include <string>

#include <spn_canvas.h>
#include <spn_image.h>
#include <ui_scheme.h>
#include <widget.h>

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
				UiScheme::GetInstance()
				.textColor
			);
			canvas->DrawString(text, x, y);
		}
	private:
		std::string text;
		int x = 0;
		int y = 0;
	};

}

#endif 
