#include "Vector3.h"

#include <cmath>
#include "Vector2.h"
#include "Vector4.h"
#include "Matrix4.h"

namespace tfx {
	Vector3::Vector3(const Vector2& v, float z) : x(v.x), y(v.y), z(z) {
	}

	Vector3 Vector3::normalized() const {
		float len = length();
		return Vector3(x / len, y / len, z / len);
	}

	float Vector3::dot(const Vector3& other) const {
		return x * other.x + y * other.y + z * other.z;
	}

	Vector3 Vector3::cross(const Vector3& other) const {
		return Vector3(
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		);
	}

	inline Vector2 Vector3::xy() const { return Vector2(x, y); }

	float Vector3::length() const {
		return sqrtf(dot(*this));
	}

	Vector4 Vector3::extend(float w) const {
		return Vector4(x, y, z, w);
	}

	Vector3 Vector3::operator+(const Vector3& other) const {
		return Vector3(x + other.x, y + other.y, z + other.z);
	}

	Vector3 Vector3::operator-(const Vector3& other) const {
		return Vector3(x - other.x, y - other.y, z - other.z);
	}

	Vector3 Vector3::operator*(const Vector3& other) const {
		return Vector3(x * other.x, y * other.y, z * other.z);
	}

	Vector3 Vector3::operator*(float other) const {
		return Vector3(x * other, y * other, z * other);
	}

	Vector3 Vector3::operator-() const {
		return Vector3(-x, -y, -z);
	}

	Vector3 Vector3::unproject(const Vector4& viewport, const Matrix4& modelView, const Matrix4& projection) {
		Matrix4 invMVP = (projection * modelView).inverted();
		Vector4 in;

		in[0] = (x - (float) viewport[0]) / (float) viewport[2] * 2.0f - 1.0f;
		in[1] = (y - (float) viewport[1]) / (float) viewport[3] * 2.0f - 1.0f;
		in[2] = 2.0f * z - 1.0f;
		in[3] = 1.0f;

		Vector4 out = invMVP * in;
		out.w = 1.0f / out.w;

		return Vector3(out.x, out.y, out.z) * out.w;
	}

}