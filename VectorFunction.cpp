#include "VectorFunction.h"
#include <math.h>
#include <cmath>
#include <algorithm>
Vector2 Add(const Vector2& v1, const Vector2& v2) {
	Vector2 v;
	v.x = v1.x + v2.x;
	v.y = v1.y + v2.y;
	return v;
}

Vector2 Subtruct(const Vector2& v1, const Vector2& v2) {
	Vector2 v;
	v.x = v1.x - v2.x;
	v.y = v1.y - v2.y;
	return v;
}

Vector2 Multiply(float scalar, const Vector2& v) {
	Vector2 returnV;
	returnV.x = v.x * scalar;
	returnV.y = v.y * scalar;
	return returnV;
}

Vector3 Add(const Vector3& v1, const Vector3& v2) {
	Vector3 v;
	v.x = v1.x + v2.x;
	v.y = v1.y + v2.y;
	v.z = v1.z + v2.z;
	return v;
}

Vector3 Subtruct(const Vector3& v1, const Vector3& v2) {
	Vector3 v;
	v.x = v1.x - v2.x;
	v.y = v1.y - v2.y;
	v.z = v1.z - v2.z;
	return v;
}

Vector3 Multiply(float scalar, const Vector3& v) {
	Vector3 returnV;
	returnV.x = v.x * scalar;
	returnV.y = v.y * scalar;
	returnV.z = v.z * scalar;
	return returnV;
}

float Dot(const Vector3& v1, const Vector3& v2) { return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z; }

float Length(const Vector3& v) { return sqrtf(Dot(v, v)); }

Vector3 Nomalize(const Vector3& v) {
	float length = Length(v);
	if (length == 0) {
		return v;
	}
	return Multiply((1.0f / length), v);
}

Vector2 Lerp(const Vector2& v1, const Vector2& v2, float t) { return v1 + Multiply(t, v2 - v1); }

Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t)
{
	return v1 + Multiply(t,v2-v1);
}


Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t) 
{
	Vector3 a = Nomalize(v1), b = Nomalize(v2);
	float s = (1.0f - t) * Length(a) + t*Length(b);
	Vector3 e1, e2;
	e1 = float(1.0f / Length(a)) * a;
	e2 = float(1.0f / Length(b)) * b;

	float dot = std::clamp(Dot(a, b),0.0f,1.0f);
	float theta = std::acos(dot/*/( Length(a)*Length(b))*/);
	if (theta ==0.0f)
	{
		return Lerp(a,b,t);
	}
	return s*((std::sinf((1.0f-t)*theta)/std::sinf(theta))*a + (std::sinf(t*theta)/std::sinf(theta))*b);
}

Vector2 operator+(const Vector2& v1, const Vector2& v2) { return Add(v1, v2); }
Vector2 operator-(const Vector2& v1, const Vector2& v2) { return Subtruct(v1, v2); }
Vector2 operator*(float k, const Vector2& v) { return Multiply(k, v); }
Vector2 operator*(const Vector2& v, float k) { return Multiply(k, v); }


Vector3 operator+(const Vector3& v1, const Vector3& v2) { return Add(v1, v2); }
Vector3 operator-(const Vector3& v1, const Vector3& v2) { return Subtruct(v1, v2); }
Vector3 operator*(float k, const Vector3& v) { return Multiply(k, v); }
Vector3 operator*(const Vector3& v, float k) { return Multiply(k, v); }

Vector3 operator+=(Vector3& v1, Vector3& v2) { return v1 = Add(v1, v2); }
Vector3 operator+=(Vector3& v1, const Vector3& v2) { return v1 = Add(v1, v2); }

Vector3 operator-=(const Vector3& v1, const Vector3& v2) { return Subtruct(v1, v2); }
