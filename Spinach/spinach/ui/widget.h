#ifndef WIDGET_H
#define WIDGET_H
#include <iostream>
#include <ui_event.h>

namespace spn {
	class Canvas;
}

namespace spn::rmgui {
	class Widget {
	protected:
		bool focus = false;
		uint32_t eventMask = 0;
		int id = -1;
		void Subscribe(UiEventType type) {
			eventMask |= static_cast<uint32_t>(type);
		}
	public:
		virtual ~Widget() = default;
		virtual void Display(spn::Canvas* canvas) = 0;
		virtual bool OnMouseDown(MouseButton button, int x, int y) { return false; };
		virtual bool OnMouseUp(MouseButton button, int x, int y) { return false; };
		virtual bool OnMouseMove(int x, int y) { return false; };
		virtual bool OnMouseDrag(int x, int y) { return false; };
		virtual bool OnMouseWheel(int x, int y) { return false; };
		virtual bool OnActionKeyDown(KeyCode kc) { return false; };
		virtual bool OnActionKeyUp(KeyCode kc) { return false; };
		virtual bool OnCharInput(char c) { return false; };

		virtual void OnFocusGained() {}
		virtual void OnFocusLost() {}
		void SetFocus(bool flag) {
			if (focus != flag) {
				focus = flag;
				if (focus) {
					OnFocusGained();
				}
				else {
					OnFocusLost();
				}
			}
		}
		bool HasFocus() const {
			return focus;
		}
		void SetId(int aId) {
			id = aId;
		}
		int GetId() {
			return id;
		}
		bool IsSubscribed(UiEventType type) const {
			return eventMask & static_cast<uint32_t>(type);
		}
	};
}

#endif