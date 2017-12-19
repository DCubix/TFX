#ifndef TFX_MATH_VECTOR4_H
#define TFX_MATH_VECTOR4_H

#include "../core/Util.h"
#include "Vector3.h"

namespace tfx {
	class Vector4 {
	public:
		Vector4() : x(0), y(0), z(0), w(0) {}
		Vector4(float v) : x(v), y(v), z(v), w(v) {}
		Vector4(float x, float y, float z, float w) : x(x), y(y), z(z), w(w) {}

		Vector4 normalized() const;

		Vector3 xyz() const { return Vector3(x, y, z); }

		float dot(const Vector4& other) const;
		float length() const;

		Vector4 operator +(const Vector4& other) const;
		Vector4 operator -(const Vector4& other) const;
		Vector4 operator *(const Vector4& other) const;
		Vector4 operator *(float other) const;
		Vector4 operator -() const;

		const float& operator [](u32 i) const {
			switch (i) {
				case 0: return x;
				case 1: return y;
				case 2: return z;
				case 3: return w;
				default:
					return -1;
			}
		}

		float& operator [](u32 i) {
			switch (i) {
				case 0: return x;
				case 1: return y;
				case 2: return z;
				case 3: return w;
				default:
					return w;
			}
		}

		float x, y, z, w;
	};
}

#endif // TFX_MATH_VECTOR4_H