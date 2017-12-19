#include "Matrix4.h"

namespace tfx {
	Matrix4::Matrix4() {
		m_rows[0] = Vector4(1, 0, 0, 0);
		m_rows[1] = Vector4(0, 1, 0, 0);
		m_rows[2] = Vector4(0, 0, 1, 0);
		m_rows[3] = Vector4(0, 0, 0, 1);
	}

	Matrix4::Matrix4(float m[16])
		: Matrix4(
			m[0], m[1], m[2], m[3],
			m[4], m[5], m[6], m[7],
			m[8], m[9], m[10], m[11],
			m[12], m[13], m[14], m[15]
		)
	{}

	Matrix4::Matrix4(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33)
	{
		m_rows[0] = Vector4(m00, m01, m02, m03);
		m_rows[1] = Vector4(m10, m11, m12, m13);
		m_rows[2] = Vector4(m20, m21, m22, m23);
		m_rows[3] = Vector4(m30, m31, m32, m33);
	}
	Matrix4::Matrix4(
		const Vector4& row0,
		const Vector4& row1,
		const Vector4& row2,
		const Vector4& row3)
	{
		m_rows[0] = row0;
		m_rows[1] = row1;
		m_rows[2] = row2;
		m_rows[3] = row3;
	}

	Matrix4::Matrix4(const Matrix4& mat) {
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				m_rows[i][j] = mat.m_rows[i][j];
			}
		}
	}

	Matrix4 Matrix4::translation(float x, float y, float z) {
		return Matrix4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			   x,    y,    z, 1.0f
		);
	}

	Matrix4 Matrix4::translation(const Vector3& translation) {
		return Matrix4::translation(translation.x, translation.y, translation.z);
	}

	Matrix4 Matrix4::rotationX(float angle) {
		float c = cosf(angle), s = sinf(angle);
		return Matrix4(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f,    c,    s, 0.0f,
			0.0f,   -s,    c, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	Matrix4 Matrix4::rotationY(float angle) {
		float c = cosf(angle), s = sinf(angle);
		return Matrix4(
			   c, 0.0f,   -s, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			   s, 0.0f,    c, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	Matrix4 Matrix4::rotationZ(float angle) {
		float c = cosf(angle), s = sinf(angle);
		return Matrix4(
			   c,    s, 0.0f, 0.0f,
			  -s,    c, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	Matrix4 Matrix4::scaling(float x, float y, float z) {
		return Matrix4(
			   x, 0.0f, 0.0f, 0.0f,
			0.0f,    y, 0.0f, 0.0f,
			0.0f, 0.0f,    z, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	Matrix4 Matrix4::scaling(const Vector3& scale) {
		return Matrix4::scaling(scale.x, scale.y, scale.z);
	}

	Matrix4 Matrix4::uniformScaling(float scale) {
		return Matrix4::scaling(scale, scale, scale);
	}

	Matrix4 Matrix4::axisAngle(const Vector3& axis, float angle) {
		Vector3 ax = axis.normalized();
		float c = cosf(angle), s = sinf(angle);
		float oc = 1.0f - c;

		return Matrix4(
			oc * ax.x * ax.x + c, oc * ax.x * ax.y - ax.z * s, oc * ax.z * ax.x + ax.y * s, 0.0f,
			oc * ax.x * ax.y + ax.z * s, oc * ax.y * ax.y + c, oc * ax.y * ax.z - ax.x * s, 0.0f,
			oc * ax.z * ax.x - ax.y * s, oc * ax.y * ax.z + ax.x * s, oc * ax.z * ax.z + c, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f
		);
	}

	Matrix4 Matrix4::lookAt(const Vector3& eye, const Vector3& target, const Vector3& up) {
		Vector3 z = (target - eye).normalized();
		Vector3 x = up.cross(z).normalized();
		Vector3 y = z.cross(x).normalized();
		Vector3 e = -eye;

		return Matrix4(
				 x.x,	   y.x,		 z.x, 0.0f,
				 x.y,	   y.y,		 z.y, 0.0f,
				 x.z,	   y.z,		 z.z, 0.0f,
			x.dot(e), y.dot(e), z.dot(e), 1.0f
		);
	}

	Matrix4 Matrix4::ortho(float left, float right, float bottom, float top, float near, float far) {
		float w = right - left;
		float h = top - bottom;
		float d = far - near;
		return Matrix4(
			2.0f / w,     0.0f,      0.0f, 0.0f,
			    0.0f, 2.0f / h,      0.0f, 0.0f,
		  	    0.0f,     0.0f, -2.0f / d, 0.0f,
			-(right + left) / w, -(top + bottom) / h, -(far + near) / d, 1.0f
		);
	}

	Matrix4 Matrix4::perspective(float fov, float asp, float near, float far) {
		float cot = 1.0f / tanf(fov / 2.0f);
		float d = near - far;
		return Matrix4(
			cot / asp, 0.0f, 0.0f, 0.0f,
			0.0f, cot, 0.0f, 0.0f,
			0.0f, 0.0f, (far + near) / d, -1.0f,
			0.0f, 0.0f, (2.0f * far * near) / d, 0.0f
		);
	}

	Matrix4 Matrix4::transposed() const {
		Vector4 a = m_rows[0];
		Vector4 b = m_rows[1];
		Vector4 c = m_rows[2];
		Vector4 d = m_rows[3];
		return Matrix4(
			a.x, b.x, c.x, d.x,
			a.y, b.y, c.y, d.y,
			a.z, b.z, c.z, d.z,
			a.w, b.w, c.w, d.w
		);
	}

	Matrix4 Matrix4::inverted() const {
		Matrix4 mat = Matrix4((*this));
		float tmp[12] = { 0.0f };
		float src[16] = { 0.0f };

		// Transpose
		for (int i = 0; i < 4; i++) {
			src[i + 0] = (*this)[i][0];
			src[i + 4] = (*this)[i][1];
			src[i + 8] = (*this)[i][2];
			src[i + 12] = (*this)[i][3];
		}

		// Calculate pairs for first 8 elements (cofactors)
		tmp[0] = src[10] * src[15];
		tmp[1] = src[11] * src[14];
		tmp[2] = src[9] * src[15];
		tmp[3] = src[11] * src[13];
		tmp[4] = src[9] * src[14];
		tmp[5] = src[10] * src[13];
		tmp[6] = src[8] * src[15];
		tmp[7] = src[11] * src[12];
		tmp[8] = src[8] * src[14];
		tmp[9] = src[10] * src[12];
		tmp[10] = src[8] * src[13];
		tmp[11] = src[9] * src[12];

		// Calculate first 8 elements (cofactors)
		mat[0][0] = tmp[0] * src[5] + tmp[3] * src[6] + tmp[4] * src[7];
		mat[0][0] -= tmp[1] * src[5] + tmp[2] * src[6] + tmp[5] * src[7];
		mat[0][1] = tmp[1] * src[4] + tmp[6] * src[6] + tmp[9] * src[7];
		mat[0][1] -= tmp[0] * src[4] + tmp[7] * src[6] + tmp[8] * src[7];
		mat[0][2] = tmp[2] * src[4] + tmp[7] * src[5] + tmp[10] * src[7];
		mat[0][2] -= tmp[3] * src[4] + tmp[6] * src[5] + tmp[11] * src[7];
		mat[0][3] = tmp[5] * src[4] + tmp[8] * src[5] + tmp[11] * src[6];
		mat[0][3] -= tmp[4] * src[4] + tmp[9] * src[5] + tmp[10] * src[6];
		mat[1][0] = tmp[1] * src[1] + tmp[2] * src[2] + tmp[5] * src[3];
		mat[1][0] -= tmp[0] * src[1] + tmp[3] * src[2] + tmp[4] * src[3];
		mat[1][1] = tmp[0] * src[0] + tmp[7] * src[2] + tmp[8] * src[3];
		mat[1][1] -= tmp[1] * src[0] + tmp[6] * src[2] + tmp[9] * src[3];
		mat[1][2] = tmp[3] * src[0] + tmp[6] * src[1] + tmp[11] * src[3];
		mat[1][2] -= tmp[2] * src[0] + tmp[7] * src[1] + tmp[10] * src[3];
		mat[1][3] = tmp[4] * src[0] + tmp[9] * src[1] + tmp[10] * src[2];
		mat[1][3] -= tmp[5] * src[0] + tmp[8] * src[1] + tmp[11] * src[2];

		// Calculate pairs for second 8 elements (cofactors)
		tmp[0] = src[2] * src[7];
		tmp[1] = src[3] * src[6];
		tmp[2] = src[1] * src[7];
		tmp[3] = src[3] * src[5];
		tmp[4] = src[1] * src[6];
		tmp[5] = src[2] * src[5];
		tmp[6] = src[0] * src[7];
		tmp[7] = src[3] * src[4];
		tmp[8] = src[0] * src[6];
		tmp[9] = src[2] * src[4];
		tmp[10] = src[0] * src[5];
		tmp[11] = src[1] * src[4];

		// Calculate second 8 elements (cofactors)
		mat[2][0] = tmp[0] * src[13] + tmp[3] * src[14] + tmp[4] * src[15];
		mat[2][0] -= tmp[1] * src[13] + tmp[2] * src[14] + tmp[5] * src[15];
		mat[2][1] = tmp[1] * src[12] + tmp[6] * src[14] + tmp[9] * src[15];
		mat[2][1] -= tmp[0] * src[12] + tmp[7] * src[14] + tmp[8] * src[15];
		mat[2][2] = tmp[2] * src[12] + tmp[7] * src[13] + tmp[10] * src[15];
		mat[2][2] -= tmp[3] * src[12] + tmp[6] * src[13] + tmp[11] * src[15];
		mat[2][3] = tmp[5] * src[12] + tmp[8] * src[13] + tmp[11] * src[14];
		mat[2][3] -= tmp[4] * src[12] + tmp[9] * src[13] + tmp[10] * src[14];
		mat[3][0] = tmp[2] * src[10] + tmp[5] * src[11] + tmp[1] * src[9];
		mat[3][0] -= tmp[4] * src[11] + tmp[0] * src[9] + tmp[3] * src[10];
		mat[3][1] = tmp[8] * src[11] + tmp[0] * src[8] + tmp[7] * src[10];
		mat[3][1] -= tmp[6] * src[10] + tmp[9] * src[11] + tmp[1] * src[8];
		mat[3][2] = tmp[6] * src[9] + tmp[11] * src[11] + tmp[3] * src[8];
		mat[3][2] -= tmp[10] * src[11] + tmp[2] * src[8] + tmp[7] * src[9];
		mat[3][3] = tmp[10] * src[10] + tmp[4] * src[8] + tmp[9] * src[9];
		mat[3][3] -= tmp[8] * src[9] + tmp[11] * src[10] + tmp[5] * src[8];

		// Calculate determinant
		float det = 1.0f / (src[0] * mat[0][0] + src[1] * mat[0][1] + src[2] * mat[0][2] + src[3] * mat[0][3]);
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				mat[i][j] = mat[i][j] * det;
			}
		}
		
		return mat;
	}

	Matrix4 Matrix4::operator*(const Matrix4& mat) const {
		float d[16] = { 0.0f };
		Matrix4 ot = mat.transposed();
		for (int j = 0; j < 4; j++) {
			for (int i = 0; i < 4; i++) {
				d[i + j * 4] = m_rows[j].dot(ot.m_rows[i]);
			}
		}
		return Matrix4(d);
	}

	Vector4 Matrix4::operator*(const Vector4& vec) const {
		return Vector4(
			m_rows[0].dot(vec),
			m_rows[1].dot(vec),
			m_rows[2].dot(vec),
			m_rows[3].dot(vec)
		);
	}

	Vector3 Matrix4::operator*(const Vector3& vec) const {
		Vector4 v = vec.extend(1.0f);
		return Vector3(
			m_rows[0].dot(v),
			m_rows[1].dot(v),
			m_rows[2].dot(v)
		);
	}

	Matrix4 Matrix4::operator*(float v) const {
		return Matrix4(
			m_rows[0] * v,
			m_rows[1] * v,
			m_rows[2] * v,
			m_rows[3] * v
		);
	}

}