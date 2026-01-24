#ifndef UI_EVENT_TRANSLATOR
#define UI_EVENT_TRANSLATOR
#include <iostream>
#include <SDL3/SDL.h>
#include "ui_event.h"

namespace spn::rmgui {
	static void ClearUiEvent(UiEvent& uie) {
		uie.eventType = UiEventType::None;
		uie.mouseButton = MouseButton::Unknown;
		uie.character = 0;
		uie.keyCode = KeyCode::Unknown;
		uie.mouseButton = MouseButton::Unknown;
		uie.mouseX = 0;
		uie.mouseY = 0;
	}
	static void TranslateSdlEvent(const SDL_Event* e, UiEvent& uie) {
		switch (e->type) {
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			if (e->button.button == SDL_BUTTON_LEFT) {
				uie.eventType = UiEventType::MouseDown;
				uie.mouseButton = MouseButton::Left;
				uie.mouseX = e->button.x;
				uie.mouseY = e->button.y;
			}
			else if (e->button.button == SDL_BUTTON_RIGHT) {
				uie.eventType = UiEventType::MouseDown;
				uie.mouseButton = MouseButton::Right;
				uie.mouseX = e->button.x;
				uie.mouseY = e->button.y;
			}
			break;
		case SDL_EVENT_MOUSE_MOTION:
			if (e->motion.state & SDL_BUTTON_LMASK) {
				uie.eventType = UiEventType::MouseDrag;
				uie.mouseButton = MouseButton::Left;
				uie.mouseX = e->motion.x;
				uie.mouseY = e->motion.y;
			}
			else {
				uie.eventType = UiEventType::MouseMove;
				uie.mouseX = e->motion.x;
				uie.mouseY = e->motion.y;
			}
			break;
		case SDL_EVENT_MOUSE_BUTTON_UP:
			if (e->button.button == SDL_BUTTON_LEFT) {
				uie.eventType = UiEventType::MouseUp;
				uie.mouseButton = MouseButton::Left;
				uie.mouseX = e->button.x;
				uie.mouseY = e->button.y;
			}
			else if (e->button.button == SDL_BUTTON_RIGHT) {
				uie.eventType = UiEventType::MouseUp;
				uie.mouseButton = MouseButton::Right;
				uie.mouseX = e->button.x;
				uie.mouseY = e->button.y;
			}
			break;

		case SDL_EVENT_TEXT_INPUT:
			uie.eventType = UiEventType::CharInput;
			uie.character = e->text.text[0];
			break;

		case SDL_EVENT_KEY_DOWN:
			uie.eventType = UiEventType::ActionKeyDown;
			uie.keyCode = KeyCode::Unknown;
			switch (e->key.key)
			{
			case SDLK_BACKSPACE:
				uie.keyCode = KeyCode::Backspace;
				break;
			case SDLK_LEFT:
				uie.keyCode = KeyCode::Left;
				break;
			case SDLK_RIGHT:
				uie.keyCode = KeyCode::Right;
				break;
			case SDLK_UP:
				uie.keyCode = KeyCode::Up;
				break;
			case SDLK_DOWN:
				uie.keyCode = KeyCode::Down;
				break;
			}
			break;
		case SDL_EVENT_KEY_UP:
			uie.eventType = UiEventType::ActionKeyUp;
			uie.keyCode = KeyCode::Unknown;
			switch (e->key.key)
			{
			case SDLK_BACKSPACE:
				uie.keyCode = KeyCode::Backspace;
				break;
			case SDLK_LEFT:
				uie.keyCode = KeyCode::Left;
				break;
			case SDLK_RIGHT:
				uie.keyCode = KeyCode::Right;
				break;
			case SDLK_UP:
				uie.keyCode = KeyCode::Up;
				break;
			case SDLK_DOWN:
				uie.keyCode = KeyCode::Down;
				break;
			}
			break;
		}
	}
}

#endif 