#include "Game.h"

namespace tfx {
	Game::~Game() {
		for (auto& kv : m_scenes) {
			delete kv.second;
		}
		m_scenes.clear();
	}

	inline void Game::setScene(const String& name) {
		m_nextScene = name;
		m_switchingScenes = true;
	}

	void Game::update(float dt) {
		if (m_nextScene.length() == 0 && !m_scenes.empty()) {
			m_nextScene = m_scenes.begin()->first;
			m_switchingScenes = true;
		}

		if (m_switchingScenes) {
			if (m_currentScene.length() > 0) {
				m_scenes[m_currentScene]->onExit();
			}

			m_currentScene = m_nextScene;

			Scene* scn = currentScene();
			Unsafe(scn) scn->onStart();

			m_switchingScenes = false;
		}

		Scene* scn = currentScene();
		Unsafe(scn) scn->update(dt);
	}

	void Game::render(Renderer* renderer) {
		if (!m_switchingScenes) {
			Scene* scn = currentScene();
			Unsafe(scn) scn->render(renderer);
		}
	}

	Scene* Game::currentScene() {
		if (m_currentScene.length() == 0) return nullptr;
		if (m_scenes.empty()) return nullptr;

		auto pos = m_scenes.find(m_currentScene);
		if (pos == m_scenes.end()) return nullptr;

		return m_scenes[m_currentScene];
	}

}