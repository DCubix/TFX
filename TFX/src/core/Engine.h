#ifndef TFX_CORE_ENGINE_H
#define TFX_CORE_ENGINE_H

#include "Util.h"
#include "Logger.h"
#include "IGame.h"
#include "../gl/Renderer.h"

#include "SDL.h"

#include <fstream>

namespace tfx {
	typedef struct EngineConfig {
		struct WindowConfig {
			bool fullScreen = false;
			u32 width = 640;
			u32 height = 480;
			String title = "TFX";
		} window;
		double fpsCap = 60.0;
	};

	class Engine {
	public:
		static int start(IGame* game, EngineConfig config = EngineConfig());

		static const EngineConfig& getConfig() { return engineConfig; }
		static IGame* getGame() { return game; }
		static Renderer* getRenderer() { return renderer; }
	private:
		static void mainloop();

		static EngineConfig engineConfig;
		static IGame *game;

		static Renderer* renderer;

		static SDL_Window *window;
		static SDL_GLContext ctx;
	};
}

#endif // TFX_CORE_ENGINE_H