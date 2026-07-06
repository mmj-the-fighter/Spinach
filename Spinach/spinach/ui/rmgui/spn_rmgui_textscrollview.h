#ifndef SPN_RMGUI_TEXT_SCROLLVIEW_H
#define SPN_RMGUI_TEXT_SCROLLVIEW_H

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <spn_canvas.h>
#include <spn_utils.h>
#include <spn_ui_scheme.h>
#include <rmgui/spn_rmgui_widget.h>

namespace spn::rmgui {
	class TextScrollView : public Widget
	{
	public:

		TextScrollView() {
			Subscribe(spn::ui::UiEventType::ActionKeyDown);
			Subscribe(spn::ui::UiEventType::MouseDown);
		}

		void SetSize(int width, int height) {
			w = width;
			h = height;
		}

		void SetPosition(int ax, int ay) {
			x = ax;
			y = ay;
		}

		bool OnMouseDown(spn::ui::MouseButton button, int x, int y) {
			if (button == spn::ui::MouseButton::Left) {
				return OnLmbDown(x, y);
			}
			return false;
		}

		bool OnLmbDown(int mx, int my) {
			if (IsPointInsideTextScrollView(mx, my)) {
				SetFocus(true);
				isHover = true;
			}
			else {
				SetFocus(false);
				isHover = false;
			}
			return false;
		}

		bool OnActionKeyDown(spn::ui::KeyCode kc) {
			switch (kc) {
			case spn::ui::KeyCode::Down:
				return OnDownArrowKey();
			case spn::ui::KeyCode::Up:
				return OnUpArrowKey();
			}
			return false;
		}

		int Clampi(int v, int min, int max) {
			if (v < min) {
				return min;
			}
			else if (v > max) {
				return max;
			}
			else {
				return v;
			}
		}

		bool OnDownArrowKey() {
			if (!HasFocus()) {
				return  false;
			}
			lineStart = Clampi(lineStart + 1, 0, lines.size());
			return true;
		}

		bool OnUpArrowKey() {
			if (!HasFocus()) {
				return  false;
			}
			lineStart = Clampi(lineStart - 1, 0, lines.size());
			return true;
		}

		int GetLineNum() {
			return lines.size();
		}
		
		std::string& GetTextAtLine(int lineNo) {
			return lines[lineNo - 1];
		}

		std::string& GetTextAtCurrentLine() {
			return GetTextAtLine(lines.size());
		}

		void AddText(const char* str) {
			lines.push_back(std::string(str));
		}
		
		void ClearText(){
			lines.clear();
		}

		void Display(spn::Canvas* canvas) {
			canvas->SetClippingRectangle(x, y, w, h);
			auto& scheme = spn::ui::UiScheme::GetInstance();
			int cwidth = canvas->GetWidth();
			int cheight = canvas->GetHeight();
			canvas->SetPrimaryColorUint(scheme.textColor);
			float tw, th;
			if (lines.size() > 0) {
				canvas->GetCStringDisplaySize(lines[0].c_str(), tw, th);
				int k = 0;
				for (int i = lineStart; i < lines.size(); ++i, ++k) {
					canvas->DrawCString(lines[i].c_str(), x + 4, y + th * k + 2);
				}
			}
			
			if (HasFocus()) {
				canvas->SetPrimaryColorUint(scheme.buttonHoverColor);
			}
			canvas->DrawRectangle(x, y, x + w- thumbW, y + h);
			canvas->DrawRectangle(x + w - thumbW, y, x + w, y + h);
			canvas->SetClippingRectangle(0, 0, cwidth, cheight);
		}
	private:
		bool IsPointInsideTextScrollView(int mx, int my) {
			return (mx >= x) &&
				(mx <= x + w) &&
				(my >= y) &&
				(my <= y + h);
		}
		float x = 0;
		float y = 0;
		float w = 0;
		float h = 0;
		float thumbW = 16;
		bool isHover = false;
		int lineStart = 0;
		std::vector<std::string> lines;
	};

}

#endif 
