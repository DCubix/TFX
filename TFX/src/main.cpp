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

using namespace tfx;

class TestGame : public IGame {
public:
	void onConfigure(GameConfig& conf) {
		conf.gameWidth = 640;
		conf.gameHeight = 480;
	}

	void onStart() override {
		tex = new GPUTexture("teapot_d.dds");
		tex_n = new GPUTexture("teapot_n.dds");
		bg = new GPUTexture("bg.dds");

		fnt = new Font("corbel.png");
	}

	void onUpdate(float dt) override {
		t += dt;
	}

	void onRender(Renderer* ren) override {
		glClear(GL_COLOR_BUFFER_BIT);

		float x1 = (sinf(t) * 0.5f + 0.5f) * 640.0f;
		float x2 = (cosf(t) * 0.5f + 0.5f) * 640.0f;

		ren->setAmbientColor(Vector3(0.2f));

		ren->setPosition(x1, 240, 150);
		ren->drawLight(Vector3(0.1f, 0.5f, 1.0f), 320, 1.0f, true);

		ren->setPosition(x2, 240, 150);
		ren->drawLight(Vector3(1.0f, 0.5f, 0.1f), 320, 1.0f, true);

		RendererState tmp = ren->lock();
		for (int x = 0; x < 640; x += 256) {
			for (int y = 0; y < 480; y += 256) {
				ren->setPosition(x, y, 2);
				ren->setOccluder(true);
				ren->draw(tex, tex_n);
			}
		}
		ren->unlock(tmp);

		ren->setPosition(0, 0, 0);
		ren->setOccluder(false);
		ren->draw(bg);

		RendererState tmp2 = ren->lock();
		ren->setPosition(20, 50, 2);
		ren->setScale(Vector2(0.4f));
		ren->draw(fnt, "Hello World!\nJust some text rendering...");
		ren->unlock(tmp2);
	}

	void onExit() override {
		delete tex;
		delete tex_n;
		delete bg;
		delete fnt;
	}

	float t = 0.0f;

	GPUTexture *tex, *tex_n, *bg;
	Font* fnt;
};

int main() {
	EngineConfig conf;
	conf.window.width = 640;
	conf.window.height = 480;
	return Engine::start(new TestGame(), conf);
}