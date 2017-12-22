#include "EC.h"

namespace tfx {
	Scene::Scene()
		: m_idCounter(0)
	{

	}

	Entity* Scene::spawn(Entity* model) {
		Entity* e = PoolOf<Entity>::create();
		e->m_id = m_idCounter++;
		e->init(model);
		return e;
	}

	void Scene::update(float dt) {
		onUpdate(dt);

		Vec<Entity*> deadEnts;
		for (Entity* e : getActive()) {
			e->update(dt);
			if (e->m_dead) {
				deadEnts.push_back(e);
			}
		}

		for (Entity* e : deadEnts) {
			destroy(e);
		}
	}

	void Scene::render(Renderer* renderer) {
		for (Entity* e : getActive()) {
			e->render(renderer);
		}

		onRender(renderer);
	}

	void Scene::restart() {
		reset();
		onStart();
	}
	
	Entity::Entity() : m_started(false), m_lifeTime(-1), m_dead(false) {}

	Entity::~Entity() {
		for (Component* c : m_components) {
			SafeDelete(c);
		}
		m_components.clear();
	}

	void Entity::update(float dt) {
		if (m_lifeTime == 0) return;

		if (!m_started) {
			for (Component* c : m_components) {
				if (c->m_enabled) c->onStart();
			}
			m_started = true;
		}

		for (Component* c : m_components) {
			if (c->m_enabled) c->onUpdate(dt);
		}

		if (m_lifeTime != -1) {
			m_lifeTime -= dt;
			if (m_lifeTime <= 0) {
				m_dead = true;
				for (Component* c : m_components) {
					if (c->m_enabled) c->onDestroy();
				}
			}
		}
	}

	void Entity::render(Renderer* ren) {
		if (m_lifeTime == 0) return;

		for (Component* c : m_components) {
			if (c->m_enabled) c->onRender(ren);
		}
	}

	void Entity::init(Entity* model) {
		for (Component* cmp : model->m_components) {
			Component* cpy = cmp->copy();
			cpy->m_owner = this;
			m_components.push_back(cpy);
		}
		m_started = false;
	}
}