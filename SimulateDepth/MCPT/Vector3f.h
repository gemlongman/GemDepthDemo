#pragma once

#include "pch.h"

class Vector3f
{
public:
	union {
		struct { float x, y, z; };
		struct { float r, g, b; };
		float num[3];
	};

	Vector3f() :x(0.0f), y(0.0f), z(0.0f) {};
	Vector3f(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	inline Vector3f operator - () { return Vector3f(-x, -y, -z); }
	Vector3f flip() { return Vector3f(-x, -y, -z); };

	inline Vector3f& operator = (const Vector3f &v) { x = v.x; y = v.y; z = v.z; return *this; }

	inline Vector3f& operator += (float num) { x += num; y += num; z += num; return *this; }
	inline Vector3f& operator += (const Vector3f &v) { x += v.x; y += v.y; z += v.z; return *this; }

	inline Vector3f& operator -= (float num) { x -= num; y -= num; z -= num; return *this; }
	inline Vector3f& operator -= (const Vector3f &v) { x -= v.x; y -= v.y; z -= v.z; return *this; }

	inline Vector3f& operator *= (float num) { x *= num; y *= num; z *= num; return *this; }
	inline Vector3f& operator *= (const Vector3f &v) { x *= v.x; y *= v.y; z *= v.z; return *this; }

	inline Vector3f& operator /= (float num) { x /= num; y /= num; z /= num; return *this; }
	inline Vector3f& operator /= (const Vector3f &v) { x /= v.x; y /= v.y; z /= v.z; return *this; }


	friend inline Vector3f operator + (const Vector3f &u, float num) { return Vector3f(u.x + num, u.y + num, u.z + num); }
	friend inline Vector3f operator + (float num, const Vector3f &u) { return Vector3f(num + u.x, num + u.y, num + u.z); }
	friend inline Vector3f operator + (const Vector3f &u, const Vector3f &v) { return Vector3f(u.x + v.x, u.y + v.y, u.z + v.z); }
	friend inline Vector3f operator - (const Vector3f &u, float num) { return Vector3f(u.x - num, u.y - num, u.z - num); }
	friend inline Vector3f operator - (float num, const Vector3f &u) { return Vector3f(num - u.x, num - u.y, num - u.z); }
	friend inline Vector3f operator - (const Vector3f &u, const Vector3f &v) { return Vector3f(u.x - v.x, u.y - v.y, u.z - v.z); }
	friend inline Vector3f operator * (const Vector3f &u, float num) { return Vector3f(u.x * num, u.y * num, u.z * num); }
	friend inline Vector3f operator * (float num, const Vector3f &u) { return Vector3f(num * u.x, num * u.y, num * u.z); }
	friend inline Vector3f operator * (const Vector3f &u, const Vector3f &v) { return Vector3f(u.x * v.x, u.y * v.y, u.z * v.z); }
	friend inline Vector3f operator / (const Vector3f &u, float num) { return Vector3f(u.x / num, u.y / num, u.z / num); }
	friend inline Vector3f operator / (float num, const Vector3f &u) { return Vector3f(num / u.x, num / u.y, num / u.z); }
	friend inline Vector3f operator / (const Vector3f &u, const Vector3f &v) { return Vector3f(u.x / v.x, u.y / v.y, u.z / v.z); }

	inline bool operator == (const Vector3f &u) { return isEqualf(x, u.x) && isEqualf(y, u.y) && isEqualf(z, u.z); }
	inline bool operator != (const Vector3f &u) { return !(isEqualf(x, u.x) && isEqualf(y, u.y) && isEqualf(z, u.z)); }

	friend inline Vector3f cross(const Vector3f &u, const Vector3f &v)
	{
		return Vector3f(u.y * v.z - u.z * v.y, u.z * v.x - u.x * v.z, u.x * v.y - u.y * v.x);
	}

	friend inline float dot(const Vector3f &u, const Vector3f &v)
	{
		return u.x * v.x + u.y * v.y + u.z * v.z;
	}

	friend inline float length(const Vector3f &u)
	{
		return sqrt(u.x * u.x + u.y * u.y + u.z * u.z);
	}

	friend inline Vector3f normalize(const Vector3f &u)
	{
		return u * (1.0f / sqrt(u.x * u.x + u.y * u.y + u.z * u.z));
	}
};

class Vector3f;
typedef Vector3f Color3f;
typedef Vector3f Point3f;



//4¡Á4
class Matrix4
{
public:
	float mat[16];
	Matrix4()
	{
		mat[0] = 1.0; mat[4] = 0.0; mat[8] = 0.0; mat[12] = 0.0;
		mat[1] = 0.0; mat[5] = 1.0; mat[9] = 0.0; mat[13] = 0.0;
		mat[2] = 0.0; mat[6] = 0.0; mat[10] = 1.0; mat[14] = 0.0;
		mat[3] = 0.0; mat[7] = 0.0; mat[11] = 0.0; mat[15] = 1.0;
	}
	Matrix4(const Vector3f & v0, const Vector3f & v1, const Vector3f & v2)
	{
		mat[0] = v0.x; mat[4] = v1.x; mat[8] = v2.x; mat[12] = 0.0f;
		mat[1] = v0.y; mat[5] = v1.y; mat[9] = v2.y; mat[13] = 0.0f;
		mat[2] = v0.z; mat[6] = v1.z; mat[10] = v2.z; mat[14] = 0.0f;
		mat[3] = 0.0f; mat[7] = 0.0f; mat[11] = 0.0f; mat[15] = 1.0f;
	}
	~Matrix4() { }
	Matrix4(const Matrix4 & Matrix4)
	{
		for (int i = 0; i < 16; i++) {
			mat[i] = Matrix4.mat[i];
		}
	}
	Matrix4 transpose()
	{
		Matrix4 Matrix;
		Matrix[0] = mat[0];
		Matrix[1] = mat[4];
		Matrix[2] = mat[8];
		Matrix[3] = mat[12];
		Matrix[4] = mat[1];
		Matrix[5] = mat[5];
		Matrix[6] = mat[9];
		Matrix[7] = mat[13];
		Matrix[8] = mat[2];
		Matrix[9] = mat[6];
		Matrix[10] = mat[10];
		Matrix[11] = mat[14];
		Matrix[12] = mat[3];
		Matrix[13] = mat[7];
		Matrix[14] = mat[11];
		Matrix[15] = mat[15];
		return Matrix;
	}

	bool inverse()
	{
		float inv[16], det;
		int i;

		inv[0] = mat[5] * mat[10] * mat[15] -
			mat[5] * mat[11] * mat[14] -
			mat[9] * mat[6] * mat[15] +
			mat[9] * mat[7] * mat[14] +
			mat[13] * mat[6] * mat[11] -
			mat[13] * mat[7] * mat[10];

		inv[4] = -mat[4] * mat[10] * mat[15] +
			mat[4] * mat[11] * mat[14] +
			mat[8] * mat[6] * mat[15] -
			mat[8] * mat[7] * mat[14] -
			mat[12] * mat[6] * mat[11] +
			mat[12] * mat[7] * mat[10];

		inv[8] = mat[4] * mat[9] * mat[15] -
			mat[4] * mat[11] * mat[13] -
			mat[8] * mat[5] * mat[15] +
			mat[8] * mat[7] * mat[13] +
			mat[12] * mat[5] * mat[11] -
			mat[12] * mat[7] * mat[9];

		inv[12] = -mat[4] * mat[9] * mat[14] +
			mat[4] * mat[10] * mat[13] +
			mat[8] * mat[5] * mat[14] -
			mat[8] * mat[6] * mat[13] -
			mat[12] * mat[5] * mat[10] +
			mat[12] * mat[6] * mat[9];

		inv[1] = -mat[1] * mat[10] * mat[15] +
			mat[1] * mat[11] * mat[14] +
			mat[9] * mat[2] * mat[15] -
			mat[9] * mat[3] * mat[14] -
			mat[13] * mat[2] * mat[11] +
			mat[13] * mat[3] * mat[10];

		inv[5] = mat[0] * mat[10] * mat[15] -
			mat[0] * mat[11] * mat[14] -
			mat[8] * mat[2] * mat[15] +
			mat[8] * mat[3] * mat[14] +
			mat[12] * mat[2] * mat[11] -
			mat[12] * mat[3] * mat[10];

		inv[9] = -mat[0] * mat[9] * mat[15] +
			mat[0] * mat[11] * mat[13] +
			mat[8] * mat[1] * mat[15] -
			mat[8] * mat[3] * mat[13] -
			mat[12] * mat[1] * mat[11] +
			mat[12] * mat[3] * mat[9];

		inv[13] = mat[0] * mat[9] * mat[14] -
			mat[0] * mat[10] * mat[13] -
			mat[8] * mat[1] * mat[14] +
			mat[8] * mat[2] * mat[13] +
			mat[12] * mat[1] * mat[10] -
			mat[12] * mat[2] * mat[9];

		inv[2] = mat[1] * mat[6] * mat[15] -
			mat[1] * mat[7] * mat[14] -
			mat[5] * mat[2] * mat[15] +
			mat[5] * mat[3] * mat[14] +
			mat[13] * mat[2] * mat[7] -
			mat[13] * mat[3] * mat[6];

		inv[6] = -mat[0] * mat[6] * mat[15] +
			mat[0] * mat[7] * mat[14] +
			mat[4] * mat[2] * mat[15] -
			mat[4] * mat[3] * mat[14] -
			mat[12] * mat[2] * mat[7] +
			mat[12] * mat[3] * mat[6];

		inv[10] = mat[0] * mat[5] * mat[15] -
			mat[0] * mat[7] * mat[13] -
			mat[4] * mat[1] * mat[15] +
			mat[4] * mat[3] * mat[13] +
			mat[12] * mat[1] * mat[7] -
			mat[12] * mat[3] * mat[5];

		inv[14] = -mat[0] * mat[5] * mat[14] +
			mat[0] * mat[6] * mat[13] +
			mat[4] * mat[1] * mat[14] -
			mat[4] * mat[2] * mat[13] -
			mat[12] * mat[1] * mat[6] +
			mat[12] * mat[2] * mat[5];

		inv[3] = -mat[1] * mat[6] * mat[11] +
			mat[1] * mat[7] * mat[10] +
			mat[5] * mat[2] * mat[11] -
			mat[5] * mat[3] * mat[10] -
			mat[9] * mat[2] * mat[7] +
			mat[9] * mat[3] * mat[6];

		inv[7] = mat[0] * mat[6] * mat[11] -
			mat[0] * mat[7] * mat[10] -
			mat[4] * mat[2] * mat[11] +
			mat[4] * mat[3] * mat[10] +
			mat[8] * mat[2] * mat[7] -
			mat[8] * mat[3] * mat[6];

		inv[11] = -mat[0] * mat[5] * mat[11] +
			mat[0] * mat[7] * mat[9] +
			mat[4] * mat[1] * mat[11] -
			mat[4] * mat[3] * mat[9] -
			mat[8] * mat[1] * mat[7] +
			mat[8] * mat[3] * mat[5];

		inv[15] = mat[0] * mat[5] * mat[10] -
			mat[0] * mat[6] * mat[9] -
			mat[4] * mat[1] * mat[10] +
			mat[4] * mat[2] * mat[9] +
			mat[8] * mat[1] * mat[6] -
			mat[8] * mat[2] * mat[5];

		det = mat[0] * inv[0] + mat[1] * inv[4] + mat[2] * inv[8] + mat[3] * inv[12];

		if (det == 0)
			return false;

		det = 1.0f / det;

		for (i = 0; i < 16; i++)
			mat[i] = inv[i] * det;

		return true;
	}

	Matrix4 & operator = (const Matrix4 &Matrix4)
	{
		for (int i = 0; i < 16; i++) {
			mat[i] = Matrix4.mat[i];
		}
		return *this;
	}

	friend inline void RotateArbitraryLine(Matrix4 & pOut, Vector3f  v1, Vector3f  v2, float theta)
	{
		float a = v1.x;
		float b = v1.y;
		float c = v1.z;

		Vector3f p = v2 - v1;
		p = normalize(p);

		float u = p.x;
		float v = p.y;
		float w = p.z;

		float uu = u * u;
		float uv = u * v;
		float uw = u * w;
		float vv = v * v;
		float vw = v * w;
		float ww = w * w;
		float au = a * u;
		float av = a * v;
		float aw = a * w;
		float bu = b * u;
		float bv = b * v;
		float bw = b * w;
		float cu = c * u;
		float cv = c * v;
		float cw = c * w;

		float costheta = cosf(theta);
		float sintheta = sinf(theta);

		pOut.mat[0] = uu + (vv + ww) * costheta;
		pOut.mat[1] = uv * (1 - costheta) + w * sintheta;
		pOut.mat[2] = uw * (1 - costheta) - v * sintheta;
		pOut.mat[3] = 0;

		pOut.mat[4] = uv * (1 - costheta) - w * sintheta;
		pOut.mat[5] = vv + (uu + ww) * costheta;
		pOut.mat[6] = vw * (1 - costheta) + u * sintheta;
		pOut.mat[7] = 0;

		pOut.mat[8] = uw * (1 - costheta) + v * sintheta;
		pOut.mat[9] = vw * (1 - costheta) - u * sintheta;
		pOut.mat[10] = ww + (uu + vv) * costheta;
		pOut.mat[11] = 0;

		pOut.mat[12] = (a * (vv + ww) - u * (bv + cw)) * (1 - costheta) + (bw - cv) * sintheta;
		pOut.mat[13] = (b * (uu + ww) - v * (au + cw)) * (1 - costheta) + (cu - aw) * sintheta;
		pOut.mat[14] = (c * (uu + vv) - w * (au + bv)) * (1 - costheta) + (av - bu) * sintheta;
		pOut.mat[15] = 1;
	}

	inline float& operator [] (int Index)
	{
		return mat[Index];
	}
	inline float* operator & ()
	{
		return (float*)this;
	}
	friend inline Matrix4 operator * (const Matrix4 &Matrix1, const Matrix4 &Matrix2)
	{
		Matrix4 Matrix3;

		Matrix3.mat[0] = Matrix1.mat[0] * Matrix2.mat[0] + Matrix1.mat[4] * Matrix2.mat[1] + Matrix1.mat[8] * Matrix2.mat[2] + Matrix1.mat[12] * Matrix2.mat[3];
		Matrix3.mat[1] = Matrix1.mat[1] * Matrix2.mat[0] + Matrix1.mat[5] * Matrix2.mat[1] + Matrix1.mat[9] * Matrix2.mat[2] + Matrix1.mat[13] * Matrix2.mat[3];
		Matrix3.mat[2] = Matrix1.mat[2] * Matrix2.mat[0] + Matrix1.mat[6] * Matrix2.mat[1] + Matrix1.mat[10] * Matrix2.mat[2] + Matrix1.mat[14] * Matrix2.mat[3];
		Matrix3.mat[3] = Matrix1.mat[3] * Matrix2.mat[0] + Matrix1.mat[7] * Matrix2.mat[1] + Matrix1.mat[11] * Matrix2.mat[2] + Matrix1.mat[15] * Matrix2.mat[3];

		Matrix3.mat[4] = Matrix1.mat[0] * Matrix2.mat[4] + Matrix1.mat[4] * Matrix2.mat[5] + Matrix1.mat[8] * Matrix2.mat[6] + Matrix1.mat[12] * Matrix2.mat[7];
		Matrix3.mat[5] = Matrix1.mat[1] * Matrix2.mat[4] + Matrix1.mat[5] * Matrix2.mat[5] + Matrix1.mat[9] * Matrix2.mat[6] + Matrix1.mat[13] * Matrix2.mat[7];
		Matrix3.mat[6] = Matrix1.mat[2] * Matrix2.mat[4] + Matrix1.mat[6] * Matrix2.mat[5] + Matrix1.mat[10] * Matrix2.mat[6] + Matrix1.mat[14] * Matrix2.mat[7];
		Matrix3.mat[7] = Matrix1.mat[3] * Matrix2.mat[4] + Matrix1.mat[7] * Matrix2.mat[5] + Matrix1.mat[11] * Matrix2.mat[6] + Matrix1.mat[15] * Matrix2.mat[7];

		Matrix3.mat[8] = Matrix1.mat[0] * Matrix2.mat[8] + Matrix1.mat[4] * Matrix2.mat[9] + Matrix1.mat[8] * Matrix2.mat[10] + Matrix1.mat[12] * Matrix2.mat[11];
		Matrix3.mat[9] = Matrix1.mat[1] * Matrix2.mat[8] + Matrix1.mat[5] * Matrix2.mat[9] + Matrix1.mat[9] * Matrix2.mat[10] + Matrix1.mat[13] * Matrix2.mat[11];
		Matrix3.mat[10] = Matrix1.mat[2] * Matrix2.mat[8] + Matrix1.mat[6] * Matrix2.mat[9] + Matrix1.mat[10] * Matrix2.mat[10] + Matrix1.mat[14] * Matrix2.mat[11];
		Matrix3.mat[11] = Matrix1.mat[3] * Matrix2.mat[8] + Matrix1.mat[7] * Matrix2.mat[9] + Matrix1.mat[11] * Matrix2.mat[10] + Matrix1.mat[15] * Matrix2.mat[11];

		Matrix3.mat[12] = Matrix1.mat[0] * Matrix2.mat[12] + Matrix1.mat[4] * Matrix2.mat[13] + Matrix1.mat[8] * Matrix2.mat[14] + Matrix1.mat[12] * Matrix2.mat[15];
		Matrix3.mat[13] = Matrix1.mat[1] * Matrix2.mat[12] + Matrix1.mat[5] * Matrix2.mat[13] + Matrix1.mat[9] * Matrix2.mat[14] + Matrix1.mat[13] * Matrix2.mat[15];
		Matrix3.mat[14] = Matrix1.mat[2] * Matrix2.mat[12] + Matrix1.mat[6] * Matrix2.mat[13] + Matrix1.mat[10] * Matrix2.mat[14] + Matrix1.mat[14] * Matrix2.mat[15];
		Matrix3.mat[15] = Matrix1.mat[3] * Matrix2.mat[12] + Matrix1.mat[7] * Matrix2.mat[13] + Matrix1.mat[11] * Matrix2.mat[14] + Matrix1.mat[15] * Matrix2.mat[15];

		return Matrix3;
	}

	friend inline Vector3f operator * (const Matrix4 &mat4, const Vector3f &Vector)
	{
		Vector3f v;
		v.x = mat4.mat[0] * Vector.x + mat4.mat[4] * Vector.y + mat4.mat[8] * Vector.z;
		v.y = mat4.mat[1] * Vector.x + mat4.mat[5] * Vector.y + mat4.mat[9] * Vector.z;
		v.z = mat4.mat[2] * Vector.x + mat4.mat[6] * Vector.y + mat4.mat[10] * Vector.z;

		return v;
	}
};
