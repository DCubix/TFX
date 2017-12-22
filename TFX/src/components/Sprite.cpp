#include "Sprite.h"

#include "Spatial.h"

namespace tfx {
	Sprite::Sprite()
		:	m_clipRect(Vector4(0, 0, 1, 1)),
			m_tint(Vector4(1)),
			m_origin(Vector2(0))
	{

	}

	Sprite::~Sprite() {
		// TEMP! REMOVE THIS AFTER RESOURCE MANAGER
		SafeDelete(m_diffuse);
		SafeDelete(m_normal);
	}

	void Sprite::onUpdate(float dt) {
		if (m_currentAnimation.length() == 0) return;
		if (m_animations.empty()) return;
		if (m_clipRects.empty()) return;

		Anim& curr = m_animations[m_currentAnimation.str()];
		
		i32 maxf = curr.frames.empty() ? m_clipRects.size() : curr.frames.size();
		float delay = 1.0f / float(curr.fps);

		curr.frameTime += dt;
		if (curr.frameTime >= delay) {
			curr.frameTime = 0.0f;
			switch (curr.playMode) {
				case AnimationPlayMode::Once:
				{
					if (curr.frame++ >= maxf - 1) curr.frame = maxf - 1;
				} break;
				case AnimationPlayMode::Loop:
				{
					if (curr.frame++ >= maxf - 1) curr.frame = 0;
				} break;
				case AnimationPlayMode::PingPong:
				{
					if (!curr.flip) {
						if (++curr.frame >= maxf - 1) {
							curr.flip = true;
						}
					} else {
						if (--curr.frame <= 0) {
							curr.flip = false;
						}
					}
				} break;
			}
		}

		i32 frame = 0;
		if (curr.frames.empty()) {
			frame = curr.frame;
		} else {
			frame = curr.frames[curr.frame];
		}

		m_clipRect = m_clipRects[frame];
	}

	void Sprite::onRender(Renderer* renderer) {
		Vector3 pos = Vector3(0.0f);
		Vector2 scl = Vector2(1.0f);
		float rot = 0.0f;

		Spatial* sp = getOwner()->get<Spatial>();
		if (sp != nullptr) {
			pos = sp->getPosition();
			scl = sp->getScale();
			rot = sp->getRotation();
		}

		RendererState tmp = renderer->lock();

		renderer->setPosition(pos.x, pos.y, pos.z);
		renderer->setScale(scl);
		renderer->setRotation(rot);
		renderer->setOrigin(m_origin);
		renderer->setColor(m_tint);
		renderer->clip(m_clipRect);

		renderer->draw(m_diffuse, m_normal);

		renderer->unlock(tmp);
	}

	Component* Sprite::copy() {
		Sprite* spr = new Sprite();
		spr->m_clipRect = m_clipRect;
		spr->m_diffuse = m_diffuse;
		spr->m_normal = m_normal;
		spr->m_origin = m_origin;
		spr->m_tint = m_tint;
		return spr;
	}

	Sprite* Sprite::setup(i32 rows, i32 cols) {
		m_clipRects.clear();

		float fw = 1.0f / cols;
		float fh = 1.0f / rows;

		for (i32 i = 0; i < rows * cols; i++) {
			float x = float(i % cols) * fw;
			float y = roundf(i / cols) * fh;
			m_clipRects.push_back(Vector4(x, y, fw, fh));
		}

		return this;
	}

	Sprite* Sprite::addAnimation(const String& name, const std::initializer_list<u32> frames) {
		Anim ani;
		ani.fps = 15;
		ani.frame = 0;
		ani.frames = frames;
		ani.frameTime = 0.0f;
		ani.playMode = AnimationPlayMode::Loop;
		
		m_animations[name.str()] = ani;

		if (m_currentAnimation.length() == 0) {
			m_currentAnimation = name;
		}

		return this;
	}

	void Sprite::playAnimation(const String& name, u32 fps, AnimationPlayMode mode) {
		auto pos = m_animations.find(name.str());
		if (pos == m_animations.end()) {
			return;
		}

		m_currentAnimation = name;

		Anim& ani = m_animations[name.str()];
		ani.fps = fps;
		ani.playMode = mode;
		ani.frame = 0;
		ani.frameTime = 0.0f;
	}
}