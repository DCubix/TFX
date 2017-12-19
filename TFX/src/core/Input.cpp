#include "Input.h"

#include "Engine.h"
#include <cmath>

static int scale_coord(int v, float ratio) {
	return (int) roundf(ratio * (float)v);
}

namespace tfx {
	SDL_Event Input::m_sdlEvent;
	std::unordered_map<int, State> Input::m_keyboard;
	std::unordered_map<int, State> Input::m_mouse;
	int Input::m_mouseX = 0;
	int Input::m_mouseY = 0;
	int Input::m_scrollOffset = 0;
	bool Input::m_closeRequested = false;

	void Input::update(float ratioX, float ratioY) {
		for (auto& kv : m_keyboard) {
			kv.second.pressed = false;
			kv.second.released = false;
		}
		for (auto& kv : m_mouse) {
			kv.second.pressed = false;
			kv.second.released = false;
		}

		while (SDL_PollEvent(&m_sdlEvent)) {
			switch (m_sdlEvent.type) {
				case SDL_QUIT: m_closeRequested = true; break;
				case SDL_KEYDOWN:
				{
					int key = m_sdlEvent.key.keysym.sym;
					m_keyboard[key].pressed = true;
					m_keyboard[key].down = true;
				} break;
				case SDL_KEYUP:
				{
					int key = m_sdlEvent.key.keysym.sym;
					m_keyboard[key].released = true;
					m_keyboard[key].down = false;
				} break;
				case SDL_MOUSEBUTTONDOWN:
				{
					int btn = m_sdlEvent.button.button;
					m_mouse[btn].pressed = true;
					m_mouse[btn].down = true;
					m_mouseX = scale_coord(m_sdlEvent.button.x, ratioX);
					m_mouseY = scale_coord(m_sdlEvent.button.y, ratioY);
				} break;
				case SDL_MOUSEBUTTONUP:
				{
					int btn = m_sdlEvent.button.button;
					m_mouse[btn].released = true;
					m_mouse[btn].down = false;
					m_mouseX = scale_coord(m_sdlEvent.button.x, ratioX);
					m_mouseY = scale_coord(m_sdlEvent.button.y, ratioY);
				} break;
				case SDL_MOUSEMOTION:
				{
					m_mouseX = scale_coord(m_sdlEvent.motion.x, ratioX);
					m_mouseY = scale_coord(m_sdlEvent.motion.y, ratioY);
				} break;
				case SDL_MOUSEWHEEL:
				{
					m_scrollOffset = m_sdlEvent.wheel.y;
				} break;
			}
		}
	}

	const State Input::getKeyboardState(int key) {
		auto pos = m_keyboard.find(key);
		if (pos != m_keyboard.end()) {
			return m_keyboard[key];
		}
		return{ false, false, false };
	}

	const State Input::getMouseState(int button) {
		auto pos = m_mouse.find(button);
		if (pos != m_mouse.end()) {
			return m_mouse[button];
		}
		return{ false, false, false };
	}

	bool Input::isKeyPressed(int key) {
		return getKeyboardState(key).pressed;
	}

	bool Input::isKeyReleased(int key) {
		return getKeyboardState(key).released;
	}

	bool Input::isKeyDown(int key) {
		return getKeyboardState(key).down;
	}

	bool Input::isMouseButtonPressed(int btn) {
		return getMouseState(btn).pressed;
	}

	bool Input::isMouseButtonReleased(int btn) {
		return getMouseState(btn).released;
	}

	bool Input::isMouseButtonDown(int btn) {
		return getMouseState(btn).down;
	}
}