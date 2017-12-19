#ifndef TFX_CORE_IGAME_H
#define TFX_CORE_IGAME_H

namespace tfx {

	typedef struct GameConfig {
		u32 gameWidth = 640;
		u32 gameHeight = 480;
	};

	class Renderer;
	class IGame {
	public:
		virtual void onConfigure(GameConfig&) {} // Called before initializing the graphics
		virtual void onStart() {}	// Called before starting the main loop.
									// Load your resources here.
		virtual void onRender(Renderer*) {} // Game rendering
		virtual void onUpdate(float) {}

		virtual void onExit() {}	// Called after the loop ends. Free the resources here!
	};
}

#endif // TFX_CORE_IGAME_H