#ifndef TFX_CORE_ENGINE_H
#define TFX_CORE_ENGINE_H

#include "Util.h"
#include "Logger.h"
#include "../logic/Game.h"
#include "../gl/Renderer.h"

#include "SDL.h"

#include <fstream>

namespace tfx {
	typedef struct EngineConfig {
		struct WinConf {
			u32 width = 640;
			u32 height = 480;
			String title = "TFX";
		} window;
		struct GameConf {
			u32 width = 640;
			u32 height = 480;
		} game;
		double fpsCap = 60.0;
	};

	class Engine {
	public:
		static int start(Game* game, EngineConfig config = EngineConfig());

		static const EngineConfig& getConfig() { return engineConfig; }
		static Game* getGame() { return game; }
		static Renderer* getRenderer() { return renderer; }
	private:
		static void mainloop();

		static EngineConfig engineConfig;
		static Game *game;

		static Renderer* renderer;

		static SDL_Window *window;
		static SDL_GLContext ctx;
	};
}

#endif // TFX_CORE_ENGINE_H