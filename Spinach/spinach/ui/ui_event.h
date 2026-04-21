#ifndef _UI_EVENT_H_
#define _UI_EVENT_H_

namespace spn::rmgui {
	enum class UiEventType : uint32_t
	{
		None = 0,

		MouseDown = 1 << 0,
		MouseUp = 1 << 1,
		MouseMove = 1 << 2,
		MouseDrag = 1 << 3,
		MouseWheel = 1 << 4,

		ActionKeyDown = 1 << 5,
		ActionKeyUp = 1 << 6,
		CharInput = 1 << 7
	};


	enum class KeyCode
	{
		Unknown,
		Left,
		Right,
		Up,
		Down,
		Backspace,
		Delete,
		Enter,
		Escape
	};

	enum class MouseButton
	{
		Unknown,
		Left,
		Right,
		Middle
	};

	struct UiEvent
	{
		UiEventType eventType;
		KeyCode keyCode;
		MouseButton mouseButton;
		int mouseX;
		int mouseY;
		char character;
	};

}



#endif
