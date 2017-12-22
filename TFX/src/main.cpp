#include <iostream>
#include <cmath>

#include "core/Engine.h"
#include "core/Input.h"
#include "core/Logger.h"

#include "gl/GPUBuffer.h"
#include "gl/GPUVertexArray.h"
#include "gl/GPUShader.h"
#include "gl/GPUTexture.h"

#include "gl/Renderer.h"
#include "gl/Font.h"

#include "components/Spatial.h"
#include "components/Sprite.h"

using namespace tfx;

class MainScene : public Scene {
public:
	void onStart() {
		ent = create()->with<Spatial>()->with<Sprite>();

		Sprite* spr = ent->get<Sprite>();
		spr->setDiffuse(new GPUTexture("star_d.png"));
		spr->setNormal(new GPUTexture("star_n.png"));

		spr->setup(3, 5);
		spr->addAnimation("default", {});
	}

	void onUpdate(float dt) {
		Spatial* esp = ent->get<Spatial>();
		esp->setPosition(Vector3(100, 100, 0));

		ent->get<Sprite>()->setOrigin(Vector2(0.5f));
	}

	void onRender(Renderer* ren) {
		ren->setAmbientColor(Vector3(0.15f));

		RendererState tmp = ren->lock();
		ren->setPosition(
			Input::getMouseX(),
			Input::getMouseY(),
			45
		);
		ren->drawLight(Vector3(1.0f), 320);
		ren->unlock(tmp);
	}

	Entity* ent;
};

int main() {
	EngineConfig conf;
	conf.window.width = 640;
	conf.window.height = 480;
	conf.game.width = 320;
	conf.game.height = 240;

	Game* game = new Game();
	game->addScene<MainScene>("main");

	return Engine::start(game, conf);
}