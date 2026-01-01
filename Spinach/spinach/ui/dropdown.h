#ifndef _DROPDOWN_H_
#define _DROPDOWN_H_

#include <iostream>
#include <functional>
#include <string>
#include <vector>

#include "../spinach/core/spn_canvas.h"
#include "../spinach/core/spn_image.h"
#include "ui_scheme.h"
#include "widget.h"

enum {
	DD_EXPANDED,
	DD_NORMALSIZE
};

struct DRect {
	int left;
	int top;
	int right;
	int bottom;
};

class Dropdown : public Widget
{
public:
	Dropdown() {
		Subscribe(UiEventType::MouseDown);
		Subscribe(UiEventType::MouseMove);
	}
	void AddOption(std::string&& str) {
		options.push_back(str);
		OnSizeChanged();
	}
	void SetOption(int index) {
		if (index >= options.size() || index < 0) {
			return;
		}
		selectedIndex = index;
	}
	int GetOption() {
		return selectedIndex;
	}
	void SetCallback(std::function<void(int id, int selectedIndex)> onClickCallback = nullptr) {
		onClickFn = onClickCallback;
	}
	void SetSize(int width, int height) {
		w = width;
		ch = height;
		OnSizeChanged();
	}
	void SetPosition(int ax, int ay) {
		x = ax;
		y = ay;
	}

	bool OnMouseDown(MouseButton button, int x, int y) {
		if (button == MouseButton::Left) {
			return OnLmbDown(x, y);
		}
		return false;
	}

	bool OnMouseMove(int mx, int my) {
		if (state == DD_EXPANDED && IsPointInsideHoverArea(mx, my)) {
			int si = (my - hoverArea.top) / ch;
			if (si < 0 || si > options.size()) {
				return false;
			}
			hoveredIndex = si;
			SetFocus(true);
			return true;
		}
		return false;
	}

	bool OnLmbDown(int mx, int my) {
		if (IsPointInsideButtonArea(mx, my)) {
			if (state == DD_NORMALSIZE) {
				state = DD_EXPANDED;
			}
			else if (state == DD_EXPANDED) {
				state = DD_NORMALSIZE;
			}
			SetFocus(false);
			return true;
		}
		else if (IsPointInsideHoverArea(mx, my)) {
			if (state == DD_EXPANDED) {
				int si = (my - hoverArea.top) / ch;
				if (si < 0 || si > options.size()) {
					return false;
				}
				selectedIndex = si;
				state = DD_NORMALSIZE;
				if (onClickFn != nullptr) {
					onClickFn(id, selectedIndex);
				}
				SetFocus(false);
				return true;
			}
		}
		else {
			SetFocus(false);
			if (state == DD_EXPANDED) {
				state = DD_NORMALSIZE;
			}
		}
		return false;
	}
	void Display(spn::Canvas* canvas) {
		if (options.size() == 0) {
			return;
		}
		switch (state)
		{
		case DD_EXPANDED:
		{
			canvas->SetPrimaryColorUint(
				UiScheme::GetInstance().buttonReleaseColor
			);
			canvas->DrawRectangle(x, y, x + w, y + ch);
			canvas->DrawFilledRectangle(x + 2, y + 2, x + w - 2, y + ch - 2);
			canvas->SetPrimaryColorUint(
				UiScheme::GetInstance()
				.textColor
			);
			canvas->DrawString(options[selectedIndex], x + 12, y + 1);
			int i = 1;
			for (const auto& opt : options) {
				if (i - 1 == hoveredIndex) {
					canvas->SetPrimaryColorUint(
						UiScheme::GetInstance()
						.buttonHoverColor
					);
					canvas->DrawRectangle(x, y + i * ch, x + w, y + (i + 1) * ch);
					canvas->DrawFilledRectangle(x, y + i * ch, x + w, y + (i + 1) * ch);
					canvas->SetPrimaryColorUint(
						UiScheme::GetInstance()
						.textColor
					);
					canvas->DrawString(options[hoveredIndex], x + 12, y + (i * ch) + 1);
				}
				else {
					canvas->SetPrimaryColorUint(
						UiScheme::GetInstance()
						.buttonReleaseColor
					);
					canvas->DrawRectangle(x, y + i * ch, x + w, y + (i + 1) * ch);
					canvas->DrawFilledRectangle(x, y + i * ch, x + w, y + (i + 1) * ch);
					canvas->SetPrimaryColorUint(
						UiScheme::GetInstance()
						.textColor
					);
					canvas->DrawString(options[i - 1], x + 12, y + (i * ch) + 1);
				}
				++i;
			}
		}
		break;
		case DD_NORMALSIZE:
			canvas->SetPrimaryColorUint(
				UiScheme::GetInstance()
				.buttonReleaseColor
			);
			canvas->DrawRectangle(x, y, x + w, y + ch);
			canvas->DrawFilledRectangle(x + 2, y + 2, x + w - 2, y + ch - 2);
			canvas->SetPrimaryColorUint(
				UiScheme::GetInstance()
				.textColor
			);
			canvas->DrawCString(options[selectedIndex].c_str(), x + 12, y + 1);
			break;
		}
		//nook
		canvas->SetPrimaryColorUint(
			UiScheme::GetInstance()
			.textColor
		);
		canvas->DrawLine(x + w - ch + 2, y + ch / 3, x + w - ch / 2, y + ch / 2);
		canvas->DrawLine(x + w - ch / 2, y + ch / 2, x + w - 2, y + ch / 3);
	}
private:
	void OnSizeChanged() {
		//h = options.size() * ch;
		hoverArea.left = x;
		hoverArea.top = y + ch;
		hoverArea.right = x + w;
		hoverArea.bottom = y + ch + options.size() * ch;
	}

	bool IsPointInsideHoverArea(int mx, int my) {
		return (mx >= hoverArea.left) &&
			(mx <= hoverArea.right) &&
			(my >= hoverArea.top) &&
			(my <= hoverArea.bottom);
	}

	bool IsPointInsideButtonArea(int mx, int my) {
		return (mx >= x) &&
			(mx <= x + w) &&
			(my >= y) &&
			(my <= y + ch);
	}

	std::function<void(int id, int selectedIndex)> onClickFn;
	int x = 0;
	int y = 0;
	int w = 0;
	int ch = 0;
	//int h = 0;
	std::vector<std::string> options;
	int selectedIndex = 0;
	int hoveredIndex = 0;
	int state = DD_NORMALSIZE;
	DRect hoverArea;
};





#endif 
