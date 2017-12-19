#ifndef TFX_GL_GPUSHADER_H
#define TFX_GL_GPUSHADER_H

#include "glad.h"
#include "../core/Util.h"

#include "../math/Vector2.h"
#include "../math/Vector3.h"
#include "../math/Vector4.h"
#include "../math/Matrix4.h"

#define LocBlock() if (location == -1) return;

namespace tfx {
	class GPUShader {
	public:
		class Uniform {
		public:
			Uniform() : location(-1) {}
			Uniform(i32 loc) : location(loc) {}

			void set(int v);
			void set(float v);
			void set(const Vector2& v);
			void set(const Vector3& v);
			void set(const Vector4& v);
			void set(const Matrix4& v);

		protected:
			i32 location;
		};

		enum GPUShaderType {
			VertexShader = GL_VERTEX_SHADER,
			FragmentShader = GL_FRAGMENT_SHADER
		};

		GPUShader();
		~GPUShader();

		void addSource(const String& source, GPUShaderType type);
		void link();

		void bind();
		void unbind();

		Uniform getUniform(const String& name);
		i32 getAttribLocation(const String& name);

	private:
		u32 m_bindCode;
		Map<std::string, u32> m_uniforms, m_attributes;

		i32 getUniformLocation(const String& name);

		static i32 createShader(const String& source, GPUShaderType type);
	};
}

#endif // TFX_GL_GPUSHADER_H