#include "GPUShader.h"

#include "../core/Logger.h"

namespace tfx {
	GPUShader::GPUShader() {
		m_bindCode = glCreateProgram();
	}

	GPUShader::~GPUShader() {
		if (m_bindCode) glDeleteProgram(m_bindCode);
	}

	void GPUShader::addSource(const String& source, GPUShaderType type) {
		i32 shader = createShader(source, type);
		if (shader != -1) {
			glAttachShader(m_bindCode, shader);
			glDeleteShader(shader);
		}
	}

	void GPUShader::link() {
		glLinkProgram(m_bindCode);
	}

	void GPUShader::bind() {
		glUseProgram(m_bindCode);
	}

	void GPUShader::unbind() {
		glUseProgram(0);
	}

	GPUShader::Uniform GPUShader::getUniform(const String& name) {
		i32 loc = getUniformLocation(name);
		return Uniform(loc);
	}

	i32 GPUShader::getAttribLocation(const String& name) {
		auto pos = m_attributes.find(name.str());
		if (pos == m_attributes.end()) {
			i32 loc = glGetAttribLocation(m_bindCode, name);
			if (loc != -1) {
				m_attributes.emplace(name.str(), loc);
			} else {
				LogWarning(String("Attribute not found: ") + name);
			}
		}
		return m_attributes[name.str()];
	}

	i32 GPUShader::getUniformLocation(const String& name) {
		auto pos = m_uniforms.find(name.str());
		if (pos == m_uniforms.end()) {
			i32 loc = glGetUniformLocation(m_bindCode, name.str().c_str());
			if (loc != -1) {
				m_uniforms.emplace(name.str(), loc);
				LogInfo(String("New uniform found: ") + name + String(" => ") + String(loc));
			} else {
				LogWarning(String("Uniform not found: ") + name);
			}
		}
		return m_uniforms[name.str()];
	}

	i32 GPUShader::createShader(const String& source, GPUShaderType type) {
		u32 shader = glCreateShader(type);

		const char* src = source.str().c_str();
		glShaderSource(shader, 1, (const GLchar**) &src, NULL);
		glCompileShader(shader);

		i32 result;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
		if (result == GL_FALSE) {
			i32 logLen;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLen);

			Vec<char> log(logLen);
			glGetShaderInfoLog(shader, logLen, NULL, log.data());

			LogError(std::string(log.begin(), log.end()));

			glDeleteShader(shader);
			return -1;
		}

		return shader;
	}

	void GPUShader::Uniform::set(int v) {
		LocBlock();
		glUniform1i(location, v);
	}

	void GPUShader::Uniform::set(float v) {
		LocBlock();
		glUniform1f(location, v);
	}

	void GPUShader::Uniform::set(const Vector2& v) {
		LocBlock();
		glUniform2f(location, v.x, v.y);
	}

	void GPUShader::Uniform::set(const Vector3& v) {
		LocBlock();
		glUniform3f(location, v.x, v.y, v.z);
	}

	void GPUShader::Uniform::set(const Vector4& v) {
		LocBlock();
		glUniform4f(location, v.x, v.y, v.z, v.w);
	}

	void GPUShader::Uniform::set(const Matrix4& v) {
		LocBlock();
		glUniformMatrix4fv(location, 1, false, v.ptr());
	}

}