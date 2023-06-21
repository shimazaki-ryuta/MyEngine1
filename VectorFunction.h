#pragma once
//#include "Matrix.h"
#include "Vector2.h"
#include "Vector3.h"
Vector2 Add(const Vector2&, const Vector2&);
Vector2 Subtruct(const Vector2&, const Vector2&);
Vector2 Multiply(float, const Vector2&);
Vector3 Add(const Vector3&, const Vector3&);
Vector3 Subtruct(const Vector3&, const Vector3&);
Vector3 Multiply(float, const Vector3&);
float Dot(const Vector3&, const Vector3&);
float Length(const Vector3&);
Vector3 Nomalize(const Vector3&);

Vector2 Lerp(const Vector2& v1, const Vector2& v2, float t);
Vector3 Lerp(const Vector3& v1, const Vector3& v2, float t);
Vector3 Slerp(const Vector3& v1, const Vector3& v2, float t);


Vector2 operator+(const Vector2&, const Vector2&);
Vector2 operator-(const Vector2&, const Vector2&);
Vector2 operator*(float k, const Vector2& v);
Vector2 operator*(const Vector2& v, float k);

Vector3 operator+(const Vector3&, const Vector3&);
Vector3 operator-(const Vector3&, const Vector3&);
Vector3 operator*(float k, const Vector3& v);
Vector3 operator*(const Vector3& v, float k);

Vector3 operator+=(Vector3&,Vector3&);
Vector3 operator+=(Vector3&, const Vector3&);
Vector3 operator-=(const Vector3&, const Vector3&);