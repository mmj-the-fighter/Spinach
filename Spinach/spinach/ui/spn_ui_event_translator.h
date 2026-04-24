#ifndef SPN_UI_EVENT_TRANSLATOR
#define SPN_UI_EVENT_TRANSLATOR
#include <iostream>
#include <SDL3/SDL.h>
#include <spn_ui_event.h>

namespace spn::ui {
	static void ClearUiEvent(spn::ui::UiEvent& uie) {
		uie.eventType = spn::ui::UiEventType::None;
		uie.mouseButton = spn::ui::MouseButton::Unknown;
		uie.character = 0;
		uie.keyCode = spn::ui::KeyCode::Unknown;
		uie.mouseButton = spn::ui::MouseButton::Unknown;
		uie.mouseX = 0;
		uie.mouseY = 0;
	}
	static void TranslateSdlEvent(const SDL_Event* e, spn::ui::UiEvent& uie) {
		switch (e->type) {
		case SDL_EVENT_MOUSE_BUTTON_DOWN:
			if (e->button.button == SDL_BUTTON_LEFT) {
				uie.eventType = spn::ui::UiEventType::MouseDown;
				uie.mouseButton = spn::ui::MouseButton::Left;
				uie.mouseX = e->button.x;
				uie.mouseY = e->button.y;
			}
			else if (e->button.button == SDL_BUTTON_RIGHT) {
				uie.eventType = spn::ui::UiEventType::MouseDown;
				uie.mouseButton = spn::ui::MouseButton::Right;
				uie.mouseX = e->button.x;
				uie.mouseY = e->button.y;
			}
			break;
		case SDL_EVENT_MOUSE_MOTION:
			if (e->motion.state & SDL_BUTTON_LMASK) {
				uie.eventType = spn::ui::UiEventType::MouseDrag;
				uie.mouseButton = spn::ui::MouseButton::Left;
				uie.mouseX = e->motion.x;
				uie.mouseY = e->motion.y;
			}
			else {
				if (e->motion.state & SDL_BUTTON_RMASK) {
					uie.mouseButton = spn::ui::MouseButton::Right;
				}
				uie.eventType = spn::ui::UiEventType::MouseMove;
				uie.mouseX = e->motion.x;
				uie.mouseY = e->motion.y;
			}
			
			break;
		case SDL_EVENT_MOUSE_BUTTON_UP:
			if (e->button.button == SDL_BUTTON_LEFT) {
				uie.eventType = spn::ui::UiEventType::MouseUp;
				uie.mouseButton = spn::ui::MouseButton::Left;
				uie.mouseX = e->button.x;
				uie.mouseY = e->button.y;
			}
			else if (e->button.button == SDL_BUTTON_RIGHT) {
				uie.eventType = spn::ui::UiEventType::MouseUp;
				uie.mouseButton = spn::ui::MouseButton::Right;
				uie.mouseX = e->button.x;
				uie.mouseY = e->button.y;
			}
			break;

		case SDL_EVENT_TEXT_INPUT:
			uie.eventType = spn::ui::UiEventType::CharInput;
			uie.character = e->text.text[0];
			break;

		case SDL_EVENT_KEY_DOWN:
			uie.eventType = spn::ui::UiEventType::ActionKeyDown;
			uie.keyCode = spn::ui::KeyCode::Unknown;
			switch (e->key.key)
			{
			case SDLK_BACKSPACE:
				uie.keyCode = spn::ui::KeyCode::Backspace;
				break;
			case SDLK_LEFT:
				uie.keyCode = spn::ui::KeyCode::Left;
				break;
			case SDLK_RIGHT:
				uie.keyCode = spn::ui::KeyCode::Right;
				break;
			case SDLK_UP:
				uie.keyCode = spn::ui::KeyCode::Up;
				break;
			case SDLK_DOWN:
				uie.keyCode = spn::ui::KeyCode::Down;
				break;
			}
			break;
		case SDL_EVENT_KEY_UP:
			uie.eventType = spn::ui::UiEventType::ActionKeyUp;
			uie.keyCode = spn::ui::KeyCode::Unknown;
			switch (e->key.key)
			{
			case SDLK_BACKSPACE:
				uie.keyCode = spn::ui::KeyCode::Backspace;
				break;
			case SDLK_LEFT:
				uie.keyCode = spn::ui::KeyCode::Left;
				break;
			case SDLK_RIGHT:
				uie.keyCode = spn::ui::KeyCode::Right;
				break;
			case SDLK_UP:
				uie.keyCode = spn::ui::KeyCode::Up;
				break;
			case SDLK_DOWN:
				uie.keyCode = spn::ui::KeyCode::Down;
				break;
			}
			break;
		}
	}
}

#endif 