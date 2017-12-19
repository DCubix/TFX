#include "Vector2.h"

#include <cmath>

namespace tfx {
	Vector2 Vector2::rotate(float angle) const {
		float s = sinf(angle);
		float c = cosf(angle);
		return Vector2((c * x) - (s * y), (s * x) + (c * y));
	}

	float Vector2::dot(const Vector2& other) const {
		return x * other.x + y * other.y;
	}

	float Vector2::perpendicularDot(const Vector2& other) const {
		return x * other.y - y * other.x;
	}

	float Vector2::length() const {
		return sqrtf(dot(*this));
	}

	Vector3 Vector2::extend(float z) const {
		return Vector3(x, y, z);
	}

	Vector2 Vector2::operator+(const Vector2& other) const {
		return Vector2(x + other.x, y + other.y);
	}

	Vector2 Vector2::operator-(const Vector2& other) const {
		return Vector2(x - other.x, y - other.y);
	}

	Vector2 Vector2::operator*(const Vector2& other) const {
		return Vector2(x * other.x, y * other.y);
	}

	Vector2 Vector2::operator*(float other) const {
		return Vector2(x * other, y * other);
	}

	Vector2 Vector2::operator-() const {
		return Vector2(-x, -y);
	}

	Vector2 Vector2::normalized() const {
		float len = length();
		return Vector2(x / len, y / len);
	}

}