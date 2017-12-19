#ifndef TFX_MATH_VECTOR2_H
#define TFX_MATH_VECTOR2_H

#include "Vector3.h"

namespace tfx {
	class Vector2 {
	public:
		Vector2() : x(0), y(0) {}
		Vector2(float v) : x(v), y(v) {}
		Vector2(float x, float y) : x(x), y(y) {}
		
		Vector2 rotate(float angle) const;
		Vector2 normalized() const;

		float dot(const Vector2& other) const;
		float perpendicularDot(const Vector2& other) const;
		float length() const;

		Vector3 extend(float z = 0.0f) const;

		Vector2 operator +(const Vector2& other) const;
		Vector2 operator -(const Vector2& other) const;
		Vector2 operator *(const Vector2& other) const;
		Vector2 operator *(float other) const;
		Vector2 operator -() const;

		float x, y;
	};
}

#endif // TFX_MATH_VECTOR2_H