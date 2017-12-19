#ifndef TFX_MATH_MAT4_H
#define TFX_MATH_MAT4_H

#include "Vector3.h"
#include "Vector4.h"

namespace tfx {
	class Matrix4 {
	public:
		Matrix4();
		Matrix4(float m[16]);
		Matrix4(float m00, float m01, float m02, float m03,
			 float m10, float m11, float m12, float m13,
			 float m20, float m21, float m22, float m23,
			 float m30, float m31, float m32, float m33);
		Matrix4(const Vector4& row0,
			const Vector4& row1,
			const Vector4& row2,
			const Vector4& row3);
		Matrix4(const Matrix4& mat);

		static Matrix4 translation(float x, float y, float z);
		static Matrix4 translation(const Vector3& translation);
		static Matrix4 rotationX(float angle);
		static Matrix4 rotationY(float angle);
		static Matrix4 rotationZ(float angle);
		static Matrix4 scaling(float x, float y, float z);
		static Matrix4 scaling(const Vector3& scale);
		static Matrix4 uniformScaling(float scale);
		static Matrix4 axisAngle(const Vector3& axis, float angle);
		static Matrix4 lookAt(const Vector3& eye, const Vector3& target, const Vector3& up);
		static Matrix4 ortho(float left, float right, float bottom, float top, float near, float far);
		static Matrix4 perspective(float fov, float aspect, float near, float far);

		Matrix4 transposed() const;
		Matrix4 inverted() const;

		const float* ptr() const { return &m_rows[0][0]; }

		const Vector4& operator [](u32 i) const {
			return m_rows[i];
		}

		Vector4& operator [](u32 i) {
			return m_rows[i];
		}

		Matrix4 operator *(const Matrix4& mat) const;
		Vector4 operator *(const Vector4& vec) const;
		Vector3 operator *(const Vector3& vec) const;
		Matrix4 operator *(float v) const;

	private:
		Vector4 m_rows[4];
	};
}

#endif // TFX_MATH_MAT4_H