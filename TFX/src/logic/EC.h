#ifndef TFX_LOGIC_EC_H
#define TFX_LOGIC_EC_H

#include "PoolOf.h"
#include "../gl/Renderer.h"

namespace tfx {
	class Component {
		friend class Entity;
	public:
		virtual void onStart() {}
		virtual void onUpdate(float dt) {}
		virtual void onRender(Renderer* renderer) {}
		virtual void onDestroy() {}

		Entity* getOwner() { return m_owner; }

		virtual Component* copy() = 0;

		bool isEnabled() const { return m_enabled; }
		void setEnabled(bool e) { m_enabled = e; }

	protected:
		Entity* m_owner;
		bool m_enabled = true;
	};

	class Entity final {
		friend class Scene;
	public:
		Entity();
		~Entity();

		template <typename C, typename = IsA<Component, C>>
		Entity* with() {
			C* cmp = new C();
			((Component*) cmp)->m_owner = this;

			m_components.push_back(cmp);
			return this;
		}

		template <typename C, typename = IsA<Component, C>>
		C* get() {
			for (Component* c : m_components) {
				C* cc = dynamic_cast<C*>(c);
				if (cc != nullptr) {
					return cc;
				}
			}
			return nullptr;
		}

		u32 getID() const { return m_id; }

	protected:
		u32 m_id;
		bool m_started, m_dead;
		float m_lifeTime;
		Vec<Component*> m_components;

		void init(Entity* model);

		void update(float dt);
		void render(Renderer* ren);

	};

	class Scene : public PoolOf<Entity> {
	public:
		Scene();

		Entity* create() override final {
			Entity* e = PoolOf<Entity>::create();
			e->m_started = false;
			e->m_id = m_idCounter++;
			return e;
		}

		Entity* spawn(Entity* model);

		virtual void onStart() {}
		virtual void onUpdate(float dt) {}
		virtual void onRender(Renderer*) {}
		virtual void onExit() {}

		void update(float dt);
		void render(Renderer* renderer);

		void restart();

	private:
		u32 m_idCounter;
	};
}

#endif // TFX_LOGIC_EC_H