#include "Engine.h"

#include "../gl/glad.h"
#include "Input.h"

#include <algorithm>

namespace tfx {
	EngineConfig Engine::engineConfig;
	
	SDL_Window *Engine::window;
	SDL_GLContext Engine::ctx;

	Renderer* Engine::renderer = nullptr;

	Game *Engine::game = nullptr;

	int Engine::start(Game* game, EngineConfig config) {
		Engine::engineConfig = config;
		Engine::game = game;

		if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
			LogFatal(SDL_GetError());
			return 1;
		}

		SDL_GL_SetAttribute(SDL_GL_ACCELERATED_VISUAL, 1);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

		window = SDL_CreateWindow(
			config.window.title,
			SDL_WINDOWPOS_CENTERED,	SDL_WINDOWPOS_CENTERED,
			config.window.width,
			config.window.height,
			SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
		);

		LogInfo("Creating window...");
		if (!window) {
			LogFatal(String("Failed to create Window. ") + String(SDL_GetError()));
			return 1;
		}

		LogInfo("Creating OpenGL context...");
		ctx = SDL_GL_CreateContext(window);
		if (ctx == NULL) {
			LogFatal(String("Failed to create OpenGL context. ") + String(SDL_GetError()));
			return 1;
		}

		LogInfo("Loading OpenGL...");
		gladLoadGLLoader(SDL_GL_GetProcAddress);

		Log(String("Vendor: ") + String(glGetString(GL_VENDOR)));
		Log(String("Renderer: ") + String(glGetString(GL_RENDERER)));
		Log(String("Version: ") + String(glGetString(GL_VERSION)));

		LogInfo(" ==> Engine Started!");
		mainloop();

		SDL_DestroyWindow(window);
		SDL_GL_DeleteContext(ctx);

		LogInfo(" ==> Engine Finished!");
		SDL_Quit();

		return 0;
	}

	void Engine::mainloop() {
		bool running = true;

		renderer = new Renderer(engineConfig.game.width, engineConfig.game.height);

		SDL_Event event;

		double timeStep = 1.0 / engineConfig.fpsCap;
		double last = Util::getTime();
		double accum = 0.0;

		int w = engineConfig.window.width, h = engineConfig.window.height;
		float rw = (float(engineConfig.game.width) / float(w));
		float rh = (float(engineConfig.game.height) / float(h));
		
		while (running) {
			bool canRender = false;

			double current = Util::getTime();
			double delta = current - last;
			last = current;

			accum += delta;

			Input::update(rw, rh);
			running = !Input::isCloseRequested();

			while (accum >= timeStep) {
				accum -= timeStep;

				Unsafe(game) game->update(float(timeStep));

				canRender = true;
			}

			if (canRender) {
				renderer->begin();
				Unsafe(game) game->render(renderer);
				renderer->end(w, h);

				SDL_GL_SwapWindow(window);
			}
		}

		SafeDelete(game);
		SafeDelete(renderer);
	}
}