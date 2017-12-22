#ifndef TFX_CORE_INPUT_H
#define TFX_CORE_INPUT_H

#include <unordered_map>
#include "SDL.h"

#include "../math/Vector2.h"
#include "Engine.h"

namespace tfx {
	typedef struct State {
		bool pressed, released, down;
	};

	class Input {
	public:
		static void update(float ratioX, float ratioY);

		static const State getKeyboardState(int key);
		static const State getMouseState(int button);

		static bool isKeyPressed(int key);
		static bool isKeyReleased(int key);
		static bool isKeyDown(int key);

		static bool isMouseButtonPressed(int btn);
		static bool isMouseButtonReleased(int btn);
		static bool isMouseButtonDown(int btn);

		static i32 getScrollOffset() { return m_scrollOffset; }
		static i32 getMouseX() { return m_mouseX; }
		static i32 getMouseY() { return m_mouseY; }

		static bool isCloseRequested() { return m_closeRequested; }

	private:
		static SDL_Event m_sdlEvent;
		static std::unordered_map<int, State> m_keyboard;
		static std::unordered_map<int, State> m_mouse;
		static i32 m_mouseX, m_mouseY;
		static i32 m_scrollOffset;
		static bool m_closeRequested;
	};
}

#endif // TFX_CORE_INPUT_H