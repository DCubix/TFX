#ifndef TFX_GL_RENDERER_H
#define TFX_GL_RENDERER_H

#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"
#include "../math/Matrix4.h"

#include "GPUTexture.h"
#include "GPUShader.h"
#include "GPUBuffer.h"
#include "GPUVertexArray.h"
#include "GPUFramebuffer.h"

#include "Font.h"

#include "../core/Util.h"

#include <functional>

#define TFX_MAX_LIGHTS 50
#define TFX_LIGHT_SHADOW_SIZE 640

namespace tfx {
	typedef struct Light {
		i32 x, y, z;
		float radius, intensity;
		Vector3 color;
		bool shadow;
	};

	typedef struct Vertex {
		Vector3 position;
		Vector2 uv;
		Vector4 color;

		Vertex() {}
		Vertex(Vector3 position, Vector2 uv, Vector4 color)
			: uv(uv), position(position), color(color) {
		}
	};

	typedef struct RendererState {
		i32 x = 0;
		i32 y = 0;
		i32 z = 0;
		Vector2 scale = Vector2(1.0f);
		Vector2 origin = Vector2(0.0f);
		float rotation = 0.0f;
		Vector4 clipRect = Vector4(0, 0, 1, 1);
		Vector4 color = Vector4(1.0f);
		bool additive = false;
		bool occluder = true;

		RendererState() {}

		RendererState copy() const {
			RendererState cpy;
			cpy.x = x;
			cpy.y = y;
			cpy.z = z;
			cpy.scale = scale;
			cpy.origin = origin;
			cpy.rotation = rotation;
			cpy.clipRect = clipRect;
			cpy.color = color;
			cpy.additive = additive;
			cpy.occluder = occluder;
			return cpy;
		}
	};

	class Renderer {
	public:
		Renderer(u32 screenWidth, u32 screenHeight);
		~Renderer();

		i32 draw(Font* font, const String& str);
		void draw(GPUTexture* diffuse, GPUTexture* normal = nullptr, bool text = false);
		void drawLight(
			Vector3 color,
			float radius,
			float intensity = 1.0f,
			bool shadow = false
		);

		void begin();
		void end(int w = 0, int h = 0);

		RendererState lock();
		void unlock(const RendererState& state);

		void setPosition(i32 x, i32 y, i32 z);
		void setScale(const Vector2& scale);
		void setRotation(float rotation);

		void translate(i32 x, i32 y, i32 z);
		void scale(const Vector2& scale);
		void setOrigin(const Vector2& origin);
		void rotate(float rotation);
		void clip(const Vector4& clipRect);
		void unclip();
		void setColor(const Vector4& color);
		void setAdditive(bool v);
		void setOccluder(bool v);

		const RendererState& getState() const { return m_state; }

		Vector3 getAmbientColor() const { return m_ambientColor; }
		void setAmbientColor(const Vector3& col) { m_ambientColor = col; }

	private:
		void updateBufferData();
		
		typedef struct Drawable {
			Vertex tl, tr, bl, br;
			bool additive, occluder, defer;
			GPUTexture *diffuse, *normal;
		};
		typedef struct Batch {
			u32 offset, indices;
			bool additive, occluder, defer;
			GPUTexture *diffuse, *normal;
			float z;

			Batch() {}
			Batch(u32 offset, u32 indices, bool additive, bool occluder, GPUTexture* diffuse, GPUTexture* normal, float z, bool defer)
				: offset(offset), indices(indices), additive(additive), occluder(occluder),
					diffuse(diffuse), normal(normal), z(z), defer(defer)
			{}
		};

		void renderBatches(GPUShader* shader,
			const std::function<bool(const Batch&)>& cb = nullptr,
			const std::function<void(GPUShader*)>& setunicb = nullptr);

		void pass1();
		void pass2();
		void pass3();

		Vec<Drawable*> m_drawables;
		Vec<Light> m_lights;
		Vec<Batch> m_batches;

		Vector3 m_ambientColor;

		u32 m_vbo, m_ibo, m_vao;
		u32 m_prevVBOSize, m_prevIBOSize;

		GPUBuffer *m_quad_vbo, *m_quad_ibo;
		GPUVertexArray* m_quad_vao;

		GPUTexture* m_defaultNormal;

		GPUShader *m_fontShader, *m_defaultShader, *m_ambientShader;

		GPUShader *m_geomPass, *m_fullScreen, *m_occlusionPass, *m_shadowPass, *m_finalPass;
		GPUFramebuffer *m_gbuffer, *m_occlusion, *m_shadow, *m_final;
		GPUTexture *m_fbo_position, *m_fbo_diffuse, *m_fbo_normals, *m_fbo_occluder, *m_fbo_shadow, *m_fbo_final;

		Matrix4 m_proj, m_view;

		u32 m_screenWidth, m_screenHeight;

		RendererState m_state;
	};
}

#endif // TFX_GL_RENDERER_H