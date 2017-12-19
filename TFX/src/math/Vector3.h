#ifndef TFX_MATH_VECTOR3_H
#define TFX_MATH_VECTOR3_H

namespace tfx {
	class Vector2;
	class Vector4;
	class Matrix4;
	class Vector3 {
	public:
		Vector3() : x(0), y(0), z(0) {}
		Vector3(float v) : x(v), y(v), z(v) {}
		Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
		Vector3(const Vector2& v, float z);

		Vector3 normalized() const;
		Vector3 cross(const Vector3& other) const;

		Vector2 xy() const;

		float dot(const Vector3& other) const;
		float length() const;

		Vector4 extend(float w = 0.0f) const;

		Vector3 operator +(const Vector3& other) const;
		Vector3 operator -(const Vector3& other) const;
		Vector3 operator *(const Vector3& other) const;
		Vector3 operator *(float other) const;
		Vector3 operator -() const;

		Vector3 unproject(const Vector4& viewport,
			const Matrix4& modelView,
			const Matrix4& projection);

		float x, y, z;
	};
}

#endif // TFX_MATH_VECTOR3_H