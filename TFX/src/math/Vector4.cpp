#include "Vector4.h"

#include <cmath>

namespace tfx {
	Vector4 Vector4::normalized() const {
		float len = length();
		return Vector4(x / len, y / len, z / len, w / len);
	}

	float Vector4::dot(const Vector4& other) const {
		return x * other.x + y * other.y + z * other.z + w * other.w;
	}

	float Vector4::length() const {
		return sqrtf(dot(*this));
	}

	Vector4 Vector4::operator+(const Vector4& other) const {
		return Vector4(x + other.x, y + other.y, z + other.z, w + other.w);
	}

	Vector4 Vector4::operator-(const Vector4& other) const {
		return Vector4(x - other.x, y - other.y, z - other.z, w - other.w);
	}

	Vector4 Vector4::operator*(const Vector4& other) const {
		return Vector4(x * other.x, y * other.y, z * other.z, w * other.w);
	}

	Vector4 Vector4::operator*(float other) const {
		return Vector4(x * other, y * other, z * other, w * other);
	}

	Vector4 Vector4::operator-() const {
		return Vector4(-x, -y, -z, -w);
	}

}