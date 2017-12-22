#ifndef TFX_LOGIC_GAME_H
#define TFX_LOGIC_GAME_H

#include "../core/Util.h"
#include "EC.h"

namespace tfx {

	class Game {
	public:
		virtual ~Game();

		template <typename T, typename = IsA<Scene, T>>
		void addScene(const String& name) {
			if (name.str().empty()) return;
			auto pos = m_scenes.find(name.str());
			if (pos == m_scenes.end())
				m_scenes.insert({ name.str(), new T() });
		}

		void setScene(const String& name);

		void update(float dt);
		void render(Renderer* renderer);

	private:
		Map<std::string, Scene*> m_scenes;
		String m_currentScene = "", m_nextScene = "";
		bool m_switchingScenes;

		Scene* currentScene();
	};
}

#endif // TFX_LOGIC_GAME_H