#ifndef TFX_COMPONENTS_SPRITE_H
#define TFX_COMPONENTS_SPRITE_H

#include "../gl/GPUTexture.h"
#include "../logic/EC.h"

#include "../core/Util.h"
#include "../math/Vector2.h"
#include "../math/Vector4.h"

#include <initializer_list>

namespace tfx {
	class Sprite : public Component {
	public:
		enum AnimationPlayMode {
			Once = 0,
			Loop,
			PingPong
		};

		Sprite();
		~Sprite();
		
		void onUpdate(float dt) override;
		void onRender(Renderer* renderer) override;

		Vector4& getClipRect() { return m_clipRect; }
		void setClipRect(const Vector4& cr) { m_clipRect = cr; }

		Vector4& getTint() { return m_tint; }
		void setTint(const Vector4& cr) { m_tint = cr; }

		Vector2& getOrigin() { return m_origin; }
		void setOrigin(const Vector2& cr) { m_origin = cr; }

		GPUTexture* getDiffuse() { return m_diffuse; }
		void setDiffuse(GPUTexture* diff) { m_diffuse = diff; }

		GPUTexture* getNormal() { return m_normal; }
		void setNormal(GPUTexture* nrm) { m_normal = nrm; }

		Component* copy() override;

		Sprite* setup(i32 rows, i32 cols);
		Sprite* addAnimation(const String& name, const std::initializer_list<u32> frames);

		void playAnimation(const String& name, u32 fps, AnimationPlayMode mode = AnimationPlayMode::Loop);

	private:
		typedef struct Anim {
			i32 frame = 0;
			float frameTime;
			
			Vec<u32> frames;
			u32 fps;
			AnimationPlayMode playMode;
			bool flip = false;
		};

		Vector4 m_clipRect, m_tint;
		Vector2 m_origin;

		Map<std::string, Anim> m_animations;
		Vec<Vector4> m_clipRects;
		String m_currentAnimation;

		GPUTexture *m_diffuse, *m_normal;
	};
}

#endif // TFX_COMPONENTS_SPRITE_H